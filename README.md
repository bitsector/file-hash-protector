# file-hash-protector

Real time file-system change monitoring software to detect virus attempts of corrupt files.

1. Intro	
2. Background
3. Assumptions
4. Software capabilities
5. Software implementation
6. How to execute


1. Intro

This document describes the function and implementation of a basic Hash based Antivirus software for Linux (Ubuntu 16.04). The purpose is to demonstrate an understanding of the existing mechanisms that countermeasure the spread of viruses on a system learned in the course 22923 Computer System Security in the Open University of Israel in Spring Semester of 2020.
This document will:
1. Provide background of what computer viruses are and how they work.
2. Describe the principles of an array of countermeasures to detect file changes by viruses.
3. Describe the specific implementations to these countermeasures in the project code.
4. Specify the assumptions on the system capability and environment.
5. Give an overview of code modular structure.
6. Explain the steps to compile, run, install and uninstall the software.

2. Background

Viruses and worms are forms of malicious software that “infects” computers and “propagate” themselves to other computers when possible. A virus running on an infected computer can perform malicious and undesired operation that can damage the system integrity and availability or compromise user data privacy and completeness. Viruses infect computers buy making changes to files. These changes are subtle and not readily detectable. However, these minor changes contain the virus’ payload – the malicious code that when executed causes damage or propagates the virus onward to other systems or devices. An example for this may be a modification of an executable binary file: a virus adds a few specific commands to the file and when it’s (the file) executed so is the virus’ code. Another example is corruption of a Word, OpenOffice or .pdf document: a virus changes the file by adding a macro that executes whenever the file is opened by the editing program. These modifications are such that even though an additional malicious code is executed – it does so in a secret and clandestine way – the user is not aware to the execution of the virus’ code. The changes the virus makes are such that the files looks and behaves the same from the standpoint of the user: the executable will still run and do its function as expected and the document will appear the same in the editing software.
Here lies the problem: how can one detect an unwanted modification, possibly made by a virus to a set of files, while this modification is designed to be undetectable?
My solution to this problem is Hash-Shield: a real time monitoring system that keeps a watch on a set of files and notifies the user (or admin) on any changes to them. It is a program written in c on Linux, installed and ran in the background as a service. It has a list of files and for each file on this list it calculates a sha512 hash digest on its content. It is possible to detect ANY change made to the files, since the change in the file’s content will be noticeable in a form of a completely different hash digest. When the system boots, the service will start running and check all the files on its list for changes by comparing the hash values when the file was added and its current hash value. If the values differ – it’s an indication that a modification has been made. This modification may be completely voluntary by the user – in this case the user will have to update the file’s hash (current value) by adding the path anew to the files watch list.
Another, complementary, mechanism is real time notification on file updates. The software uses the inotify Linux kernel mechanism which allows reading notification from a kernel object upon changes in the file system (fs). Hash Shield reads such notification on all files it watches. It runs in the background and waits for reception of these notifications. Once a notification on a watched file is received – a stored hash value vs. current hash value comparison is performed to detect modifications. If the hash values differ – a modification has been made. 

3. Assumptions

The software is meant to work on Linux systems. It’s tested on Ubuntu 16.04. The software is based on the inotify kernel utility (exists in most Linux distributions) and a Some glibc-Data-Checksums for sha512 digest calculation. It will write audit log entries into syslog (also available in most Linux distributions). Another assumption is that inotify mechanism detect ALL changes to the watched files and misses none.

4. Software capabilities

General Idea (The big picture) – The software runs on a Linux system as a system daemon. It starts early by systemd process – when only system daemons and scripts are started. User services start running later which reduces the risk of sabotage to the watched files and software executable and configuration files by user-installed malware. The software runs in the background all the time. It inspects a set of files and directories given by the system administrator and makes sure they are not modified. If a modification occurs (e.g. by a virus payload)– the software will immediately alert the system administrator and the user. A log entry (syslog and stdout) will be created for this event.
The software will save a sha512 hash value in hex format (for printability) for each watched file. When a file is modified the software will be notified by the inotify kernel utility. It will re calculate the hash of the file again and compare it to its stored record. If the file has been modified it will be detected by a different hash digests. This is the core idea of this software. It is extremely unlikely that a virus can attach its payload to a file while maintaining its hash digest as it was. 
The software will 
4.1 Boot as early as possible by systemd.
4.2 Will be installed in root directories and run as a system daemon (requires installation with sudo command).
4.3 Maintain a database of user defined file set in /etc/hs_hash_storage and corresponding sha512 hash values.
4.4 Utilize the inotify kernel utility to receive file access notifications in real time. Essentially – with the software runs it has an infinite loop on read() from the inotify fd (file descriptor) to get the events.
4.5 Alert the user in real time on suspicious file modification activity (possible indication of virus actions).
4.6 Write audits on said suspicious actions into syslog. The messages can be read from /var/log/auth.log.
4.7 Added features:
use of only safe functions e.g strncpy()
stack protection gcc flag in compilation -fstack-protector
hash file will be stored in /etc and accessible only to admin/root/sudo capable user
the executable will run with admin privileges having access to the entire fs
log messages will be sent to syslog auth facility (in ubuntu is in /var/log/auth.log)
as a part of the syslog utility, audit messages date, time and pid are also attached to each message
no -g flag in gcc compilation to leave no obvious clues to the software’s internal mechanism and structure and hinder de-compilation and reverse engineering

5. Software Implementations
The modules:

utli.c+util.h
  Basic rudimentary operations for interaction with the OS and string operations.
  Examples are open a file and read It’s content, verify that a file exists, append and write to file, as well as conversion form c-style string to and fro hex format.

hash_core.c+hash_core.h
  Responsible for all sha512 related operations including maintenance of the hash storage at /etc/hs_hash_storage. 
  Example of core operations: calculate hash value on file’s content, enter a path+hash pair to hash storage, remove path+hash pair from storage etc.

fs_notification.c+fs_notification.h
  Handles all the necessary operations to interact with inotify kernel interface. It gets an fd to read events from, initiates watches (each watch is associated with a directory) to get changes notifications on.

test.c+tests.h
  Unit tests I made to test during the development. It tests the main components and correctness of operations in all the modules.

private_log.h
  Logging c macros printing log messages into stdout and syslog.

hash_shield_main.c
  main() function and basic terminal UI

Makefile
  makefile to compile the program. Only two options here: make clean and make all.

install.sh+uninstall.sh
  Installation/uninstallation scripts. Install builds the modules and compiles the software, creates /etc/hs_hash_storage, copies the hash_shiled_main executable binary to /usr/local/bin/ and generates a boot time service for system to load in boot time. 
  Also activates the service in system.

hash-shield-startup.service
  A systemd service launch script. Copied by install.sh into /etc/systemd/system/.
  When the machine reboot the service will run in the background and print messages into syslog.

test_file+more_test_files
  directories with example files to watch, modify, and test program’s behavior.


6. How to execute

A.  Executing without installation:

1. compile:
	make all
2. add files to watch (ONLY USE FULL PATHS)
	sudo ./hash_shiled_main –add-path /path/to/file_1
	sudo ./hash_shiled_main –add-path /path/to/file_2
	…
	sudo ./hash_shiled_main –add-path /path/to/file_n
3. run the software
	sudo ./hash_shiled_main –run-shiled

The software is now running and waiting for an event on the watched files.

4. Change a file: echo “something” >> /path/to/file_1
Now a warning will appear:
ATTENTION /path/to/file_1 was modified!!
This warning is printed into stdout and into /var/lor/auth.log.
NOTE: if one echoes the files original content to itself – the file is not modified and therefor event though the event is detected – the file is not considered modified since its content is exactly the same. Also: echo -en “” >> /path/to/file_1 doesn’t modify the file’s content but is registered as a write event.

5. Remove files from watch list by 
	sudo ./hash_shield_main –remove-path /path/to/file_i
NOTE: don’t add or remove files during execution. The watched files are loaded only when the program starts. You can, however, “re-add” a file during execution. This will re-calculate the hash to that of the existing content.

B. Execute WITH installation:
1. Install
	sudo ./intall.sh
This will compile with Makefile, create /etc/hs_hash_storage, copy hash-shield-startup.service into /etc/sytemd/system, copy the executable hash_shield_main into /usr/local/bin/ and enable the service when system boots.

2. add files to watch (ONLY USE FULL PATHS)
	sudo ./hash_shiled_main –add-path /path/to/file_1
	sudo ./hash_shiled_main –add-path /path/to/file_2
	…
	sudo ./hash_shiled_main –add-path /path/to/file_n

3. Reset the machine, when it boots anew the software will be running in the background.

4. Change a file: echo “something” >> /path/to/file_1
Now a warning will appear:
ATTENTION /path/to/file_1 was modified!!
This warning is printed into stdout and into /var/lor/auth.log.

5. Uninstall: 
	sudo ./uninstall.sh
This will undo all the changes from step 1.
