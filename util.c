#include "hash_shield.h"

#ifdef DBG
#undef DBG
#define DBG 1
#endif

// returns 1 if the file at @path exists or 0 otherwise
int does_exist(const char* path){
	return access( path, F_OK ) != -1;
}
// converts a binary string @src to hex equivalent in @dst
// returns a pointer to @dst upon success or NULL for failure
char* str_to_hex(const unsigned char* src,size_t len,char* dst){
	if (!src || !dst) return NULL;
	for (int i = 0, j = 0; i < len ; ++i, j += 2){
		sprintf(dst + j, "%02x", src[i] & 0xff);
	}
	
	return dst;
}
// converts a hex style string in @src to regualr binatry string @dst
// returns a pointer to @dst upon success or NULL on failure
unsigned char* hex_to_str(const char* src, size_t len, unsigned char* dst){
	if (!src || !dst) return NULL;
	for (int i = 0, j = 0; j < len; ++i, j += 2) {
		int val[1];
		sscanf(src + j, "%2x", val);
		dst[i] = val[0];
		dst[i + 1] = '\0';
	}
	return dst;
}
// prints a string @str of length @len as hex
void print_as_hex(const unsigned char* str, size_t len){
	void* rc = NULL;
	char hex[SHA512_HEX_DIGEST_LENGTH] = {0};
	rc = str_to_hex(str,SHA512_DIGEST_LENGTH,hex);
	if (rc != hex){
		LOGE("printing as hex failed\r\n");
		return;
	}
	LOGS("\r\n'%s' in hex is \r\n%s\r\n", str, hex);
}
/* returns a malloced buffer with the contents of the file in  
* @file_path and sets @len to be the buffer's size.
* The buffer must be freed when done using it
*/
char* get_file(const char* file_path,size_t *len){
    int fd,file_size,rc;
    char* buffer;
    FILE* file_object;
    struct stat st;

    fd = open(file_path,O_RDONLY); // "r" should be enough for fseek
    if (fd == -1){
      LOGE("open() error: %d, %s\r\n",errno,strerror(errno));
      return NULL;
    }
    if ((fstat(fd, &st) != 0) || (!S_ISREG(st.st_mode))) {
      close(fd);
      LOGE("fstat() error: not standard file: %d, %s\r\n",errno,strerror(errno));
      return NULL;
    }
    file_object = fdopen(fd, "r");
    if (file_object == NULL) {
      close(fd);
      LOGE("fdopen() error: %d, %s\r\n",errno,strerror(errno));
      return NULL;
    }

    if (fseeko(file_object, 0 , SEEK_END) != 0) {
      fclose(file_object);
      close(fd);
      LOGE("fseeko() error: %d, %s\r\n",errno,strerror(errno));
      return -1;
    }

    file_size = ftello(file_object);
    if (file_size == -1) {
      fclose(file_object);
      close(fd);
      LOGE("ftello() error: %d, %s\r\n",errno,strerror(errno));
      return NULL;
    }

    if (fseeko(file_object, 0 , SEEK_SET) != 0) {
      fclose(file_object);
      close(fd);
      LOGE("fseeko() error: %d, %s\r\n",errno,strerror(errno));
      return NULL;
    }

    buffer = (char*)malloc(sizeof(char)*(file_size+1));
    buffer[file_size] = '\0';
    if (buffer == NULL) {
        fclose(file_object);
        close(fd);
        LOGE("out of memory error: %d, %s\r\n",errno,strerror(errno));
        return NULL;
    }
    rc = fread (buffer, 1, file_size, file_object);
    if (rc < file_size){
        free(buffer);
        fclose(file_object);
        close(fd);
        LOGE("fread() error: %d, %s\r\n",errno,strerror(errno));
        return NULL;
    }

    //LOGS("got from fread: %d, file_size: %d,content_length: %d, allocated: %d\r\n",rc,file_size,strlen(buffer),sizeof(char)*(file_size+1));

    fclose(file_object);
    close(fd);
	*len = file_size+1;
	return buffer;

}

// appendt to the file at @appent_to_path the contents 
// of @data
// returns 0 upon success or -1 upon failure
int append_to_file(const char* append_to_path,const char* data){
	FILE* f_ptr = NULL;
	size_t res = 0;
	if (!append_to_path || !does_exist(append_to_path)){
		LOGE("invalid path: %s\r\n", append_to_path);
		return -1;	
	}
    //LOG("append_to_path: %s",target_path); 
	
	f_ptr = fopen(append_to_path, "a+");


    if (f_ptr == NULL){
        LOGE("fopen %s error\r\n", append_to_path);
        return -1;
    }
	
    // Append data to file 
    res = fprintf(f_ptr,"%s", data);
 	
	fclose(f_ptr);
	
	return 0;
}

// writes (overwrites!) the file at @path with the contents 
// of @buffer.
// returns 0 upons success or -1 upon failure
int write_to_file(const char* path,const char* buffer){
	FILE * fp;
	int res = 0;
	if (!path || !buffer){
		LOGE("invalid argument\r\n");
		return -1;
	}

	fp = fopen (path,"w");

	if (!fp){
		LOGE("could not open %s\r\n", path);
		return -1;
	}

	res = fprintf(fp,buffer);

	if (res < 0){
		LOGE("failed to write: %s",strerror(errno));
		fclose (fp);
		return -1;
	}
	
	//LOGS("written: %d characters",res);
	//LOGS("written: content %s",buffer);
	
	fclose (fp);
	
	return 0;
}
// tested
// returns 1 if needle is in the file pointed by path or 0 otherwise
// NOTE: string can easily be substrings of hex-hash values
int is_in_file(const char* path,const char* needle){
	size_t len = 0;
	char* file_buff = NULL;
	int res = 0;
	if (!path || !needle){
		LOGE("invalid argument\r\n");
		return -1;
	}

	file_buff = get_file(path,&len);

	if (!file_buff){
		LOGE("get_file() failed\r\n");
		return -1;
	}

	res = strstr(file_buff,needle) != NULL;
	
	free(file_buff);
	
	return res;
	
}

// extracts only he directory path portion from @path and stores it in
// @dir_only_path.
// return @dir_only_path upon success of NULL upon failure 
const char* get_dir_from_path(const char* path, char* dir_only_path){
	DIR* dir = NULL;
	char* p = NULL;
	size_t i = 0;
	if (!path || !dir_only_path){
		LOGE("invalid argument\r\n");
		return NULL;
	}
	
	dir = opendir(path);
    
    if(dir != NULL){
		LOGE("path %s itself is dir!\r\n",path);
		closedir(dir);
		return NULL;
    }
	
	dir = NULL;
	
	p = path+strlen(path)-1;
	LOGS("*p is %c\r\n",*p);
	while (p>=path && *p != '/') p--;
	while (p>path){
		dir_only_path[i] = *path;
		path++;
		i++;
	}
	
	LOGE("dir path is: %s, will try to open\r\n",dir_only_path);
	dir = opendir(dir_only_path);

    if (dir == NULL){

		if(errno == ENOTDIR){
			LOGE("%s not a directory, len: %d\r\n",dir_only_path,strlen(dir_only_path));
		}else{
			LOGE("error: %s\r\n",strerror(errno));
		}
		return NULL;

	}else{

		LOGE("dir %s verified\r\n",dir_only_path);
		closedir(dir);
		return dir_only_path;
    }


	LOGE("something went wrong, could not open dir: %s \r\n",dir_only_path);
    return NULL;
	
}
//return 1 if path is of a directory (and not of a file or some other 
// file system entity)
int is_dir_path(const char* path){
	int res = 0;
	DIR* dir = NULL;
	if(!path){
		LOGE("invalid argument\r\n");
		return 0;
	}	

	dir = opendir(path);
    
    res = (dir != NULL);
    
	closedir(dir);

	return res;
	
}	
// return the number of lines in a file pointed by @path
// returns number of lines upon success or -1 upon failure
int get_line_num_in_file(const char* path){
	size_t len = 0;
	int count = 0;
	size_t i = 0;
	char* file_buffer = NULL;
	
	if (!path){
		LOGE("invalid argument");
		return -1;
	}
	
	file_buffer = get_file(path,&len);
	
	if (!file_buffer){
		LOGE("could not get file: %s",path);
		return -1;
	}
	
	while (i < len){
		if (file_buffer[i] == '\n') {
			count++;
		}
		i++;
	}
	
	free(file_buffer);
	
	return count;
	
}


