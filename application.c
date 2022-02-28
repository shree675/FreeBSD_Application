#include "utils.h"
#include <grp.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
      user = student;
      break;
    } else if (strcmp(gr->gr_name, "faculty") == 0) {
      user = faculty;
      break;
    } else if (strcmp(gr->gr_name, "hod") == 0) {
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

  // Command line application for hod user
  if (user == hod) {
    if (execl("hod_interface", "", NULL) == -1) {
      perror("In hod execl : ");
      exit(1);
    };
  }

  // Command line application for faculty user
  if (user == faculty) {
    if (execl("faculty_interface", username, NULL) == -1) {
      perror("In faculty execl : ");
      exit(1);
    };
  }
  return 0;
}