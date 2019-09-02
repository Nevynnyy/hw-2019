#define _POSIX_SOURCE

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define INPUT_STRING_SIZE 80
#define CNORMAL "\x1B[0m"
#define CYELLOW "\x1B[33m"

#include "io.h"
#include "parse.h"
#include "process.h"
#include "shell.h"

#include <limits.h>


char prevDir[PATH_MAX] = ".";
char currDir[9999];
int prevCaunt=0;
int currCaunt=0;

int cmd_help(tok_t arg[]);
int cmd_quit(tok_t arg[]);
int cmd_pwd(tok_t arg[]);
int cmd_cd(tok_t arg[]);
int cmd_wait(tok_t arg[]);
int cmd_fg(tok_t arg[]);
char thePath[PATH_MAX];

/**
 *  Built-In Command Lookup Table Structures
 */
typedef int cmd_fun_t (tok_t args[]); // cmd functions take token array and return int
typedef struct fun_desc {
    cmd_fun_t *fun;
    char *cmd;
    char *doc;
} fun_desc_t;
/** TODO: add more commands (pwd etc.) to this lookup table! */
fun_desc_t cmd_table[] = {
    {cmd_help, "help", "show this help menu"},
    {cmd_quit, "quit", "quit the command shell"},
    {cmd_pwd,"pwd","Display current working directory"},
    {cmd_cd,"cd","Changes directory from current to specified"},
    {cmd_wait,"wait","Used with a particular process id or job id to wait for completing any running process"},
    {cmd_fg,"fg","Resumes process from background to foreground"},
};

/**
 *  Determine whether cmd is a built-in shell command
 *
 */
int lookup(char cmd[]) {
    unsigned int i;
    for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
        if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0)) return i;
    }
    return -1;
}

/**
 *  Print the help table (cmd_table) for built-in shell commands
 *
 */
int cmd_help(tok_t arg[]) {
    unsigned int i;
    for (i = 0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
        printf("%s - %s\n",cmd_table[i].cmd, cmd_table[i].doc);
    }
    return 1;
}

 int cmd_pwd(tok_t arg[]){
    char *getcwd(char *buf, size_t size);
    if(getcwd(thePath,sizeof(thePath))==NULL){
        printf("No directory found");
        return 1;

    }else{
        printf(thePath);
        return 1;
    }

}

int cmd_cd(tok_t arg[]){
    char k[PATH_MAX];
    char dest[9999]="";
    tok_t first=arg[0];
    char temp[sizeof(prevDir)];
    strcpy(temp ,prevDir);
    getcwd(prevDir,sizeof(prevDir));

    if(first[0]=='~'){

        if(strstr(arg[0],"~")!=NULL){
            char v[999]="";
            strcat(v, getenv("HOME"));

            char vv[999]="";
            strcat(vv, arg[0]);
            memmove(vv, vv+1, strlen(vv));
            strcat(v, vv);

            chdir(v);
        }else{
            strcat(dest,getenv("HOME"));
            chdir(getenv("HOME"));
        }

    }else if(first[0]=='-'){

            chdir(temp);
    }else{
        chdir(arg[0]);
    }

    return 1;

}

int cmd_wait(tok_t arg[]){
    char pName[999]="";
    tok_t first=arg[0];
    strcat(pName,first);
    waitpid(pName, 0, WNOHANG);
    return 1;
    
}

int cmd_fg(tok_t arg[]){
    int i=0;
    while (arg[i]){
        ++i;
    }
    
    if(i==1){
        char pName[999]="";
        tok_t first=arg[0];
        strcat(pName,first);
        signal(pName,SIGCONT);
    }else{
        pid_t pro=getpid();
        char pName[999]="";
        //printf(pro);
        //strcat(pName,pro);
    
        //signal(pName,SIGCONT);
    }

}


/**
 *  Quit the terminal
 *
 */
int cmd_quit(tok_t arg[]) {
    printf("Bye\n");
    exit(0);
    return 1;
}

/**
 *  Initialise the shell
 *
 */
void init_shell() {
    // Check if we are running interactively
    shell_terminal = STDIN_FILENO;

    // Note that we cannot take control of the terminal if the shell is not interactive
    shell_is_interactive = isatty(shell_terminal);

    if( shell_is_interactive ) {

        // force into foreground
        while(tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp()))
            kill( - shell_pgid, SIGTTIN);

        shell_pgid = getpid();
        // Put shell in its own process group
        if(setpgid(shell_pgid, shell_pgid) < 0){
            perror("Couldn't put the shell in its own process group");
            exit(1);
        }

        // Take control of the terminal
        tcsetpgrp(shell_terminal, shell_pgid);
        tcgetattr(shell_terminal, &shell_tmodes);
    }

    /** TODO */
    // ignore signals
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
}

/**
 * Add a process to our process list
 *
 */
void add_process(process* p)
{
    int num=0;
    process* temp;
    
    if(first_process==NULL){
        first_process=p;
    }else{
        temp=first_process;
        while(temp->next!=NULL){
            temp=temp->next;

        }
        temp->next=p;
        p->prev=temp;
        p->next=NULL;
    }
    

    /** TODO **/
}

/**
 * Creates a process given the tokens parsed from stdin
 *
 */
process* create_process(tok_t* tokens)
{

    process* p=malloc(sizeof(process));

    p->completed=false;
    p->background=false;
    p->stopped=false;
    p->pid=-1;
    p->argv=tokens;
    p->status=0;
    int i = 0;
    while(tokens[i]){
        i++;
    }

    char inp[999]="";
    strcat(inp,tokens[i-1]);
    if(inp[0]=='&'){
      p->background=true;
      tok_t* newTokens=malloc((i-1)*sizeof(tok_t));
      for(int v=0;v<i-1;++v){
          newTokens[v]=tokens[v];
      }
      p->argv=newTokens;
      

    }
    p->status=1;
    p->argc=i;
    add_process(p);
    /** TODO **/
    return p;
}

/**
 * Main shell loop
 *
 */
int shell (int argc, char *argv[]) {
    int lineNum = 0;
    pid_t pid = getpid();	// get current process's PID
    pid_t ppid = getppid();	// get parent's PID
    pid_t cpid;             // use this to store a child PID

    char *s = malloc(INPUT_STRING_SIZE+1); // user input string
    tok_t *t;			                   // tokens parsed from input
    // if you look in parse.h, you'll see that tokens are just C-style strings (char*)

    // perform some initialisation
    init_shell();

    fprintf(stdout, "%s running as PID %d under %d\n",argv[0],pid,ppid);
    /** TODO: replace "TODO" with the current working directory */
    char Path[PATH_MAX];
    char *getcwd(char *buf, size_t size);
    if(getcwd(Path,sizeof(Path))==NULL){}

    fprintf(stdout, CYELLOW "\n%d %s# " CNORMAL, lineNum,Path);

    // Read input from user, tokenize it, and perform commands
    while ( ( s = freadln(stdin) ) ) {

        
        t = getToks(s);            // break the line into tokens
        int fundex = lookup(t[0]); // is first token a built-in command?
        if( fundex >= 0 ) {
            cmd_table[fundex].fun(&t[1]); // execute built-in command
        } else {
            /** TODO: replace this statement to call a function that runs executables */
            /**
             * HOW TO DO THIS PART
             * 
             * create a process
             * then run launch process on that process
             * all the exec stuff should be in launch process
             * don't be afraid to make new functions
             *
            */
           int i=0;
           while(t[i]){
               ++i;
           }
           
           if (i==0){
               
           }else{
               process* child = create_process(t);
                launch_process(child);
           }
          
        }
        
        
        lineNum++;
        /** TODO: replace "TODO" with the current working directory */
        char Path[PATH_MAX];
        char *getcwd(char *buf, size_t size);
        if(getcwd(Path,sizeof(Path))==NULL){}
        fprintf(stdout, CYELLOW "\n%d %s# " CNORMAL, lineNum,Path);
    }
    return 0;
}
