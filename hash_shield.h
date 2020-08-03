#ifndef HASH_SHIELD_H
#define HASH_SHIELD_H

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
//#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <openssl/sha.h>
#include <dirent.h>
#include <linux/inotify.h>
#include "private_log.h"

#define DBG 1

#define SHA512_HEX_DIGEST_LENGTH (2*SHA512_DIGEST_LENGTH)

#define HASH_FILE_PATH "/etc/hs_hash_storage" // TODO - obfuscate
#define TEST_FILE_1_PATH "/home/ak/mmn16/test_files/test_file_1" // TODO - obfuscate
#define TEST_FILE_2_PATH "/home/ak/mmn16/test_files/test_file_2" // TODO - obfuscate
#define TEST_FILE_3_PATH "/home/ak/mmn16/test_files/test_file_3" // TODO - obfuscate
#define TEST_FILE_4_PATH "/home/ak/mmn16/test_files/test_file_4" // TODO - obfuscate


#endif  // HASH_SHIELD_H
