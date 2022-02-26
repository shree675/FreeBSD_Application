#include "utils.h"
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_GROUPS 10
#define MAX_MARKS 100
#define MIN_MARKS 0

void *log_history(FILE *fp); //Function to format and print Version history

enum userType {
  faculty = 0,
  hod = 1,
  student = 2,
  unknown = -1,
};

// Need to do error handling

int main() {
  printf("\nWelcome to this realtime multi-user application!\n\n");
  uid_t uid =
      getuid(); // gets the user ID of the user that executed this application

  struct passwd *pw = getpwuid(uid);
  if (pw == NULL) {
    perror("getpwuid error: ");
  }

  // since a user can belong to a list of groups, check the list for 'student',
  // 'faculty', or 'hod'
  int nGroups = MAX_GROUPS;
  gid_t *groups = malloc(nGroups * sizeof(gid_t));

  if (getgrouplist(pw->pw_name, pw->pw_gid, groups, &nGroups) == -1) {
    //  if the number of groups is greater than MAX_GROUPS,
    //  we reallocate groups based on the correct nGroups value
    groups = malloc(nGroups * sizeof(gid_t));
    getgrouplist(pw->pw_name, pw->pw_gid, groups, &nGroups);
  }

  enum userType user = unknown;
  int shouldExit = 0;

  for (int i = 0; i < nGroups; i++) {
    struct group *gr = getgrgid(groups[i]);
    if (gr == NULL) {
      perror("getgrgid error: ");
      continue;
    }

    if (strcmp(gr->gr_name, "student") == 0) {
      // printf("student group detected\n");
      user = student;
      break;
    } else if (strcmp(gr->gr_name, "faculty") == 0) {
      // printf("faculty group detected\n");
      user = faculty;
      break;
    } else if (strcmp(gr->gr_name, "hod") == 0) {
      // printf("hod group detected\n");
      user = hod;
      break;
    }
  }

  if (user == unknown) {
    printf("User %s does not belong to groups \"student\", \"faculty\" or "
           "\"hod\"\n",
           pw->pw_name);
    exit(1);
  }

  const int num_students = get_num_users_in_group("student");
  const int num_faculty = get_num_users_in_group("faculty");

  char username[MAX_LEN];

  strcpy(username, pw->pw_name);
  int username_length = strlen(username);

  // Command line application for student user
  if (user == student) {
    if (execl("student_interface", username, NULL) == -1) {
      perror("In student execl : ");
      exit(1);
    };
  }
  int hodChoice = 0;
  while (user == hod && !shouldExit) {
    printf("Welcome to Application Menu for HOD. Please enter your choice: \n");
    printf("1. Exit\n");
    printf("2. Display Entire Table\n");    // needs to be implemented
    printf("3. Edit Marks\n");              // needs to be implemented
    printf("4. View Overall Statistics\n"); // needs to be implemented
    printf("5. Broadcast a message\n");
    printf("> ");
    scanf("%d", &hodChoice);
    switch (hodChoice) {
    case 1:
      shouldExit = 1;
      break;
    case 2: {
      printf("Displaying marks...\n");
      printf("\t\t");
      for (int i = 1; i <= num_students; i++) {
        printf("Student %d | ", i);
      }
      printf("\n");
      // Displaying the contents of all files
      for (int i = 1; i <= num_faculty; i++) {
        printf("Faculty %d:  \t", i);
        for (int j = 1; j <= num_students; j++) {
          char filename[MAX_LEN];
          sprintf(filename, "./data/data%d%d/data%d%d.txt", i, j, i, j);
          // printf("Filename :  %s\n", filename);
          FILE *ptr = fopen(filename, "r");
          if (ptr == NULL) {
            printf("File could not be opened");

          } else {
            char ch = fgetc(ptr);
            while (ch != EOF && ch != '\n') {
              printf("%c", ch);
              ch = fgetc(ptr);
            }
          }
          fclose(ptr);
          printf("\t\t");
        }
        printf("\n");
      }
      printf("-----------------------------------------------------------------"
             "----------------------------------\n");
      break;
    }

    case 3: {
      int edit_student = 0;
      int edit_row = 0;

      printf("\t\t");
      for (int i = 1; i <= num_students; i++) {
        printf("Student %d | ", i);
      }
      printf("\n");
      // Displaying the contents of all files
      for (int i = 1; i <= num_faculty; i++) {
        printf("Faculty %d:  \t", i);
        for (int j = 1; j <= num_students; j++) {
          char filename[MAX_LEN];
          sprintf(filename, "./data/data%d%d/data%d%d.txt", i, j, i, j);
          // printf("Filename :  %s\n", filename);
          FILE *ptr = fopen(filename, "r");
          if (ptr == NULL) {
            printf("File could not be opened");

          } else {
            char ch = fgetc(ptr);
            while (ch != EOF && ch != '\n') {
              printf("%c", ch);
              ch = fgetc(ptr);
            }
          }
          fclose(ptr);
          printf("\t\t");
        }
        printf("\n");
      }
      printf("-----------------------------------------------------------------"
             "----------------------------------\n");

      printf("\nEnter the student number whose marks you want to edit: ");
      scanf("%d", &edit_student);
      printf("\nEnter the row number where you want to edit marks: ");
      scanf("%d", &edit_row);
      printf("\nEnter the updated marks : ");
      int updated_marks = 0;
      scanf("%d", &updated_marks);

      // getting the filename
      char filename[MAX_LEN];
      sprintf(filename, "./data/data%d%d/data%d%d.txt", edit_row, edit_student,
              edit_row, edit_student);
      // printf("Filename, %s\n", filename);

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
              edit_row, edit_student, edit_row, edit_student);
      popen(command_add, "r");
      printf("Marks updated\n");
      break;
    }
    case 4: {
      printf("Displaying Overall Statistics\n");

      // Displaying the contents of all files
      int sum = 0;
      int max = MIN_MARKS;
      int min = MAX_MARKS;
      int student_sum[num_students];
      int student_min[num_students];
      int student_max[num_students];
      for(int i=0; i<num_students; i++) {
        student_sum[i] = 0;
        student_max[i] = MIN_MARKS;
        student_min[i] = MAX_MARKS;
      }
      printf("\t\t");
      for (int i = 1; i <= num_students; i++) {
        printf("Student %d | ", i);
      }
      printf("\n");
      for (int i = 1; i <= num_faculty; i++) {
        printf("Faculty %d:  \t", i);
        for (int j = 1; j <= num_students; j++) {
          char filename[MAX_LEN];
          sprintf(filename, "./data/data%d%d/data%d%d.txt", i, j, i, j);
          // printf("Filename :  %s\n", filename);
          FILE *ptr = fopen(filename, "r");
          if (ptr == NULL) {
            printf("File could not be opened");

          } else {
            char ch = fgetc(ptr);
            int mark = 0;
            while (ch != EOF && ch != '\n') {
              printf("%c", ch);
              mark = mark * 10 + (int)(ch - '0');
              ch = fgetc(ptr);
            }
            // marks[i] = mark;
            max = (mark > max) ? mark : max;
            min = (mark < min) ? mark : min;
            sum += mark;
            student_max[j-1] = (mark > student_max[j-1]) ? mark : student_max[j-1];
            student_min[j-1] = (mark < student_min[j-1]) ? mark : student_min[j-1];
            student_sum[j-1] += mark;
          }
          fclose(ptr);
          printf("\t\t");
        }
        printf("\n");
      }
      printf("\n---------------------------------------------------------------"
             "----------------------------------\n");
      printf("Student-wise:\t\t");
      for (int i = 1; i <= num_students; i++) {
        printf("Student %d  |  ", i);
      }
      printf("\nAverage\t\t");
      for (int i = 0; i < num_students; i++) {
        printf("\t%.2f  \t", (float)student_sum[i]/num_faculty);
      }
      printf("\nMinimum\t\t");
      for (int i = 0; i < num_students; i++) {
        printf("\t%d\t", student_min[i]);
      }
      printf("\nMaximum\t\t");
      for (int i = 0; i < num_students; i++) {
        printf("\t%d\t", student_max[i]);
      }

      printf("\n---------------------------------------------------------------"
             "----------------------------------\n");
      printf("Total of all scores: %d\n", sum);
      printf("Average of all scores: %.2f\n",
             (float)sum / (num_students * num_faculty));
      printf("Maximum score is %d and Minimum score is %d\n", max, min);
      printf("\n---------------------------------------------------------------"
             "----------------------------------\n");
      break;
    }
      // case 5: {
      //   key_t key;
      //   int msgid;
      //   key = ftok("application.c", 66);
      //   printf("key: %ld\n", key);
      //   msgid = msgget(key, 0666 | IPC_CREAT);
      //   char temp[MAX_LEN];
      //   fgets(temp, MAX_LEN, stdin);
      //   message.msg_type = 1;
      //   printf("Enter your message: ");
      //   fgets(message.msg_text, MAX_LEN, stdin);
      //   msgsnd(msgid, &message, sizeof(message), 0);
      //   msgsnd(msgid, &message, sizeof(message), 0);
      //   msgsnd(msgid, &message, sizeof(message), 0);
      // }
      // default:
      //   printf("Please enter a valid option\n");
      // }
    }
  }
  int facultyChoice = 0;
  while (user == faculty && !shouldExit) {
    printf("Welcome to Application Menu for Faculty. Please enter your choice: "
           "\n");
    printf("1. Exit\n");
    printf("2. Display marks\n");       // needs to be implemented
    printf("3. Broadcast a message\n"); // this is not there
    printf("4. Edit marks table\n");
    printf("5. Undo edit operation\n");
    printf("6. View version history\n");
    printf("> ");
    // each faculty is a row, each student is a column
    scanf("%d", &facultyChoice);
    // Finding the faculty number from the username
    int count = 0; // printf("username_length: %d \n ",username_length);
    for (int i = username_length - 1; i >= 7; i--) {
      count = count * 10 + (int)(username[i] - '0');
    }
    int faculty_number = count;
    // printf("Faculty number %d\n\n", faculty_number);
    switch (facultyChoice) {
    case 1:
      shouldExit = 1;
      break;
    case 2:
      printf("Displaying marks...\n");
      break;
    // case 3: {
    //   key_t key;
    //   int msgid;
    //   key = ftok("application.c", 66);
    //   printf("key: %ld\n", key);
    //   msgid = msgget(key, 0666 | IPC_CREAT);
    //   char temp[MAX_LEN];
    //   fgets(temp, MAX_LEN, stdin);
    //   message.msg_type = 1;
    //   printf("Enter your message: ");
    //   fgets(message.msg_text, MAX_LEN, stdin);
    //   msgsnd(msgid, &message, sizeof(message), 0);
    //   msgsnd(msgid, &message, sizeof(message), 0);
    //   msgsnd(msgid, &message, sizeof(message), 0);
    // }
    case 4: {
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
        // printf("Filename :  %s\n", filename);

        FILE *ptr = fopen(filename, "r");
        if (ptr == NULL) {
          printf("File could not be opened");

        } else {
          char ch = fgetc(ptr);
          while (ch != EOF && ch != '\n') {
            printf("%c", ch);
            ch = fgetc(ptr);
          }
        }
        fclose(ptr);
        printf("\t\t");
      }

      printf("\nEnter the student number whose marks you want to edit: ");
      scanf("%d", &edit_student);
      // printf("Student number chosen is %d\n", edit_student);

      printf("\nEnter the updated marks : ");
      int updated_marks = 0;
      scanf("%d", &updated_marks);

      // getting the filename
      char filename[MAX_LEN];
      sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number,
              edit_student, faculty_number, edit_student);
      // printf("Filename, %s\n", filename);

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
    case 5: {
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
        // printf("Filename, %s\n", filename);

        FILE *ptr = fopen(filename, "r");
        if (ptr == NULL) {
          printf("File could not be opened");

        } else {
          char ch = fgetc(ptr);
          while (ch != EOF && ch != '\n') {
            printf("%c", ch);
            ch = fgetc(ptr);
          }
        }
        fclose(ptr);
        printf("\t\t");
      }

      printf("\nEnter the student number whose marks you want to undo: ");
      scanf("%d", &edit_student);
      // printf("Student number chosen is %d\n", edit_student);

      // getting the filename
      char filename[MAX_LEN];
      sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number,
              edit_student, faculty_number, edit_student);
      // printf("Filename, %s\n", filename);

      char command_add[MAX_LEN];

      sprintf(command_add,
              "cd data/data%d%d && git revert HEAD --no-edit && cd .. && cd ..",
              faculty_number, edit_student);

      popen(command_add, "r");
      break;
    }
    case 6: {
      int edit_student = 0;
      for (int i = 1; i <= num_students; i++) {
        printf("Student no %d\t", i);
      }
      printf("\n");
      // Displaying the contents of the faculty files.
      for (int i = 1; i <= num_students; i++) {
        char filename[MAX_LEN];
        sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number, i,
                faculty_number, i);
        // printf("Filename, %s\n", filename);

        FILE *ptr = fopen(filename, "r");
        if (ptr == NULL) {
          printf("File could not be opened");
        } else {
          char ch = fgetc(ptr);
          while (ch != EOF && ch != '\n') {
            printf("%c", ch);
            ch = fgetc(ptr);
          }
        }
        fclose(ptr);
        printf("\t\t");
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
      //char log_message[MAX_LEN];
      //fscanf(p, "%s", log_message);
      //printf("The Version history of file is as follows: \n %s\n", log_message);
      pclose(p);
      popen("cd .. && cd ..", "r");

      break;
    }
    default:
      printf("Please enter a valid option\n");
    }
  }
  return 0;
}