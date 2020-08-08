#include "hash_core.h"

#ifdef DBG
#undef DBG
#define DBG 1
#endif

// tested
// empties the hash list file
int reset_hash_file(){
	// TODO - possibly a better solution is to write 0x05 EOF ascii
	return write_to_file(HASH_FILE_PATH,"");
}
//tested
//returns the address of hash_dst
unsigned char* hash_buffer(unsigned char* hash_dst,unsigned const char* buf, size_t len){
	return SHA512(buf, len , hash_dst);
}
//tested
//returns the address of hash_dst contining the hash value of the contents of a file
unsigned char* hash_a_file(const char* path,unsigned char* hash_dst){
	char *file_contents = NULL;
	size_t contents_len = 0;
	
	if (!hash_dst || ! path){
		LOGE("invalid argument"); 
		return NULL;
	}
	
	file_contents = get_file(path,&contents_len);

	if (!file_contents){
		LOGE("opening file: %s failed",path); 
		return NULL;
	}

	//LOGS("file contents: %s, len: %d",file_contents,contents_len); 
	hash_buffer(hash_dst,file_contents,contents_len);
	
	free(file_contents);
	return hash_dst;
}
// tested
char* hash_a_file_as_hex(const char* path,char* hex_hash_dst){
	unsigned char* res = NULL;
	unsigned char temp_hash[SHA512_DIGEST_LENGTH] = {0};
	if (!hex_hash_dst || ! path){
		LOGE("invalid argument"); 
		return NULL;
	}
	
	res = hash_a_file(path,temp_hash);
	
	if (!res){
		LOGE("could not get initial hash, something terribly wrong has happened"); 
		return NULL;
	}
	
	res = str_to_hex(temp_hash,SHA512_DIGEST_LENGTH,hex_hash_dst);
	
	if (!res){
		LOGE("could not convert to hex, something terribly wrong has happened"); 
		return NULL;
	}
	
	return hex_hash_dst;
	
	
}
// tested
char* get_path_and_hash_line(const char* path,char* dst){
	void* rc = NULL;
	unsigned char original_hash[SHA512_DIGEST_LENGTH+1] = {0};
	unsigned char hex_res[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	if (!path || !dst) return NULL;
	rc = hash_a_file(path,original_hash);
	if (rc != original_hash){
		LOGE("hash_a_file() failed: %p, hex_res: %p\r\n",rc,original_hash);
		return;
	}
	
	//LOGS("hash: %s\r\n, converting to hex...\r\n",rc);

    rc = str_to_hex(original_hash, SHA512_DIGEST_LENGTH,hex_res);
	if (rc != hex_res){
		LOGE("digest to hex failed! result returned: %p, hex_res: %p\r\n",rc,hex_res);
		return;
	}
	
	snprintf(dst,strlen(path)+SHA512_HEX_DIGEST_LENGTH+4,"%s:%s\r\n",path,hex_res);
	
	/*
	LOGS("dst: %s",dst,strlen(dst),strlen(path)+SHA512_HEX_DIGEST_LENGTH+4);
	LOGS("calculated length: %d",strlen(path)+SHA512_HEX_DIGEST_LENGTH+4);
	LOGS("strlen(dst): %d",strlen(dst));
	LOGS("strlen(path): %d",strlen(path));
	*/
	return dst;


}

//tested
// returns 1 if path is a file path in hash file
int is_path_in_hash_file(const char* path){
	int ret = 0;
	size_t len = 0;
	char *hash_file_buffer = NULL;
	char* start = NULL;
	if (!path) return 0;
	
	hash_file_buffer = get_file(HASH_FILE_PATH,&len);
	
	if (!hash_file_buffer){
		LOGE("could not get hash file");
		return 0;
	}
		
	start = strstr(hash_file_buffer,path);
	
	ret = ( (start != NULL) && (start[strlen(path)] == ':'));
	
	free(hash_file_buffer);

	return ret;
	
}
// tested
// adds a file path and it's hex-hash value to hash values file
int add_file_to_hash_list(const char* path){
	void* rc = 0;
	int res = 0;
	char new_hash_line[PATH_MAX+SHA512_HEX_DIGEST_LENGTH+4] = {0};
	if (!path || !does_exist(path)){
		LOGE("invalid path: %s\r\n", path);
		return -1;	
	}
	
	if (is_file_path_in_hash_file(path)){
		
		LOGS("path %s already in file, will calculate hash anew");
		
		res = remove_path_from_hash_list(path);
		
		if (res < 0){
			LOGE("could not remove existing file %s entry from file even though it's there");
			return -1;	
		
		}
		
	}

	rc = get_path_and_hash_line(path,new_hash_line);
	
	if (!rc){
		LOGE("get_path_and_hash_line() failed\r\n");
		return -1;	
	}
	
	LOGS("appending: %s",new_hash_line);
	
	res = append_to_file(HASH_FILE_PATH,new_hash_line);
	
	if (res<0){
		LOGE("append_to_file() failed\r\n");
		return -1;	
	}

	return 0;
}
// tested
// gets the stored hash value of a file
unsigned char* get_hash_of_file_from_list(const char* path, unsigned char* stored_hash){
	size_t len = 0;
	char *hash_file_buffer = NULL;
	char *path_start = NULL;
	char *hash_start = NULL;
	if (!path || !stored_hash){
		LOGE("invalid argument");
		return NULL;
	}
	if (!is_file_path_in_hash_file(path)){
		LOGE("no hash entry stored for %s",path);
		return NULL;
	}
		
	hash_file_buffer = get_file(HASH_FILE_PATH,&len);
	
	if (!hash_file_buffer){
		LOGE("could not get hash file");
		return NULL;
	}
	
	path_start = strstr(hash_file_buffer,path);

	if (!path_start){
		LOGE("%s not found in hash file contents, this should not happen\r\n",path);
		free(hash_file_buffer);
		return NULL;
	}
	
	hash_start = strstr(path_start,":");

	if (!hash_start){
		LOGE("could not find \":\" saparator, this should not happen, file possibly corrupted\r\n");
		free(hash_file_buffer);
		return NULL;
	}
	
	strncpy(stored_hash,hash_start+1,SHA512_HEX_DIGEST_LENGTH);
	
	LOGS("stored hash: %s\r\n",stored_hash);
	
	free(hash_file_buffer);
	
	return stored_hash;
	
}
//tested
int remove_path_from_hash_list(const char* path){
	size_t len = 0;
	char* hash_file_buffer = NULL;
	char* new_file_buffer = NULL;
	char* path_start = NULL;
	size_t i = 0;
	LOGE("started");
	if (!path){
		LOGE("invalid argument\r\n");
		return -1;
	}
	if (!is_file_path_in_hash_file(path)){
		LOGE("no hash entry stored for %s\r\n",path);
		return -1;
	}
		
	hash_file_buffer = get_file(HASH_FILE_PATH,&len);
	
	if (!hash_file_buffer){
		LOGE("could not get hash file\r\n");
		return -1;
	}
	
	path_start = strstr(hash_file_buffer,path);

	if (!path_start){
		LOGE("%s not found in hash file contents, this should not happen\r\n",path);
		free(hash_file_buffer);
		return -1;
	}
	
	new_file_buffer = (char*)malloc( sizeof(char)*(len - (strlen(path) + SHA512_HEX_DIGEST_LENGTH + 4) +1) );
	
	if (!new_file_buffer){
		LOGE("could not allocate new buffer, out of memory\r\n");
		free(hash_file_buffer);
		return -1;
	}
	LOGS("new_file_buffer allocation size: %d, times sizeof: %d",
		(len - (strlen(path) + SHA512_HEX_DIGEST_LENGTH + 4)),
		sizeof(char)*(len - (strlen(path) + SHA512_HEX_DIGEST_LENGTH + 4)));
		
	memset(new_file_buffer,0,sizeof(char)*(len - (strlen(path) + SHA512_HEX_DIGEST_LENGTH + 4)+1));
	
	while (hash_file_buffer + i < path_start){
		new_file_buffer[i] = hash_file_buffer[i];
		i++;
	};
		
	while ( i + ( strlen(path) + SHA512_HEX_DIGEST_LENGTH + 4) < len){
		new_file_buffer[i] = hash_file_buffer[i + (strlen(path) + SHA512_HEX_DIGEST_LENGTH + 4)-1];
		i++;
	};
	
	LOGS("old file:\r\n%s\r\n",hash_file_buffer);
	LOGS("new file:\r\n%s\r\n",new_file_buffer);

	write_to_file(HASH_FILE_PATH,new_file_buffer);
	
	free(new_file_buffer);
	free(hash_file_buffer);

	return 0;
	
}
int check_all_existing_hashes(){
	size_t len = 0;
	int num_paths = 0;
	char curr_path[PATH_MAX+1] = {0};
	char stored_hex_hash[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	char curr_hex_hash[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	char* hash_file_buffer = NULL;
	char* line_start = NULL;
	char* next_break = NULL;
	char* res = NULL;
	int i = 0;

	num_paths = get_line_num_in_file(HASH_FILE_PATH);

	if (num_paths == 0){
		LOGE("no files to watch");
		return 0;
	}else if (num_paths < 0){
		LOGE("error getting paths");
		return -1;
	}

	
	hash_file_buffer = get_file(HASH_FILE_PATH,&len);
	
	if (!hash_file_buffer){
		LOGE("could not get hash file");
		return 0;
	}
	
	
	
	// go over all paths and compare their current hash
	// to stored hash
	line_start = hash_file_buffer;
	next_break = strstr(line_start,":");
	while( i < num_paths){ // in effect num lines
		memset(curr_path,0,sizeof(curr_path));
		memset(stored_hex_hash,0,sizeof(stored_hex_hash));
		memset(curr_hex_hash,0,sizeof(curr_hex_hash));
		strncpy(curr_path,line_start,next_break-line_start);
		LOG("curr_path: %s",curr_path);
		strncpy(stored_hex_hash,next_break+1,SHA512_HEX_DIGEST_LENGTH);
		LOG("it's stored hash: %s",stored_hex_hash);
		res = hash_a_file_as_hex(curr_path,curr_hex_hash);

		if (!res){
			LOGE("could not calculate hash as hex for: %s",curr_path);
			free(hash_file_buffer);
			return -1;
		}
		
		LOGS("--comparison--");
		
		if (strncmp(stored_hex_hash,curr_hex_hash,SHA512_HEX_DIGEST_LENGTH) != 0 ){
			LOG_ATTENTION("ATTENTION: %s was modified!!",curr_path);
		}else{
			LOG_OK("%s not modified",curr_path);
		}

		if (i < num_paths -1){
			line_start = strstr(next_break+1,"/");
			next_break = strstr(line_start,":");
		}

		i++;
	}	
	
	free(hash_file_buffer);

	return 0;
	
}
