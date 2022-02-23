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

// Need to do error handling
struct message_type {
    long msg_type;
    char msg_text[MAXLEN];
} message;

int main() {
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
    getgrouplist(pw->pw_name, pw->pw_gid, groups, &ngroups);    //printf("Username: %s\n", pw->pw_name);

    int isStudent = 0;
    int isFaculty = 0;
    int isHod = 0;
    int exit = 0;
    
    for (int i = 0; i < ngroups; i++){
        struct group* gr = getgrgid(groups[i]);
        if(gr == NULL) {
            perror("getgrgid error: ");
        }
        //printf("Printing all detected groups\n");
        //printf("%s\n",gr->gr_name);
        /* if(strcmp(gr->gr_name, "wheel"))
        {
            printf("wheel group detected\n");
        } */
        if(strcmp(gr->gr_name, "student")==0) {
            //printf("student group detected\n");
            isStudent = 1; break;
        }
        else if(strcmp(gr->gr_name, "faculty")==0) {
            //printf("faculty group detected\n");
            isFaculty = 1; break;
        }
        else if(strcmp(gr->gr_name, "hod")==0) {
            //printf("hod group detected\n");
            isHod = 1; break;
        }
    }

    //Some common variables used in all cases
    // TODO: no of students and faculty currently hardcoded, need to be changed (no of members of student/faculty group maybe)
    int no_of_students = 2;
    int no_of_faculty = 2;
    char username[MAXLEN]; 
    strcpy(username, pw->pw_name);
    int username_length = strlen(username);
    // Command line application for student user
    while(isStudent && !exit){
        int pid=fork();
        if(pid>0){ //parent process i.e. the student interface
            printf("\n\nWelcome to Application Menu for student. Please enter your choice: \n");
            printf("1. Exit\n");
            printf("2. Display your marks\n");
            printf("3. Display Marks Statistics\n");
            printf("4. Echo\n"); //we can get rid of the echo functionality
            printf("> ");
            int studentChoice=0;
            scanf("%d", &studentChoice);
            
            switch(studentChoice){
                case 1: exit = 1;
                        break;
                case 2: {
                        printf("Displaying marks...\n");
                        //Finding the student number from the username
                        int count = 0;      //printf("username_length: %d \n ",username_length);
                        for(int i=username_length-1; i>=7; i--) {
                            count = count*10 + (int)(username[i] -'0');
                        }
                        int student_number = count;
                        //printf("Student number %d\n\n", student_number);
                        printf("\nMarks awarded by evaluators. (evaluator names hidden)\n");
                        /* for(int i=1; i<=no_of_faculty; i++)
                        {
                            printf("Evaluator no %d\t", i);
                        }
                        printf("\n"); */

                        //Consider randomizing the order of display in every run, even though faculty name is already hidden 
                        // Displaying the contents of the student files.
                        for(int i=1; i<=no_of_faculty; i++)
                        {
                            char filename[MAXLEN];
                            sprintf(filename, "./data/data%d%d/data%d%d.txt", i, student_number, i, student_number);
                            printf("Filename :  %s\n", filename);

                            FILE *ptr = fopen(filename, "r");
                            if(ptr == NULL)
                            {
                                printf("File could not be opened");

                            }
                            else
                            {
                                char ch = fgetc(ptr);
                                while(ch != EOF && ch != '\n'){
                                    printf("%c", ch);	
                                    ch = fgetc(ptr);
                                }
                            }
                            fclose(ptr);
                            printf("\t\t");
                        }
                        break;
                }
                case 3: {
                        printf("Displaying Statistics of Marks obtained\n");
                        //Finding the student number from the username
                        int count = 0;      //printf("username_length: %d \n ",username_length);
                        for(int i=username_length-1; i>=7; i--) {
                            count = count*10 + (int)(username[i] -'0');
                        }
                        int student_number = count;
                        //int marks[MAXLEN];
                        int sum = 0;
                        int max = -1;
                        int min = 10000;
                        for(int i=1; i<=no_of_faculty; i++)
                        {
                            char filename[MAXLEN];
                            sprintf(filename, "./data/data%d%d/data%d%d.txt", i, student_number, i, student_number);
                            //printf("Filename :  %s\n", filename);

                            FILE *ptr = fopen(filename, "r");
                            if(ptr == NULL)
                            {
                                printf("File could not be opened");
                            }
                            else
                            {
                                char ch = fgetc(ptr);
                                int mark = 0;
                                while(ch != EOF && ch != '\n'){
                                    printf("%c", ch);
                                    mark = mark*10 + (int)(ch-'0');	
                                    ch = fgetc(ptr);
                                }
                                //marks[i] = mark;
                                max = (mark>max) ? mark:max;
                                min = (mark<min) ? mark:min;
                                sum+=mark;
                            }
                            fclose(ptr);
                            printf("\t\t");
                        }
                        printf("\n-------------------------------------------------------------------------------------------------\n");
                        printf("Total of all scores: %d\n", sum);
                        printf("Average of all scores: %f\n", (float)sum/no_of_faculty);
                        printf("Maximum score is %d and Minimum score is %d\n", max, min);
                        printf("\n-------------------------------------------------------------------------------------------------\n");
                        break;
                }
                case 4: {
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
        printf("2. Display Entire Table\n"); //needs to be implemented
        printf("3. Edit Marks\n"); //needs to be implemented
        printf("4. View Overall Statistics\n"); //needs to be implemented
        printf("5. Broadcast a message\n");
        printf("> ");
        scanf("%d", &hodChoice);
        switch(hodChoice)
        {
            case 1: exit = 1;
                    break;
            case 2: {
                    printf("Displaying marks...\n");
                    printf("\t\t");
                    for(int i=1; i<=no_of_students; i++)
                    {
                        printf("Student %d | ", i);
                    }
                    printf("\n");
                    // Displaying the contents of all files
                    for(int i=1; i<=no_of_faculty; i++)
                    {
                        printf("Faculty %d:  \t", i);
                        for(int j=1; j<=no_of_students; j++)
                        {
                            char filename[MAXLEN];
                            sprintf(filename, "./data/data%d%d/data%d%d.txt", i, j, i, j);
                            //printf("Filename :  %s\n", filename);
                            FILE *ptr = fopen(filename, "r");
                            if(ptr == NULL)
                            {
                                printf("File could not be opened");

                            }
                            else
                            {
                                char ch = fgetc(ptr);
                                while(ch != EOF && ch != '\n'){
                                    printf("%c", ch);
                                    ch = fgetc(ptr);
                                }
                            }
                            fclose(ptr);
                            printf("\t\t");
                            }
                        printf("\n");
                    } 
                    printf("---------------------------------------------------------------------------------------------------\n");  
                    break;   
                }
            
            case 3:
                {
        	    int edit_student = 0;
                int edit_row = 0;

                printf("\t\t");
                    for(int i=1; i<=no_of_students; i++)
                    {
                        printf("Student %d | ", i);
                    }
                    printf("\n");
                    // Displaying the contents of all files
                    for(int i=1; i<=no_of_faculty; i++)
                    {
                        printf("Faculty %d:  \t", i);
                        for(int j=1; j<=no_of_students; j++)
                        {
                            char filename[MAXLEN];
                            sprintf(filename, "./data/data%d%d/data%d%d.txt", i, j, i, j);
                            //printf("Filename :  %s\n", filename);
                            FILE *ptr = fopen(filename, "r");
                            if(ptr == NULL)
                            {
                                printf("File could not be opened");

                            }
                            else
                            {
                                char ch = fgetc(ptr);
                                while(ch != EOF && ch != '\n'){
                                    printf("%c", ch);	
                                    ch = fgetc(ptr);
                                }
                                
                            }
                            fclose(ptr);
                            printf("\t\t");
                            }
                        printf("\n");
                    } 
                    printf("---------------------------------------------------------------------------------------------------\n");  
            
                printf("\nEnter the student number whose marks you want to edit: ");
                scanf("%d", &edit_student);
                printf("\nEnter the row number where you want to edit marks: ");
                scanf("%d", &edit_row);
                printf("\nEnter the updated marks : ");
                int updated_marks = 0;
                scanf("%d", &updated_marks);

		// getting the filename
                char filename[MAXLEN];
                sprintf(filename, "./data/data%d%d/data%d%d.txt", edit_row, edit_student, edit_row, edit_student);
                // printf("Filename, %s\n", filename);


		// writing the updated marks to the file
                FILE *ptr = fopen(filename, "w");
                if(ptr == NULL){
                    printf("file could not be opened\n");
                }
                fprintf(ptr, "%d", updated_marks);
                fclose(ptr);

                char command_add[MAXLEN];
                sprintf(
                    command_add, 
                    "cd data/data%d%d/ && git add data%d%d.txt && git commit -m \"updated data file\" && cd .. && cd ..",
                    edit_row, 
                    edit_student,
                    edit_row, 
                    edit_student
                );
                popen(command_add, "r");
                printf("Marks updated\n");
                break;
            } 
            case 4: {
                    printf("Displaying Overall Statistics\n");
                    // Displaying the contents of all files
                    int sum = 0;
                    int max = -1;
                    int min = 10000;
                    for(int i=1; i<=no_of_faculty; i++)
                    {
                        for(int j=1; j<=no_of_students; j++)
                        {
                            char filename[MAXLEN];
                            sprintf(filename, "./data/data%d%d/data%d%d.txt", i, j, i, j);
                            //printf("Filename :  %s\n", filename);
                            FILE *ptr = fopen(filename, "r");
                            if(ptr == NULL)
                            {
                                printf("File could not be opened");

                            }
                            else
                            {
                                char ch = fgetc(ptr);
                                int mark = 0;
                                while(ch != EOF && ch != '\n'){
                                    printf("%c", ch);
                                    mark = mark*10 + (int)(ch-'0');	
                                    ch = fgetc(ptr);
                                }
                                //marks[i] = mark;
                                max = (mark>max) ? mark:max;
                                min = (mark<min) ? mark:min;
                                sum+=mark;
                            }
                            fclose(ptr);
                            printf("\t\t");
                            }
                        printf("\n");
                    } 
                    printf("\n-------------------------------------------------------------------------------------------------\n");
                    printf("Total of all scores: %d\n", sum);
                    printf("Average of all scores: %f\n", (float)sum/(no_of_students*no_of_faculty));
                    printf("Maximum score is %d and Minimum score is %d\n", max, min);
                    printf("\n-------------------------------------------------------------------------------------------------\n");
                    break;   
                }     
            case 5: {
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
        printf("2. Display marks\n"); //needs to be implemented
        printf("3. Broadcast a message\n"); //this is not there
        printf("4. Edit marks table\n");
        printf("5. Undo edit operation\n");
        printf("6. View version history\n");
        printf("> ");
        // each faculty is a row, each student is a column
        scanf("%d", &facultyChoice);
        //Finding the faculty number from the username
        int count = 0;      //printf("username_length: %d \n ",username_length);
        for(int i=username_length-1; i>=7; i--) {
            count = count*10 + (int)(username[i] -'0');
        }
        int faculty_number = count;
        //printf("Faculty number %d\n\n", faculty_number);
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
        	    int edit_student = 0;
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


                // Displaying the contents of the faculty files.
                for(int i=1; i<=no_of_students; i++)
                {
                    char filename[MAXLEN];
                    sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number, i, faculty_number, i);
                    //printf("Filename :  %s\n", filename);

                    FILE *ptr = fopen(filename, "r");
                    if(ptr == NULL)
                    {
                        printf("File could not be opened");

                    }
                    else
                    {
                        char ch = fgetc(ptr);
                        while(ch != EOF && ch != '\n'){
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
                char filename[MAXLEN];
                sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number, edit_student, faculty_number, edit_student);
                // printf("Filename, %s\n", filename);


		// writing the updated marks to the file
                FILE *ptr = fopen(filename, "w");
                if(ptr == NULL){
                    printf("file could not be opened\n");
                }
                fprintf(ptr, "%d", updated_marks);
                fclose(ptr);

                char command_add[MAXLEN];

                sprintf(
                    command_add, 
                    "cd data/data%d%d/ && git add data%d%d.txt && git commit -m \"updated data file\" && cd .. && cd ..",
                    faculty_number, 
                    edit_student,
                    faculty_number, 
                    edit_student
                );
                popen(command_add, "r");
                break;
            }
	    case 5:{
        	    int edit_student = 0;
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


                // Displaying the contents of the faculty files.
                for(int i=1; i<=no_of_students; i++)
                {
                    char filename[MAXLEN];
                    sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number, i, faculty_number, i);
                    // printf("Filename, %s\n", filename);

                    FILE *ptr = fopen(filename, "r");
                    if(ptr == NULL)
                    {
                        printf("File could not be opened");

                    }
                    else
                    {
                        char ch = fgetc(ptr);
                        while(ch != EOF && ch != '\n'){
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
                char filename[MAXLEN];
                sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number, edit_student, faculty_number, edit_student);
                // printf("Filename, %s\n", filename);


                char command_add[MAXLEN];

                sprintf(
                    command_add, 
                    "cd data/data%d%d && git revert HEAD --no-edit && cd .. && cd ..",
                    faculty_number,
                    edit_student
                );
                

                popen(command_add, "r");
                break;
	        }
            case 6:{
        	    int edit_student = 0;
                for(int i=1; i<=no_of_students; i++)
                {
                    printf("Student no %d\t", i);
                }
                printf("\n");
                // Displaying the contents of the faculty files.
                for(int i=1; i<=no_of_students; i++)
                {
                    char filename[MAXLEN];
                    sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number, i, faculty_number, i);
                    // printf("Filename, %s\n", filename);

                    FILE *ptr = fopen(filename, "r");
                    if(ptr == NULL)
                    {
                        printf("File could not be opened");
                    }
                    else
                    {
                        char ch = fgetc(ptr);
                        while(ch != EOF && ch != '\n'){
                            printf("%c", ch);	
                            ch = fgetc(ptr);
                        }
                    }
                        fclose(ptr);
                        printf("\t\t");
                }

                printf("\nEnter the student number whose marks version history you want to view: ");
                scanf("%d", &edit_student);
                // printf("Student number chosen is %d\n", edit_student);
                // getting the filename
                char filename[MAXLEN];
                sprintf(filename, "./data/data%d%d/data%d%d.txt", faculty_number, edit_student, faculty_number, edit_student);
                // printf("Filename, %s\n", filename);

                popen("cd .. && cd ..", "r");
                char command_view[MAXLEN];
                sprintf(
                    command_view, 
                    "cd data/data%d%d && git log -p --oneline -- data%d%d.txt",
                    faculty_number,
                    edit_student, 
                    faculty_number,
                    edit_student
                );

                FILE* p = popen(command_view, "r");
                char log_message[MAXLEN];
                fscanf(p, "%s", log_message);
                printf("The Version history of file is as follows: \n %s\n", log_message);
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
