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

    //printf("Username: %s\n", pw->pw_name);

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
        //printf("Printing all detected groups\n");
        //printf("%s\n",gr->gr_name);
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
        if(pid>0){ //parent process i.e. the student interface
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
                    char exitStr[]="exit\n";
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
        else if(pid==0){ //child process of student that listens for broadcasted messages
            key_t key;
            int msgid;
            key=ftok("application.c",66); //66 is just a random number #define it
            // printf("key: %d\n",key);
            msgid=msgget(key,0666|IPC_CREAT); //0666 is probably read write no execute permissions
            while(true){
                msgrcv(msgid,&message,sizeof(message),1,0);
                //printf("\nData received: %s\n",message.msg_text);
               
                char message1[MAXLEN], message2[MAXLEN];
                strcpy(message1, "\nData received:");
                strcat(message1,message.msg_text);
                write(STDOUT_FILENO, message1, strlen(message1));
                strcpy(message2, "\n> ");
                write(STDOUT_FILENO, message2, strlen(message2));
            }
            msgctl(msgid,IPC_RMID,NULL);
        }
    }
    int hodChoice = 0;
    while(isHod && !exit)
    {
        printf("Welcome to Application Menu for HOD. Please enter your choice: \n");
        printf("1. Exit\n");
        printf("2. Display your marks\n");
        printf("3. Broadcast a message\n");
        scanf("%d", &hodChoice);
        switch(hodChoice)
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
                msgsnd(msgid,&message,sizeof(message),0);
                msgsnd(msgid,&message,sizeof(message),0);
            }
            default: 
                    printf("Please enter a valid option\n");
        }
    }
    int facultyChoice = 0;
    while(isFaculty && !exit)
    {
        printf("Welcome to Application Menu for Faculty. Please enter your choice: \n");
        printf("1. Exit\n");
        printf("2. Display your marks\n");
        printf("3. Broadcast a message\n");
        printf("4. Edit marks table\n");
        // each faculty is a row, each student is a column
        scanf("%d", &facultyChoice);
        int no_of_students = 2;
        char buffer[120]; 
        strcpy(buffer, pw->pw_name);
        int username_length = strlen(buffer);
        int count = 0;
        //printf("username_length: %d \n ",username_length);
        for(int i=username_length-1; i>=7; i--)
        {
            //printf("username_length: %d i: %d\n ",username_length, i);
            count = count*10 + (int)(buffer[i] -'0');
        }
        int faculty_number = count;
        //printf("Faculty number %d\n\n", faculty_number);
        int edit_student = 0;
        switch(facultyChoice)
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
                msgsnd(msgid,&message,sizeof(message),0);
                msgsnd(msgid,&message,sizeof(message),0);
            }
            case 4: {
                // display in a row, all student numbers with names, and their marks given by this faculty
                // eg. student1 (Mohith)     student2 (Rita)
                //       89                        34
                // Enter the student no whos marks you want to edit:
                // TODO: get names of students and number of students - either from system calls or a meta data file
                for(int i=1; i<=no_of_students; i++)
                {
                    printf("Student no %d\t", i);
                }
                printf("\n");
                for(int i=1; i<=no_of_students; i++)
                {
                    char filename[MAXLEN];
                    sprintf(filename, "./data/data%d%d.txt", faculty_number, i);
                    printf("Filename, %s\n", filename);
                    //strcat(filename, (char)(faculty_number+'0'));
                    //strcat(filename, (char)(i+'0'));
                    //strcat(filename, ".txt");
                    FILE *ptr = fopen(filename, "r");
                    if(ptr == NULL)
                    {
                        printf("File could not be opened");

                    }
                    else
                    {
                        char ch; 
                        do{
                            ch = fgetc(ptr);
                            printf("%c", ch);
                        }while(ch != EOF);
                    }
                    fclose(ptr);
                    printf("\t\t");
                }
                printf("Enter the student number whose marks you want to edit: ");
                scanf("%d", &edit_student);
                printf("Student number chosen is %d", edit_student);
                printf("Enter the updated marks ");
                int updated_marks = 0;
                scanf("%d", &updated_marks);
                FILE *ptr = fopen(filename, "w");
            }
            default: 
                    printf("Please enter a valid option\n");
        }
    }


    return 0;
}