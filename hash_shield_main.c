#include "hash_shield.h"
#include "hash_core.h"
#include "fs_notification.h"
#include "tests.h"



static const char usage_str[] = " --help : display this message\r\n"
								" --add-path <path>: set or modify existing files hash\r\n"
								" --remove-path <path>: remove path from hash list\r\n" // TODO - test
								" --run-tests : run tests (THIS WILL DELETE ALL HASHED CONTENT SO FAR!!)\r\n"
								" --run-shield : start running hash_shield\r\n" // TODO - test
								" --reset : clears all stored files paths and their hash values\r\n";



void usage(){
	printf("hash_shield usage: \r\n");
	printf("%s\r\n",usage_str);

}
void add_path(const char* path){
	int res = 0;
	printf("add_path: %s\r\n",path);
	res = add_file_to_hash_list(path);	
	if (res < 0){
		printf("add_path: %s - failed\r\n",path);
	}else{
		printf("add_path: %s - success\r\n",path);
	}
}

// TODO - test
void remove_path(){
	printf("remove_path: \r\n");

}


void start_shield(){

	printf("start_shield: \r\n");
	
	run_inotify();

}
void reset(){
	int res = reset_hash_file();
	if (res == 0){
		printf("%s() success\r\n",__func__);
	}else{
		printf("%s() failed\r\n",__func__);
	}
}
// TODO - test this
static int check_hash_storage(){
	int res = 0;
	if (!does_exist(HASH_FILE_PATH)){
		LOGE("hash storage file does not exist! attempting to create...");
		res = write_to_file(HASH_FILE_PATH,"");
		if (res == -1){
			LOGE("could not create hash storage %s, is the program runnign with the right permissions?");
			return -1;
		}
	}
	return 0;
}
int main(int argc, char* argv[]){
	if (check_hash_storage() == -1){
		LOGE("could not get hash storage!!");
		return -1;
	}
	
	if (argc == 2){
		if (strncmp(argv[1],"--help",sizeof("--help")) == 0){
			usage();
		}else if (strncmp(argv[1],"--run-tests",sizeof("--run-tests")) == 0){
			run_all_tests();
		}else if (strncmp(argv[1],"--reset",sizeof("--reset")) == 0){
			reset();
		}else if (strncmp(argv[1],"--run-shield",sizeof("--run-shield")) == 0){
			start_shield();
		}else if (strncmp(argv[1],"--add-path",sizeof("--add-path")) == 0){
			printf("--add-path: missing <path> \r\n");
			usage();
		}else if (strncmp(argv[1],"--remove-path",sizeof("--remove-path")) == 0){
			printf("--remove-path: missing <path> \r\n");
			usage();
		}else{
			printf("unknown option: %s\r\n", argv[1]);
			usage();
		}
	}else if (argc == 3){
		
		if (strncmp(argv[1],"--add-path",sizeof("--add-path")) == 0){
			add_path(argv[2]);
		}else if (strncmp(argv[1],"--remove-path",sizeof("--remove-path")) == 0){
			remove_path();
		}else{
			printf("unknown option: %s\r\n", argv[1]);
			usage();
		}
		
	}else{
		printf("unknown option: %s\r\n", argv[1]);
		usage();
	}
	return 0;
}
