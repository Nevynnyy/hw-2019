#include "process.h"
#include "shell.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <termios.h>
#include "parse.h"



/**
 *  Executes the process p.
 *  If the shell is in interactive mode and the process is a foreground process,
 *  then p should take control of the terminal.
 *
 */


void launch_process(process *p) {
    /** TODO **/
    setpgid(p->pid,p->pid);
    
    

    pid_t child1=fork();
    
    if(child1==0){
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
    

        if(p->background){
                put_process_in_background(child1,1);
                waitpid(p->pid,&p->status,WNOHANG);
        }else{
        put_process_in_background(child1,0);
        waitpid(p->pid,&p->status,0);
        }

        if(execv(p->argv[0],&p->argv[0])){
            char theEnvPath[999]="";
            strcat(theEnvPath,getenv("PATH"));

            //int m=(strlen(theEnvPath)<=INT_MAX) ? (int)((ssize_t)strlen(theEnvPath)) : -1;

            tok_t* arr=getToks(theEnvPath);

            int i = 0;
            while(arr[i]){
                i++;
            }

            int j=0;
            int flag=0,namePos=0;
            int names[99];
            int v=0;
            while (p->argv[j]){
                char theField[999]="";
                strcat(theField,p->argv[j]);
                if(theField[0]=='<'){
                    flag=-1;
                    namePos=j+1;
                    names[v]=j+1;
                    ++v;
                }else if(theField[0]=='>'){
                    flag=1;
                    namePos=j+1;
                        names[v]=j+1;
                    ++v;
                }

                ++j;
            }

            if(j==0){
                
            }

            
            for(int n=0;n<i;++n){
                
                char theField[999]="";
                strcat(theField,arr[n]);
                theField[strlen(theField)]='/';
                strcat(theField,p->argv[0]);

                if(flag==1){
                    char * input[namePos];
                    for(int v=0;v<namePos;++v){
                        if(v==namePos-1){
                            input[v]=NULL;
                        }else{
                            input[v]=p->argv[v];
                        }
                    }

                    for(int new=0;new<v;++new){
                        char fileName[999]="";
                        strcat(fileName,p->argv[names[new]]);
                        freopen(fileName,"a+",stdout);

                        if(execv(theField,input)){
                            
                        }else{
                            p->completed=true;
                            break;
                        }
                    }


                }else if(flag==-1){
                    char * input[namePos];
                    for(int v=0;v<namePos;++v){
                        if(v==namePos-1){
                            input[v]=NULL;
                        }else{
                            input[v]=p->argv[v];
                        }
                    }
                    char fileName[999]="";
                    strcat(fileName,p->argv[namePos]);
                    FILE* openFile=freopen(fileName,"r",stdin);

                    if(execv(theField,input)){
                        
                    }else{
                        p->completed=true;
                        break;
                    }

                }else{
                    char* input[j+1];
                    for(int v=0;v<j+1;++v){
                        if(v==j){
                            input[v]=NULL;
                        }else{
                            input[v]=p->argv[v];
                        }
                    }

                    if(execv(theField,input)){
                        
                    }else{
                        p->completed=true;
                        break;
                    }
                }

            }    
            
            char err[999]="'";
            strcat(err,p->argv[0]);
            char sec[5]="' ";
            strcat(err,sec);
            char firstPart[999]="Could not execute ";
            strcat(firstPart,err);
            char thirdPart[50]=": No such file or directory";
            strcat(firstPart,thirdPart);

            if(p->completed==false){
                printf("%s",firstPart);
                p->completed=true;
            }

        }

        exit(child1);
        
    }else{

        wait(0);

    }
            
}

/**
 *  Put a process in the foreground. This function assumes that the shell
 *  is in interactive mode. If the cont argument is true, send the process
 *  group a SIGCONT signal to wake it up.
 *
 */
void put_process_in_foreground (process *p, int cont) {
    /** TODO **/
    if(cont==1){
        signal(SIGCONT,0);
    }
}

/**
 *  Put a process in the background. If the cont argument is true, send
 *  the process group a SIGCONT signal to wake it up. 
 *
 */
void put_process_in_background (process *p, int cont) {
    /** TODO **/
    if(cont==1){
        p->background=true;
        signal(p->pid,SIGCONT);
    }
    
}

/**
 *  Prints the list of processes.
 *
 */
void print_process_list(void) {
    process* curr = first_process;
    while(curr) {
        if(!curr->completed) {
            printf("\n");
            printf("PID: %d\n",curr->pid);
            printf("Name: %s\n",curr->argv[0]);
            printf("Status: %d\n",curr->status);
            printf("Completed: %s\n",(curr->completed)?"true":"false");
            printf("Stopped: %s\n",(curr->stopped)?"true":"false");
            printf("Background: %s\n",(curr->background)?"true":"false");
            printf("Prev: %lx\n",(unsigned long)curr->prev);
            printf("Next: %lx\n",(unsigned long)curr->next);
        }
        curr=curr->next;
    }
}
