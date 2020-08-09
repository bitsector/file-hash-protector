#!/bin/sh

rm -f /usr/local/bin/hash_shield_main

rm -f /etc/systemd/system/hash-shield-startup.service

rm -f /etc/hs_hash_storage

make clean
