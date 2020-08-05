#ifndef HS_UTIL_H
#define HS_UTIL_H

int does_exist(const char* path);
char* get_file(const char* file_path,size_t *len);
char* str_to_hex(const unsigned char* src,size_t len,char* dst);
unsigned char* hex_to_str(const char* src, size_t len, unsigned char* dst);
void print_as_hex(const unsigned char* str, size_t len);
int append_to_file(const char* path,const char* data);
int write_to_file(const char* path,const char* buffer);
int is_in_file(const char* path,const char* needle);
const char* get_dir_from_path(const char* path, char* dir_only_path);
int is_dir_path(const char* path);
//int print_file(const char* path);
int get_line_num_in_file(const char* path);

#endif  // HS_UTIL_H
