#include "utils.h"
#include <fcntl.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// returns the number of members in the given group name
int get_num_users_in_group(char *group_name) {
  int num_members = 0;
  struct group *gr = getgrnam(group_name);
  if (gr == NULL) {
    printf("%s\n", group_name);
    perror("Cannot get group name: ");
    exit(1);
  }
  while (gr->gr_mem[++num_members])
    ;
  return num_members;
}

void sigint_handler(int signum) {
  write(STDOUT_FILENO, "\nBye!\n", 6);
  exit(1);
}

char *read_file(char *file_name) {
  FILE *fp = fopen(file_name, O_RDONLY);
  if (fp == NULL) {
    perror("Could not read file: ");
    fclose(fp);
    return NULL;
  }

  char *contents = malloc(MAX_CHARACTERS_IN_FILE * sizeof(char));
  fscanf(fp, "%s", contents);
  fclose(fp);
  return contents;
}

void *log_history(FILE *fp) {
  printf("||Printing the version history||\n\nLatest Version\n");
  char *contents = malloc(MAX_CHARACTERS_IN_FILE * sizeof(char));
  while(fscanf(fp, "%[^\n] ", contents)!=EOF) {
    if(contents[0] && (contents[0] == '+' || contents[0] == '-')) {
      if(contents[1] && (contents[1] == '+' || contents[1] == '-'))
        printf("---------------------------------------------\n");
      else
        printf("> %s\n", contents);
    }
  }
  printf("Oldest Version\n");
  printf("---------------------------------------------\n");
  fclose(fp);
  return NULL;
}

char **get_marks(int student_number, int num_faculty) {
  // TODO: Fix this

  // char **marks = malloc(num_faculty * sizeof(char[MAX_LEN]));
  // for (int i = 1; i <= num_faculty; i++) {
  //   char filename[MAX_LEN];
  //   sprintf(filename, "data/data%d%d/data%d%d.txt", i, student_number, i,
  //           student_number);
  //   if (access(filename, R_OK) == -1) {
  //     perror("In access()");
  //     exit(1);
  //   }

  //   FILE *ptr = fopen(filename, "r");
  //   if (ptr == NULL) {
  //     perror("In fopen()");
  //   } else {
  //     fscanf(ptr, "%s", marks[i]);
  //   }
  //   fclose(ptr);
  // }
  // return marks;
  return NULL;
}