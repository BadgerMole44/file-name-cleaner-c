/*
Purpose: This header file contains data types, functions, macros, and headers related to directories.
    Headers: 
        - listed below.
    Data Types:
        1) Directory_Item: Items 1) have a name, and 2) are files or dirs.
        2) Directory: 1) a list of directory items (hard coded to have a maximum of 50), and 2) a directory size (initally 0)
    Functions:
        1) is_path_to_dir(): directory.c
        2) initialize_directory: directory.c
        3) append_to_dir_dynamic: directory.c
        4) double_dir_items_capacity: directory.c
        5) get_dir_contents(): directory.c

*/
#ifndef DIRECTORY_H    // Guard: prevents multiple inclusions of header file. (IF NOT DEFINED)
#define DIRECTORY_H    // Guard: defines the macro DIRECTORY_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <tchar.h>
#include <ctype.h>
#include <windows.h>


#define MAX_NAME_LENGTH MAX_PATH
#define MAX_TYPE_LENGTH 10
typedef struct {
    char cur_name[MAX_NAME_LENGTH];
    char type[MAX_TYPE_LENGTH];
    char new_name[MAX_NAME_LENGTH];
} Directory_Item;

typedef struct {
    Directory_Item *items;
    unsigned int length;
    unsigned int capacity;
} Directory;

int is_path_to_dir(const char *path);
Directory *initialize_directory(void);
int append_to_dir_dynamic(Directory *dir, char *name, char *type, char* new_name);
int double_dir_items_capacity(Directory *dir);
int fill_item_names_to_clean(const char* path_to_dir, Directory *dir);
int clean_item_name(char *cur_path, char *new_path);




#endif // DIRECTORY_H                           Guard: closes ifndef