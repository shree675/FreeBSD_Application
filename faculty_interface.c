#include "utils.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  struct sigaction sa = {0};
  sa.sa_handler = sigint_handler;
  sigaction(SIGINT, &sa, NULL);

  const char *username = argv[0];
  const int username_length = strlen(username);

  const int num_students = get_num_users_in_group("student");
  const int num_faculty = get_num_users_in_group("faculty");

  int faculty_number = 0;
  for (int i = username_length - 1; i >= 7; i--) {
    faculty_number = faculty_number * 10 + (int)(username[i] - '0');
  }

  printf("Welcome to Application Menu for Faculty. Please enter your choice: "
         "\n");

  while (1) {

    printf("1. Display marks\n"); // TODO: needs to be implemented
    printf("2. Edit marks table\n");
    printf("3. Undo edit operation\n");
    printf("4. View version history\n");
    printf("[Press Ctrl + C exit]\n");
    printf("> ");

    int faculty_choice = 0;
    scanf("%d", &faculty_choice);

    switch (faculty_choice) {
    case 1:
      printf("Displaying marks...\n");
      for (int i = 1; i <= num_students; i++) {
        printf("student%d  ", i);
      }
      printf("\n");
      for (int i = 1; i <= num_students; i++) {
        char filename[MAX_LEN];
        sprintf(filename, "data/data%d%d/data%d%d.txt", faculty_number, i,
                faculty_number, i);
        FILE *fp = fopen(filename, "r");
        int marks;
        fscanf(fp, "%d", &marks);
        printf("%d         ", marks);

      }
      printf("\n");
      break;

    case 2: {
      int edit_student = 0;

      // display in a row, all student numbers with names, and their marks
      // given
      // by this faculty eg. student1 (Mohith)     student2 (Rita)
      //       89                        34
      // Enter the student no whos marks you want to edit:
      // TODO: get names of students and number of students - either from
      // system
      // calls or a meta data file

      for (int i = 1; i <= num_students; i++) {
        printf("Student no %d\t", i);
      }
      printf("\n");

      // Displaying the contents of the faculty files.
      for (int i = 1; i <= num_students; i++) {
        char filename[MAX_LEN];
        sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number, i,
                faculty_number, i);

        FILE *ptr = fopen(filename, "r");
        if (ptr == NULL) {
          printf("File could not be opened");
          continue;
        } else {
          char contents[MAX_LEN];
          fscanf(ptr, "%s", contents);
          printf("%s", contents);
        }
        fclose(ptr);
        printf("\t\t");
      }

      printf("\nEnter the student number whose marks you want to edit: ");
      scanf("%d", &edit_student);

      printf("\nEnter the updated marks : ");
      int updated_marks = 0;
      scanf("%d", &updated_marks);

      // getting the filename
      char filename[MAX_LEN];
      sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number,
              edit_student, faculty_number, edit_student);

      // writing the updated marks to the file
      FILE *ptr = fopen(filename, "w");
      if (ptr == NULL) {
        printf("file could not be opened\n");
      }
      fprintf(ptr, "%d", updated_marks);
      fclose(ptr);

      char command_add[MAX_LEN];

      sprintf(command_add,
              "cd data/data%d%d/ && git add data%d%d.txt && git commit -m "
              "\"updated data file\" && cd .. && cd ..",
              faculty_number, edit_student, faculty_number, edit_student);
      popen(command_add, "r");
      break;
    }
    case 3: {
      int edit_student = 0;
      // display in a row, all student numbers with names, and their marks given
      // by this faculty eg. student1 (Mohith)     student2 (Rita)
      //       89                        34
      // Enter the student no whos marks you want to edit:
      // TODO: get names of students and number of students - either from system
      // calls or a meta data file
      for (int i = 1; i <= num_students; i++) {
        printf("Student no %d\t", i);
      }
      printf("\n");

      // Displaying the contents of the faculty files.
      for (int i = 1; i <= num_students; i++) {
        char filename[MAX_LEN];
        sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number, i,
                faculty_number, i);

        FILE *ptr = fopen(filename, "r");
        if (ptr == NULL) {
          printf("File could not be opened");

        } else {
          char contents[MAX_LEN];
          fscanf(ptr, "%s", contents);
          printf("%s\n", contents);
        }
        fclose(ptr);
        printf("\t\t");
      }

      printf("\nEnter the student number whose marks you want to undo: ");
      scanf("%d", &edit_student);

      // getting the filename
      char filename[MAX_LEN];
      sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number,
              edit_student, faculty_number, edit_student);

      char command_add[MAX_LEN];

      sprintf(command_add,
              "cd data/data%d%d && git revert HEAD --no-edit && cd .. && cd ..",
              faculty_number, edit_student);

      popen(command_add, "r");
      break;
    }
    case 4: {
      int edit_student = 0;
      for (int i = 1; i <= num_students; i++) {
        printf("Student%d  ", i);
      }
      printf("\n");

      // Displaying the contents of the faculty files.
      for (int i = 1; i <= num_students; i++) {
        char filename[MAX_LEN];
        sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number, i,
                faculty_number, i);

        FILE *ptr = fopen(filename, "r");
        if (ptr == NULL) {
          printf("File could not be opened");
        } else {
          char contents[MAX_LEN];
          fscanf(ptr, "%s", contents);
          printf("%s         ", contents);
        }
        fclose(ptr);
        
      }

      printf("\nEnter the student number whose marks version history you want "
             "to view: ");
      scanf("%d", &edit_student);
      // printf("Student number chosen is %d\n", edit_student);
      // getting the filename
      char filename[MAX_LEN];
      sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number,
              edit_student, faculty_number, edit_student);
      // printf("Filename, %s\n", filename);

      popen("cd .. && cd ..", "r");
      char command_view[MAX_LEN];
      sprintf(command_view,
              "cd data/data%d%d && git log -p --oneline -- data%d%d.txt",
              faculty_number, edit_student, faculty_number, edit_student);

      FILE *p = popen(command_view, "r");
      log_history(p);

      /* char ch = fgetc(p);
      while (ch != EOF) {
        printf("%c", ch);
        ch = fgetc(p);
      } */
      // char log_message[MAX_LEN];
      // fscanf(p, "%s", log_message);
      // printf("The Version history of file is as follows: \n %s\n",
      // log_message);
      pclose(p);
      popen("cd .. && cd ..", "r");

      break;
    }
    default:
      printf("Please enter a valid option!\n");
      empty_stdin();
    }
  }
  return 0;
}
