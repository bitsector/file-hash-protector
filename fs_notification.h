#ifndef HS_INOTIFY_H
#define HS_INOTIFY_H

#include "hash_shield.h"
#include "util.h"

void stop_fs_notifications();
int run_inotify();

#endif  // HS_INOTIFY_H
