#ifndef HASH_CORE_H
#define HASH_CORE_H

#include "hash_shield.h"
#include "util.h"

int reset_hash_file();
unsigned char* hash_buffer(unsigned char* hash_dst,unsigned const char* buf, size_t len);
unsigned char* hash_a_file(const char* path,unsigned char* hash_dst);
char* get_path_and_hash_line(const char* path,char* dst);
int is_path_in_hash_file(const char* path);
int add_file_to_hash_list(const char* path);
unsigned char* get_hash_of_file_from_list(const char* path, unsigned char* stored_hash);
char* hash_a_file_as_hex(const char* path,char* hex_hash_dst);
int check_all_existing_hashes();

#endif  // HASH_CORE_H
