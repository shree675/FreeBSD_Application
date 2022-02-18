#define _PROGRAM_NAME "whoami"
#define MAXLEN 1000
#include <stdlib.h>
#include <pwd.h>
#include <stdio.h>
#include <grp.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <unistd.h>

struct message_type{
    long msg_type;
    char msg_text[MAXLEN];
} message;

int main(){
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
    int studentChoice=0;
    while(isStudent && !exit){
        int pid=fork();
        if(pid>0){
            printf("Welcome to Application Menu for student. Please enter your choice: \n");
            printf("1. Exit\n");
            printf("2. Display your marks\n");
            printf("3. Echo\n");
            scanf("%d", &studentChoice);
            switch(studentChoice){
                case 1: exit = 1;
                        break;
                case 2: printf("Displaying marks...\n");
                        break;
                case 3: {
                    char* temp;
                    char exitStr[]="exit";
                    fgets(temp,MAXLEN,stdin);
                    while(true){
                        printf("echo> ");
                        fgets(temp,MAXLEN,stdin);
                        if(strcmp(temp,exitStr)==0){
                            break;
                        }
                        printf("%s\n",temp);
                    }
                }
                default: 
                        printf("Please enter a valid option\n");
            }
        }
        else if(pid==0){
            key_t key;
            int msgid;
            key=ftok("application.c",66);
            // printf("key: %d\n",key);
            msgid=msgget(key,0666|IPC_CREAT);
            while(true){
                msgrcv(msgid,&message,sizeof(message),1,0);
                printf("\nData received: %s\n",message.msg_text);
            }
            msgctl(msgid,IPC_RMID,NULL);
        }
    }
    int choice = 0;
    while(isHod && !exit)
    {
        printf("Welcome to Application Menu for HOD. Please enter your choice: \n");
        printf("1. Exit\n");
        printf("2. Display your marks\n");
        printf("3. Broadcast a message\n");
        scanf("%d", &choice);
        switch(choice)
        {
            case 1: exit = 1;
                    break;
            case 2: printf("Displaying marks...\n");
                    break;
            case 3: {
                key_t key;
                int msgid;
                key=ftok("application.c",66);
                printf("key: %d\n",key);
                msgid=msgget(key,0666|IPC_CREAT);
                char temp[MAXLEN];
                fgets(temp,MAXLEN,stdin);
                message.msg_type=1;
                printf("Enter your message: ");
                fgets(message.msg_text,MAXLEN,stdin);
                msgsnd(msgid,&message,sizeof(message),0);
            }
            default: 
                    printf("Please enter a valid option\n");
        }
    }

    return 0;
}