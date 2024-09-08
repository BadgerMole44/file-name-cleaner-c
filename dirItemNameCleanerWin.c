#include "directory.h"

int get_yes_no(void);
int clear_stdin_buffer(char *str, int *str_len);
int is_yes_or_no(char *str);
int rename_items(Directory *dir);

/*
Purpose: Cleans the names of all files in a directory such that all names are: 1) lower case, 2) white spaces are converted to underscores.
Description: 
    1) executable is called on windowsOS.
    2) takes one param PATH to dir. prompt and exit.
    3) validates the argument. 
    4) reads names of items in the dir at PATH to an array.
    5) reads the type of item (file or dir) to a separate array.
    6) looks through the list of names and creates a new list of clean names
    7) displays current name, clean name, and file type or messages that all names are already clean.
*/
int main (int argc, const char *argv[]) {  // *argv[] is an array of pointers to chars
    int result;
    Directory *dir;
    int path_len;
    int item_name_index;
    char *item_name_ptr;
    char *new_item_name_ptr;
    
    if (argc != 2) {
        printf("Usage: Expects 1 argument string. ABSOLUTE_PATH to directory.\n");
        return 1;
    }

    result = is_path_to_dir(argv[1]);

    if (result == 1) {
        printf("Invalid Argument: ABSOLUTE_PATH exceeds windows MAX_PATH (%i chars including \\0).\n", MAX_PATH);
        return 2;
    } else if (result == 2) {
        printf("Invalid Argument: ABSOLUTE_PATH not found on system.\n");
        return 3;
    } else if (result == 3) {
        printf("Invalid Argument: ABSOLUTE_PATH does not lead to a directory.\n");
        return 4;
    }

    dir = initialize_directory();

    result = fill_item_names_to_clean(argv[1], dir);

    if (result == 4) {
        printf("Path name not compatible with FindFirstFile.\n");
        return 5;
    } else if (result == 5) {
        printf("Invalid Handle value on FindFirstFile initial call.\n");
        return 6;
    } else if (result == 6) {
        printf("Directory.capacity overflow: unsigned int capacity exceeded. Not all items at specified path added to Directory.items.\n");
    } else if (result == 7) {
        printf("Malloc failed during append_to_dir_dynamic: Not all items at specified path added to Directory.items.\n");
    }
    
    printf("item names at %s that will be changed\n\n", argv[1]);

    for (int i = 0; i < dir->length; i++) {
        path_len = strlen(dir->items[i].cur_name);
        for (int j = path_len; j > -1, dir->items[i].cur_name[j] != '\\'; j--) {
            item_name_index = j;
        }
        printf("Name: %s, ", &dir->items[i].cur_name[item_name_index]);
        printf("Clean Name: %s, ", &dir->items[i].new_name[item_name_index]);
        printf("Item Type: %s\n", dir->items[i].type);
    }
    printf("\nApply changes? (Enter y/n): ");

    if (get_yes_no() == 0) {
        printf("No changes applied.\n");
    } else {
        result = rename_items(dir);
        if (result == 0) {
            printf("All item name changes applied.\n");
        } else {
            printf("Some item name changes not applied.\n");
        }
    }
    return 0;
}

/*
Purpose: Get a yes or no input from the user.
Description:
    - fgets input
    - loops till yes or no input
Recieves: 
    - nothing.
Returns:
    - int: 0 for no input 1 for yes input
*/
int get_yes_no(void) {
    const int MAX_INPUT_LENGTH = 5;              // "yes\0\n"
    char in[MAX_INPUT_LENGTH];
    char *result = NULL;
    int in_len;

    while (result == NULL) {
        result = fgets(in, MAX_INPUT_LENGTH, stdin);
        if (result != NULL) {
            in_len = strlen(in);
            int exceeded_buffer = clear_stdin_buffer(in, &in_len);
            if (exceeded_buffer == 1) {
                result = NULL;
                printf("Invalid Input: Exceeded input buffer. Expects Yes or No: ");
            } else {
                int yes_or_no = is_yes_or_no(in);
                if (yes_or_no == -1) {
                    result = NULL;
                    printf("Invalid Input: Expects Yes or No: ");
                } else if (yes_or_no == 1) {
                    return 1;
                } else {
                    return 0;
                }
            }
        }
    }
}
/*
Purpose: Make sure the stdin input buffer is clear. Indicate if the fgets input length was exceeded.
Description:
    - check if \n char is in the str. 
        - if yes remove it an decrement the in_len.
        - if no clear the input buffer
Recieves:
    - char *str: string that fegts read to.
    - int *str_len: pointer to the length of the string.
Returns:
    - 1 if the length was exceeded
    - 0 if not
*/
int clear_stdin_buffer(char *str, int *str_len) {
    if (str[*str_len - 1] == '\n') {
        str[strcspn(str, "\n")] = 0;
        *str_len--;
        return 0;
    } else {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            continue;
        }
        return 1;
    }
}
/*
Purpose: determin if a string is yes or no. Case does not matter, y, Y, yes, YeS, etc.
Description:
    - convert the input to lowercase.
    - compare.
Recieves:
    - char *str.
Returns:
    - -1 not yes or no.
    - 0 no.
    - 1 yes.
*/
int is_yes_or_no(char *str) {
    int str_len = strlen(str);
    for (int i = 0; i < str_len; i++) {
        if (isupper(str[i]) == 1) {
            str[i] = tolower(str[i]);
        }
    }
    if (strcmp(str, "yes") == 0 || strcmp(str, "y") == 0 || strcmp(str, "ye") == 0) {
        return 1;
    } else if (strcmp(str, "no") == 0 || strcmp(str, "n") == 0) {
        return 0;
    } else {
        return -1;
    }
}
/*
Purpose: All items in the directory object are renamed according to the new clean name.
Description:
    - iterate through the dir and use the rename function.
Recieves:
    - Directory *dir.
Returns:
    - int 1 if some files were not sucessfully renamed.
    - int 0 all files successfully renamed.
*/
int rename_items(Directory *dir) {
    int result;
    int all_files_renamed = 1;
    for (int i = 0; i < dir->length; i++) {
        result = rename(dir->items[i].cur_name, dir->items[i].new_name);
        if (result == 1) {
            all_files_renamed = 0;
        }
    }
    if (all_files_renamed == 1) {
        return 0;
    } else {
        return 1;
    }
}