#include "fs_notification.h"

#ifdef DBG
#undef DBG
#define DBG 0
#endif

#define EVENT_SIZE  ( sizeof (struct inotify_event) )

#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

#define EVENT_MASK (IN_MODIFY | IN_CLOSE_WRITE)

// buffer where inotify events will be stored after each read from @inotify_fd
char buffer[EVENT_BUF_LEN] = {0};
// fd to read fs events from inotify mechanism
static int inotify_fd = 0;
// number of paths in watch file
static int num_paths = 0; 
// number of dirs actaully being watched in @paths_array
static int num_watched_dirs = 0; 
// dir path and corresponding watch number to match fs event to dir path
struct wd_dir_pair{
	int wd;
	char dir_path[PATH_MAX];
};
// an array containing all the paths of watched dir and their corresponding wds
static struct wd_dir_pair* paths_array = NULL;

// free all allocated memory in fs_notification.c
static void finalize(){
	if (paths_array) {
		for ( int i = 0  ; i < num_watched_dirs ; i++ ){
			if ( paths_array[i].wd != -1 ){
				inotify_rm_watch(inotify_fd,paths_array[i].wd);
			}
		}
		free(paths_array);
		paths_array = NULL;
	}
	if (inotify_fd){
		close(inotify_fd);
		inotify_fd = 0;
	}
}

// stop getting notifications form inotify mechanism
void stop_fs_notifications(){
    LOGS("stopping all inotify operations and releasing resources");
	finalize();
}

// auxilary function that prints the specific inotify event mask
static const char* get_exact_event_mask(unsigned int mask){
    switch(mask){
        case (IN_ACCESS):
            return "IN_ACCESS";

        case (IN_MODIFY):
            return "IN_MODIFY";

        case (IN_ATTRIB):
            return "IN_ATTRIB";

        case (IN_CLOSE_WRITE):
            return "IN_CLOSE_WRITE";

        case (IN_CLOSE_NOWRITE):
            return "IN_CLOSE_NOWRITE";

        case (IN_OPEN):
            return "IN_OPEN";

        case (IN_MOVED_FROM):
            return "IN_MOVED_FROM";

        case (IN_MOVED_TO):
            return "IN_MOVED_TO";

        case (IN_CREATE):
            return "IN_CREATE";

        case (IN_DELETE):
            return "IN_DELETE";

        case (IN_DELETE_SELF):
            return "IN_DELETE_SELF";

        case (IN_MOVE_SELF):
            return "IN_NONBLOCK,IN_MOVE_SELF";

        case (IN_UNMOUNT):
            return "IN_UNMOUNT";

        case (IN_Q_OVERFLOW):
            return "IN_Q_OVERFLOW";

        case (IN_IGNORED):
            return "IN_IGNORED";

        case (IN_CLOSE):
            return "IN_CLOSE";

        case (IN_MOVE):
            return "IN_MOVE";

        case (IN_ONLYDIR):
            return "IN_ONLYDIR";

        case (IN_DONT_FOLLOW):
            return "IN_DONT_FOLLOW";

        case (IN_EXCL_UNLINK):
            return "IN_EXCL_UNLINK";

        case (IN_MASK_ADD):
            return "IN_MASK_ADD";

        case (IN_ISDIR):
            return "IN_ISDIR";

        case (IN_ONESHOT):
            return "IN_ONESHOT";

        case (IN_CLOEXEC):
            return "IN_CLOEXEC";
        default:
            return "unknown";

    };
}
// auxilary function: print the content of @event
static void print_event(struct inotify_event *event){
    LOGS("event:");
    LOGS("watch: %d",event->wd);
    LOGS("mask: %d = %s",event->mask,get_exact_event_mask(event->mask));
    LOGS("cookie: %d",event->cookie);
    LOGS("len: %d",event->len);
    LOGS("name: %s",event->name);

}
// returns the dir path associated with the inotify watch @wd
static char* dir_path_only(int wd){
	int i=0;
	while (i<num_watched_dirs){
		if (paths_array[i].wd == wd){
			return paths_array[i].dir_path;
		}
		i++;
	}
	return NULL;
}

// core function:
// for all the modified in @event:
// 1. get the specific file and construct a full path
// from dir+filename
// 2. check if this full path is in hash storage (if the file is watched)
// 3. if the file is watched then compare the stored hash and the current
// hash
// 4. if the hashes differ it's an indication that the file was modified
// - in this case print an alert into stdout and syslog
static int handle_event(struct inotify_event* event){
	int i = 0;
	char entire_path[PATH_MAX] = {0};
	char stored_hash[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	char fresh_hash[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	char* dir_path = NULL;
	char* res = NULL;
	
	if (!event){
		LOGE( "event is NULL");
		return -1;
	}
	
	dir_path = dir_path_only(event->wd);
	
	if (!dir_path){ // dir not found, it's actually ok
		LOGS("dir not found for wd: %d",event->wd);
		return 0;
	}
	
	strncpy(entire_path,dir_path,PATH_MAX);
	strncat(entire_path,"/",2);
	strncat(entire_path,event->name,strlen(event->name));
	
	LOGS("constructed path: %s",entire_path);
	
	if (is_file_path_in_hash_file(entire_path)){
		
		// here compare hash of current file with the one in hash file
		
		res = get_hash_of_file_from_list(entire_path,stored_hash);
		
		if (!res){
			LOGE("could not get stored hash, something went terribly wrong");
			return -1;
		}
		
		res = hash_a_file_as_hex(entire_path,fresh_hash);
		if (!res){
			LOGE("could not calculate hash of %s contents, maybe we don't have access?");
			return -1;
		}
		
		
		LOGS("stored hash:");
		LOGS("%s",stored_hash);
		LOGS("fresh hash:");
		LOGS("%s",fresh_hash);
		
		
		if (strncmp(stored_hash,fresh_hash,SHA512_HEX_DIGEST_LENGTH) != 0 ){
			LOG_ATTENTION("ATTENTION: %s was modified!!",entire_path);
		}else{
			LOG_OK("%s not modified",entire_path);
		}
		
	}else{
		LOGS("path %s not found in hash file",entire_path);
	}

	return 0;
}
// checks if the event to see it's relevant (read events dont modify
// files for example) and sends to handle_event() only the relevant
// events
static int parse_events(int length){
	int i = 0;
	while ( i < length ) {     
		struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
		if ( event->len ) {
			//print_event(event);
			if ( event->mask & IN_MODIFY ) {
				if ( event->mask & IN_ISDIR ) {
					LOGS( "event is IN_MODIFY on dir");
				}else {
					LOGS( "event is IN_MODIFY on file");
					handle_event(event);
				}
			}else if ( event->mask & IN_DELETE ) {
				// TODO - maybe handle this as well
				if ( event->mask & IN_ISDIR ) {
					LOGS( "event is IN_MODIFY on dir" );
				}
				else {
					LOGS( "event is IN_MODIFY on dir" );
				}
			}
		}
		i += EVENT_SIZE + event->len;
	}

	return 0;
}
// for each path in hash storage create a watch in the 
// inotify structure to keep an eye on changes in this path 
static int fill_with_paths(){
	size_t len = 0;
	char curr_path[PATH_MAX] = {0};
	char curr_dir_only[PATH_MAX] = {0};
	char* hash_file_buffer = NULL;
	char* line_start = NULL;
	char* next_break = NULL;
	char* res = NULL;
	int i = 0;
	
	hash_file_buffer = get_file(HASH_FILE_PATH,&len);

	if (!hash_file_buffer){
		LOGS("could not get hash file");
		return -1;
	}
	
	// go over all paths and get their dir
	line_start = hash_file_buffer;
	next_break = strstr(line_start,":");
	while( i < num_paths){ // in effect num lines
		memset(curr_path,0,sizeof(curr_path));
		memset(curr_dir_only,0,sizeof(curr_dir_only));
		strncpy(curr_path,line_start,next_break-line_start);
		LOGS("curr_path: %s",curr_path);
		res = get_dir_from_path(curr_path,curr_dir_only);
		if (!res){
			LOGE("could not get dir from path: %s",curr_path);
			free(hash_file_buffer);
			finalize();
			return -1;
		}
		LOGS("curr_dir_only: %s",curr_dir_only);
		strncpy(paths_array[i].dir_path,curr_dir_only,PATH_MAX);
		LOGS("paths_array[i].dir_path: %s",paths_array[i].dir_path);
		if (i < num_paths -1){
			line_start = strstr(next_break+1,"/");
			next_break = strstr(line_start,":");
		}
		i++;
	}	
	
	free(hash_file_buffer);
	return 0;
}
// initiates inotify structure with all the path in stored hashes file
// so all the changes on these paths trigger an inotify notification 
static int add_watches(){
	num_paths = get_line_num_in_file(HASH_FILE_PATH);

	if (num_paths == 0){
		LOGE("no files to watch");
		return -1;
	}else if (num_paths < 0){
		LOGE("error getting paths");
		return -1;
	}

	paths_array = (struct wd_dir_pair*)malloc(sizeof(struct wd_dir_pair)*num_paths);
	
	if (!paths_array){
		LOGE("malloc failed, out of memory");
		finalize();
		return -1;
	}
	
	memset(paths_array,0,sizeof(struct wd_dir_pair)*num_paths);
	
	fill_with_paths(); 
	
	for (int i=0;i<num_paths;i++){
		paths_array[i].wd = inotify_add_watch( inotify_fd, paths_array[i].dir_path, EVENT_MASK);
		if (paths_array[i].wd == -1){
			LOGE("inorify_add_watch on %s failed!",paths_array[i].dir_path);	
			finalize();
			return -1;
		}
		LOGS("path: %s, wd: %d",paths_array[i].dir_path,paths_array[i].wd);
		num_watched_dirs++;
	}

	LOG("success! watched paths: %d in %d directories",num_paths,num_watched_dirs);
	return 0;
}

// inits and runs in in infinite loop a read from inotify fd
// this is a core funcion of the mechanism
int run_inotify(){
	int event_length = 0;
	int res = 0;
	
	inotify_fd = inotify_init();
	
	if (inotify_fd < 0){
		LOGE("inotify init error");
		return -1;
	}
	
	// init inotify watches for each dir path
	res = add_watches();
	
	if (res < 0){
		LOGE("add watches error");
		return -1;
	}

	// loop endleslly reading events, parsing them and responding
	// on hash mismatch
	do{
		
		event_length = read(inotify_fd,buffer, EVENT_BUF_LEN);
		
		res = parse_events(event_length);
		
		if (res < 0){
			LOGE("event parsing failed");
			finalize();
			return -1;
		}
		
	}while(1);
	
	return 0;
}
