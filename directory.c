#include "directory.h"
/*
Contains: 
    - is_path_to_dir()
    - initialize_directory()
    - get_dir_contents()
*/

/*
Purpose: Returns 1 if the path leads to an existing directory.
Description: 
    1) Checks the length of the string. If the length is greater than 260 (win MAX_PATH) chars then return 1.
    2) Check that the string is a real path on the system. Return 2 if not.
    3) Check that the path leads to a directory. Return 3 if not. 
    4) return 1 otherwise 
Expects: A pointer to a string.
Returns: int 
*/
int is_path_to_dir(const char *path) {
    if ( (strlen(path) + 1) > MAX_PATH) {      // Windows MAX_PATH is 260 including \0
        return 1;
    }    
    DWORD file_attribute =  GetFileAttributes(path);  // returns a FILE_ATTRIBUTE_CONSTANT
    if (file_attribute == INVALID_FILE_ATTRIBUTES) {
        return 2;
    } else if (file_attribute != FILE_ATTRIBUTE_DIRECTORY) {
        return 3;
    } else {
        return 0;
    }
}
/*
Purpose: initializes a directory struct and returns a pointer to it.
Description:
    1) Initializes a directory struct.
    2) sets initial items length, and capacity.
Recieves:
    - nothing.
returns: 
    - Directory * if successful.
    - NULL if failed.
*/
Directory *initialize_directory(void) {
    Directory *dir = (Directory *) malloc(sizeof(Directory));     //Allocate memory for the struct. malloc returns void * so (Directory *) type casts.
    
    if (dir == NULL) {
        fprintf(stderr, "Error: Malloc failed in directory.c during initialize_directory for Directory\n");
        return NULL;
    } 
    
    int initial_capacity = 7;
    dir->items = (Directory_Item *) malloc(initial_capacity * sizeof(Directory_Item)); 

    if (dir->items == NULL) {
        fprintf(stderr, "Error: Malloc failed in directory.c during initialize_directory for Directory.items\n");
        free(dir);                                              // free knows how much memory to free because of malloc
        return NULL;
    }

    dir->length = 0;
    dir->capacity = initial_capacity;
    return dir;
}
/*
Purpose: Dynamically appends the params to the end of the directory.items array resizing if needed.
Description:
    1) if the name length is longer than max name length return 1. 
    2) if size == length resize.
        - if capacity overflows unsigned int return 2
        - if malloc does not work return 3.
    3) add name to Directory.items.name.
    4) add type to Directory.items.type.
    5) increments length.
    6) for success return 0
Recieves:
    - Directory pointer.
    - char *name. Name of file or dir.
    - char *type. File or dir.
returns:
    - int.
*/
int append_to_dir_dynamic(Directory *dir, char *name, char *type, char* new_name) {
    if ((strlen(name) + 1) > MAX_NAME_LENGTH) {
        return 1;
    } else if (dir->length == dir->capacity) {
        int result = double_dir_items_capacity(dir);
        if (result == 1) {
            return 2;
        } else if (result == 2) {
            return 3;
        }
    }
    strcpy(dir->items[dir->length].cur_name, name);
    strcpy(dir->items[dir->length].type, type);
    strcpy(dir->items[dir->length].new_name, new_name);
    dir->length ++;
    return 0;
}
/*
Purpose: Double the capacity of the arry pointed too by directory.items and maintain the contents of the original directory.items array.
Description:
    1) if new array capacity is larger than unsigned int return 1 
    1) Mallocs a temp array double the size of dir.capacity.
        - if failed returns 2.
    2) copies values into the new array.
    3) points directory at new array.
    4) frees old array.
    5) update capacity.
    6) return 0.
Recieves: Directory *
returns: 
    - 0 for success.
    - 1 if malloc can't find the space.
*/
int double_dir_items_capacity(Directory *dir) {
    if (dir->capacity > UINT_MAX - dir->capacity) {
        return 1;
    }
    unsigned int new_capacity = dir->capacity * 2;
    unsigned int cur_capacity = dir->capacity;

    Directory_Item *new_items = (Directory_Item *) malloc(new_capacity * sizeof(Directory_Item));
    if (new_items == NULL) {
        return 2;
    }

    for (unsigned int i = 0; i < cur_capacity; i++) {
        strcpy(new_items[i].cur_name, dir->items[i].cur_name);
        strcpy(new_items[i].type, dir->items[i].type);
        strcpy(new_items[i].new_name, dir->items[i].new_name);
    }
    free(dir->items);
    dir->items = new_items;
    dir->capacity = new_capacity;
    return 0;
}
/*
Purpose: Looks at all items in the specified directory, for each item with whitespaces or capital letters in the item name add the info to the dict struct.
Description:
    1) validate that path param is a valid path to dir.
    2) use findFirstFile and interpret results.
    3) check the file name for whitespaces and capital letters.
    4) if found add the current path to item, item type, and new item path to the dir struct.
    5) repeat from set 2 untill.
        - all files in the directory have been looked at.
        - no memory left to add item names to dict struct.
Recieves: 
    - char *path_to_dir: path to the directory.
    - Directory *dir: struct pointer.
returns:
    - int.
        - 1 path is longer than max path.
        - 2 file can't be found.
        - 3 path does not lead to dir.
        - 4 path does not leave room for findFirstFile appending.
        - 5 invalid handle value on first call for findFirstFile.
        - 6 unsigned int overflow: dir capacity exceeded. not all items added to dir.
        - 7 malloc failed: items in dir exceed memory.
        - 0 upon success.
*/
int fill_item_names_to_clean(const char* path_to_dir, Directory *dir) {
    int result;
    char dir_fff_search_path[MAX_PATH];
    WIN32_FIND_DATA find_file_data;
    HANDLE handle_find = INVALID_HANDLE_VALUE;
    char dir_item_path[MAX_PATH];
    char tmp_clean_name[MAX_PATH];
    char type[MAX_TYPE_LENGTH];

    result = is_path_to_dir(path_to_dir);
    if (result == 1) {
        return 1;
    } else if (result == 2) {
        return 2;
    } else if (result == 3) {
        return 3;
    }

    if (strlen(path_to_dir) > MAX_PATH - 3) {    // make sure the dir is compatiable findFirstFile: space for "\", "*", and "0".
        return 4;
    }
    strcpy(dir_fff_search_path, path_to_dir);
    strcat(dir_fff_search_path, "\\*");

    handle_find = FindFirstFile(dir_fff_search_path, &find_file_data);
    
    if (handle_find == INVALID_HANDLE_VALUE) {
        return 5;
    }

    do  {
        if (strcmp(find_file_data.cFileName, ".") == 0 || strcmp(find_file_data.cFileName, "..") == 0) {
            continue;
        } else {
            strcpy(dir_item_path, path_to_dir);
            strcat(dir_item_path, "\\");
            strcat(dir_item_path, find_file_data.cFileName);
        }

        if (clean_item_name(dir_item_path, tmp_clean_name) == 0) {
            
            if (find_file_data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
                strcpy(type, "Directory");
            } else {
                strcpy(type, "File");
            }

            result = append_to_dir_dynamic(dir, dir_item_path, type, tmp_clean_name);
            if (result == 2) {
                FindClose(handle_find);
                return 6;
            } if (result == 3) {
                FindClose(handle_find);
                return 7;
            }
        }
    }
    while (FindNextFile(handle_find, &find_file_data) != 0);
    FindClose(handle_find);
    return 0;
}
/*
Purpose: If the item name has whitespaces or capital letters replace with underscores and lowercase.
Description: 
    1) extract the item name from the path.
    2) parse the item name if for whitespaces and capital letters adding the chars to the new_name array.
Recieves:
    char * path_to_item[MAX_PATH]
    char * new_name[MAX_PATH]
Returns:
    1 if no changes are made
    0 if changes are made.
*/
int clean_item_name(char *cur_path, char *new_path) {
    int item_name_index;
    int cur_path_len = strlen(cur_path);
    int change_made = 0;

    // find the index of the first char of the file name.
    for (int i = cur_path_len; i > -1 && cur_path[i] != '\\'; i--) {
        item_name_index = i;
    }
    
    // copy the cur_path into new_path from the beginning to just before the item_name_index
    for (int i = 0; i <= MAX_PATH && i < item_name_index; i++) {
        new_path[i] = cur_path[i];
    }

    // for copy the rest of the item name by converting whitespaces and uppercase this includes \0
    for (int i = item_name_index; i <= MAX_PATH && i <= cur_path_len; i++) {
        char cur_char = cur_path[i];
        if (cur_char == ' ') {
            change_made = 1;
            new_path[i] = '_';
        } else if (isupper(cur_char) != 0) {
            change_made = 1;
            new_path[i] = tolower(cur_char);
        } else {
            new_path[i] = cur_char;
        }
    }
    if (change_made == 0) {
        return 1;
    } else {
        return 0;
    }
}