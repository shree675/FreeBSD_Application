#define _PROGRAM_NAME "whoami"
#include <stdlib.h>
#include <pwd.h>
#include <stdio.h>
#include <grp.h>
#include <sys/types.h>
#include <string.h>


int main()
{
    printf("\nWelcome to this realtime multi-user application!\n\n" );        
    __uid_t uid = getuid();     //gets the user ID of the user that executed this application

    struct passwd* pw = getpwuid(uid);
    if(pw == NULL){
        perror("getpwuid error: ");
    }
    //since a user can belong to a list of groups, check the list for 'student', 'faculty', or 'hod'
    int ngroups = 0;    //this call is just to get the correct ngroups
    getgrouplist(pw->pw_name, pw->pw_gid, NULL, &ngroups);
    __gid_t groups[ngroups];
    //here we actually get the groups
    getgrouplist(pw->pw_name, pw->pw_gid, groups, &ngroups);

    int isStudent = 0;
    int isFaculty = 0;
    int isHod = 0;
    int exit = 0;

    //example to print the groups name
    for (int i = 0; i < ngroups; i++){
        struct group* gr = getgrgid(groups[i]);
        if(gr == NULL){
            perror("getgrgid error: ");
        }
        printf("Printing all detected groups\n");
        printf("%s\n",gr->gr_name);
        /* if(strcmp(gr->gr_name, "wheel"))
        {
            printf("wheel group detected\n");
        } */
        if(strcmp(gr->gr_name, "student")==0)
        {
            printf("student group detected\n");
            isStudent = 1;
            break;
        }
        else if(strcmp(gr->gr_name, "faculty")==0)
        {
            printf("faculty group detected\n");
            isFaculty = 1;
            break;
        }
        else if(strcmp(gr->gr_name, "hod")==0)
        {
            printf("hod group detected\n");
            isHod = 1;
            break;
        }
    }
    int choice = 0;
    while(isHod && !exit)
    {
        printf("Welcome to Application Menu for HOD. Please enter your choice: \n");
        printf("1. Exit\n");
        printf("2. Display your marks\n");
        scanf("%d", &choice);
        switch(choice)
        {
            case 1: exit = 1;
                    break;
            case 2: printf("Displaying marks...\n");
                    break;
            default: 
                    printf("Please enter a valid option\n");
        }
    }

    return 0;
}