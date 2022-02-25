#include "utils.h"
#include <err.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/event.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_MARKS 100
#define MIN_MARKS 0

void exit_program(int signum) {
  exit(1);
}

int main(int argc, char *argv[]) {
  // char *username = argv[1];
  char *username = "student1";
  int username_length = strlen(username);

  int student_number = 0;
  for (int i = username_length - 1; i >= 7; i--) {
    student_number = student_number * 10 + (int)(username[i] - '0');
  }

  const int num_faculty = get_num_users_in_group("faculty");

  struct sigaction sa = {0};
  sa.sa_handler = sigint_handler;
  sigaction(SIGINT, &sa, NULL);

  if (fork()) {
    //   parent: display the interface here
    sa.sa_handler = exit_program;
    sigaction(SIGINT, &sa, NULL);
    while (1) {

      printf("\n\nWelcome to Application Menu for student. Please enter your "
             "choice: \n");
      printf("1. Display your marks\n");
      printf("2. Display Marks Statistics\n");
      printf("[Press Ctrl + C to exit]\n");
      printf("> ");

      int student_choice = 0;
      scanf("%d", &student_choice);

      switch (student_choice) {
      case 1: {
        printf("Displaying marks...\n");
        printf("\nMarks awarded by evaluators. (evaluator names hidden)\n");

        // Consider randomizing the order of display in every run, even though
        // faculty name is already hidden

        // Displaying the contents of the student files.
        for (int i = 1; i <= num_faculty; i++) {
          char filename[MAX_LEN];
          sprintf(filename, "data/data%d%d/data%d%d.txt", i, student_number, i,
                  student_number);
          if (access(filename, R_OK) == -1) {
            perror("In access()");
            exit(1);
          }

          FILE *ptr = fopen(filename, "r");
          if (ptr == NULL) {
            perror("In fopen()");
          } else {
            char contents[MAX_LEN];
            fscanf(ptr, "%s", contents);
            printf("%s\t", contents);
          }
          fclose(ptr);
        }
        break;
      }
      case 2: {
        printf("Displaying Statistics of Marks obtained\n");
        // int marks[MAXLEN];
        int sum = 0;
        int max = MIN_MARKS;
        int min = MAX_MARKS;
        for (int i = 1; i <= num_faculty; i++) {
          char filename[MAX_LEN];
          sprintf(filename, "./data/data%d%d/data%d%d.txt", i, student_number,
                  i, student_number);
          FILE *ptr = fopen(filename, "r");
          if (ptr == NULL) {
            printf("File could not be opened");
          } else {
            int mark = 0;
            fscanf(ptr, "%d", &mark);

            max = (mark > max) ? mark : max;
            min = (mark < min) ? mark : min;
            sum += mark;
          }
          fclose(ptr);
          printf("\t\t");
        }
        printf("\n-------------------------------------------------------------"
               "------------------------------------\n");
        printf("Total of all scores: %d\n", sum);
        printf("Average of all scores: %f\n", (float)sum / num_faculty);
        printf("Maximum score is %d and Minimum score is %d\n", max, min);
        printf("\n-------------------------------------------------------------"
               "------------------------------------\n");
        break;
      }
      default:
        printf("Please enter a valid option!");
      }
    }

    wait(0);

  } else {
    //   child: handle the triggers that occur
    //   when the pipe is written to

    char contents[MAX_LEN];
    char named_pipe[MAX_LEN];

    sprintf(named_pipe, "p_%s", username);

    struct kevent event;  /* Event we want to monitor */
    struct kevent tevent; /* Event triggered */
    int kq, fd, ret;

    fd = open(named_pipe, O_RDWR);

    /* Create kqueue. */
    kq = kqueue();
    if (kq == -1)
      err(EXIT_FAILURE, "kqueue() failed");

    /* Initialize kevent structure. */
    EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_CLEAR, NOTE_WRITE, 0, NULL);
    /* Attach event to the	kqueue.	*/
    ret = kevent(kq, &event, 1, NULL, 0, NULL);
    if (ret == -1)
      err(EXIT_FAILURE, "kevent register");
    if (event.flags & EV_ERROR)
      errx(EXIT_FAILURE, "Event error: %s", strerror(event.data));

    while (1) {
      /*	Sleep until something happens. */
      ret = kevent(kq, NULL, 0, &tevent, 1, NULL);
      if (ret == -1) {
        err(EXIT_FAILURE, "kevent wait");
      } else if (ret > 0) {
        read(fd, contents, MAX_LEN);
        printf("%s\n", contents);
      }
    }
    close(fd);
  }
  return 0;
}