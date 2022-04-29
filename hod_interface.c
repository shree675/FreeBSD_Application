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
  // sigaction(0, &sa, NULL);

  const int num_students = get_num_users_in_group("student");
  const int num_faculty = get_num_users_in_group("faculty");

  printf("Welcome to Application Menu for HOD. Please enter your choice: \n");

  while (1) {
    printf("1. Display Entire Table\n");    // needs to be implemented
    printf("2. Edit Marks\n");              // needs to be implemented
    printf("3. View Overall Statistics\n"); // needs to be implemented
    printf("4. Broadcast a message\n");
    printf("5. Export all the marks as a csv\n");
    printf("[Press Ctrl + C exit]\n");
    printf("> ");

    int hod_choice = 0;
    scanf("%d", &hod_choice);

    switch (hod_choice) {
    case 1: {
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
    case 2: {
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
    case 3: {
      printf("Displaying Overall Statistics\n");

      // Displaying the contents of all files
      int sum = 0;
      int max = MIN_MARKS;
      int min = MAX_MARKS;
      int student_sum[num_students];
      int student_min[num_students];
      int student_max[num_students];
      for (int i = 0; i < num_students; i++) {
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
            max = (mark > max) ? mark : max;
            min = (mark < min) ? mark : min;
            sum += mark;
            student_max[j - 1] =
                (mark > student_max[j - 1]) ? mark : student_max[j - 1];
            student_min[j - 1] =
                (mark < student_min[j - 1]) ? mark : student_min[j - 1];
            student_sum[j - 1] += mark;
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
        printf("\t%.2f  \t", (float)student_sum[i] / num_faculty);
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
    case 4: {
      int fd;
      char named_pipe[MAX_LEN];
      char message[MAX_LEN];
      write(STDOUT_FILENO, "Enter the message: ", 19);
      empty_stdin();
      fgets(message, MAX_LEN, stdin);

      for (int i = 1; i <= num_students; i++) {
        sprintf(named_pipe, "pipes/p_student%d", i);
        fd = open(named_pipe, O_RDWR);
        write(fd, message, strlen(message) + 1);
        close(fd);
      }
      break;
    }
    case 5: {
      char output[MAX_LEN] = "marks.csv";
      printf("Enter the output file name: ");
      scanf("%s", output);
      FILE *output_file = fopen(output, "w");
      if (output_file == NULL) {
        perror("Cannot write to file: ");
        printf("Defaulting to marks.csv file");
        strcpy(output, "marks.csv");
      }

      char marks_file_name[MAX_LEN];
      FILE *marks_file;

      char header[MAX_LEN] = "";

      fprintf(output_file, "faculty_name,");
      for (int i = 1; i <= num_students; i++) {
        char student_name[MAX_LEN];
        sprintf(student_name, "student%d,", i);
        strcat(header, student_name);
      }
      fprintf(output_file, "%s\n", header);

      char row[MAX_LEN] = "";
      int marks;
      for (int i = 1; i <= num_faculty; i++) {
        fprintf(output_file, "faculty%d,", i);
        for (int j = 1; j <= num_students; j++) {
          sprintf(marks_file_name, "./data/data%d%d/data%d%d.txt", i, j, i, j);
          marks_file = fopen(marks_file_name, "r");
          fscanf(marks_file, "%d", &marks);
          fprintf(output_file, "%d,", marks);
          fclose(marks_file);
        }
        fprintf(output_file, "\n");
      }
      fclose(output_file);
      printf("Saved marks in %s successfully\n", output);
      break;
    }
    default:
      printf("Please enter a valid option!\n");
      empty_stdin();
    }
  }

  return 0;
}