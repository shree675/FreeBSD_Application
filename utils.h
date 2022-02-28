#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#define MAX_CHARACTERS_IN_FILE 10
#define MAX_LEN 1000
#define MAX_MARKS 100
#define MIN_MARKS 0
#define MAX_GROUPS 10

enum userType {
  faculty = 0,
  hod = 1,
  student = 2,
  unknown = -1,
};


int get_num_users_in_group(char *);

// prints Bye and terminates the program
void sigint_handler(int);

char *read_file(char *);

int update_file(char *, char *);

char** get_marks(int, int);

void empty_stdin();

void *log_history(FILE *);

#endif