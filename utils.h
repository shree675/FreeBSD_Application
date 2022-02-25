#ifndef UTILS_H_
#define UTILS_H_

#define MAX_CHARACTERS_IN_FILE 10
#define MAX_LEN 1000

int get_num_users_in_group(char *);

// prints Bye and terminates the program
void sigint_handler(int);

char *read_file(char *);

int update_file(char *, char *);

char** get_marks(int, int);

#endif