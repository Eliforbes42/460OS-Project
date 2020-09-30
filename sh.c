/***** sh.c file *****/
#include "ucode.c"

void clearStringArr(char *arr[], int len){
    int i = 0;
    for(; i < len; i++)
        arr[i] = "";
}

int contains(char *s, char c){
    int i = 0, j = strlen(s);
    for(; i < j; i++)
        if(s[i] == c)
            return 1;//true -- s contains c
    return 0;//false
}

//returns true if s contains any char's in c
int containsAny(char *s, char *c){
    int i = 0, j = 1, k = strlen(s), l = strlen(c);
    for(; i < k; i++)
        for(j = 1; j < l; j++)
            if(s[i] == c[j])
                return j;//return location
    return 0;//false
}
int containStr(char *s, char *c){
    int i = 0, j = 0, k = strlen(s), l = strlen(c), count = 0;
    for(; i < k; i++)
        for(j = 0; j < l; j++)
            if(s[i] == c[j]) {
                count++;
                if(count == l-1)
                    return i-count;//return location
            }
    return 0;//false
}

int ArrContainsStr(char *s[], char *c[], int numS, int numC){
    int i = 0, j = 0;
    for(; i < numS; i++){
        for(j = 0; j < numC; j++)
            if(!strcmp(s[i],c[j]))
                return j;
    }
    return -1;//false
}

void breakCmd(char *cmd[], int numCmd, char *cmd1, char *cmd2){
    int i = 0, bar = 0;
    while(i < numCmd){
        if(!bar){
            if(strcmp(cmd[i], "|")) {
                if(i!=0)
                    strcat(cmd1, " ");
                strcat(cmd1, cmd[i]);
            }    
            else
                bar = 1;
        }
        else {
            strcat(cmd2, cmd[i]);
            if(i!=numCmd-1)
                strcat(cmd2, " ");
        }
        i++;
    }
}

void pipeItUp(char *cmdLine){
    char *commands[128], cmd1[64] = "", cmd2[64] = "";
    int pid, pd[2];
    printf("cmdLine=%s-\n",cmdLine);
    int cmdCount = tokenize(cmdLine, commands);
    int i = 0;
    for(; i < cmdCount; i++)
        printf("PIPEcmd[%d]=%s\n", i, commands[i]);
    breakCmd(commands, cmdCount, cmd1, cmd2);
    
    
    printf("PIPEcmd1=%s\nPIPEcmd2=%s\n",cmd1,cmd2);
    pipe(pd); //make pipe: pd[0]=READ, pd[1]=WRITE
    pid = fork(); //fork child to share pipe
    if(pid){
        close(pd[1]); //close WRITE end of pipe
        dup2(pd[0],0); //redirect stdin to pipe READ end
       if(contains(cmd2, '|'))
           pipeItUp(cmd2);
       else
            exec(cmd2);            
    }
    else{
        close(pd[0]); //close pipe READ end
        dup2(pd[1],1); //redirect stdout to pipe WRITE end
        exec(cmd1);
    }
                        // //gets(cmdLine);
}

int main(int argc, char *argv[ ])
{  //index, procId, statusPtr, redirect, pipe descriptors, #commands
    int i, pid, status, redirect, pd[2], numCmd;
    char *curTty = "", cmdLine[128], tempLine[128],cmd1[64] = "", cmd2[64] = "",
        *commands[128], *redirects[4] = {"<",">",">>","|"};
    while(1)
    {   
        redirect = 0; numCmd = 0; clearStringArr(commands, 128); curTty = "";

        pid = getpid(); //get PROC# - distinguish shells
        printf("sh %d # ", pid); //shell input prompt

        gets(cmdLine);

        if(cmdLine[0] == 0 
          || cmdLine[0] == '\0' 
          || !strcmp(cmdLine,""))
            continue; //on blank line
        printf("cmdLine=%s-\n",cmdLine);
        
        // if(!strcmp(cmdLine, "")) //continue on blank line
        //     continue;
        if(!strcmp(cmdLine, "logout")) //cmd=="logout"
            exit(0);            //exit with 0 as value
        
        strcpy(tempLine,cmdLine);
        numCmd = tokenize(tempLine, commands);
        
        if(!strcmp(commands[0], "cd")){
            chdir(commands[1]);
            continue;
        }
        printf("numCmd=%d\n", numCmd);
        int k = 0, curFd, curFd0, numClosed=0; i = 0;
        while(i < numCmd) {
            printf("cmd[%d]=%s-\n", i, commands[i]);
            i++;
        }        
        if(!strcmp(commands[0], "cat") && contains(cmdLine, '|'))
            pipeItUp(cmdLine);
        if((redirect = ArrContainsStr(commands, redirects, numCmd, 4)) != -1)
        {   
            printf("redirect = %d -- %s\n", redirect, redirects[redirect]);
            switch(redirect){
                case 0: curTty = ""; gettty(curTty);
                        printf("tty=%s\n", curTty);
                        close(0);//close stdin
                        curFd = open(commands[2], O_RDONLY); //"a.out < f1"
                        pid = fork();
                        if(!pid)
                            exec(cmdLine);
                        else    
                            pid = wait(&status);
                        
                        close(curFd);
                        open(curTty, O_RDONLY);//open stdin
                    break;
                case 1: curTty = ""; gettty(curTty);
                        printf("tty=%s\n", curTty); //get tty - can reopen stdout
                        close(1); numClosed=1;//close stdout
                        if(!strcmp(commands[2], ">")) //cat f1 > f2
                        {
                            curFd = open(commands[3], O_WRONLY|O_CREAT);
                            close(0); numClosed = 2;
                            curFd0 = open(commands[1], O_RDONLY);
                        }
                        else
                            curFd = open(commands[2], O_WRONLY|O_CREAT);//cat > f2 
                        pid = fork();
                        if(!pid)
                            exec(cmdLine);
                        else
                            pid = wait(&status);                       
                    break;
                case 2: //close(0); open(rhs, O_RDONLY|O_CREAT); exec(lhs);
                        curTty = ""; gettty(curTty);
                        printf("tty=%s\n", curTty); //get tty - can reopen stdout
                        close(1); numClosed = 1;//close stdout
                        if(!strcmp(commands[2], ">>")) //cat f1 >> f2
                        {
                            curFd = open(commands[3], O_APPEND|O_WRONLY|O_CREAT);
                            close(0); numClosed = 2;
                            curFd0 = open(commands[1], O_RDONLY);
                        }    
                        else
                            curFd = open(commands[2], O_APPEND|O_WRONLY|O_CREAT);//cat >> f2 
                        pid = fork();
                        if(!pid)
                            exec(cmdLine);
                        else
                            pid = wait(&status);                                                
                    break;
                case 3: pipeItUp(cmdLine);  
                gets(cmdLine); //try to grab input, but this is skipped past
                    break;
                default: break;
            }
            switch(numClosed){
                case 1: close(1);
                        open(curTty, O_WRONLY);//open stdout
                    break;
                case 2: close(0); close(1);
                        open(curTty, O_RDONLY);//open stdin
                        open(curTty, O_WRONLY);//open stdout
                    break;
                default: break;
            }
            numClosed = 0;
            redirect = -1; curTty = "";
        }
        else{//if(redirect == -1){
            if(!strcmp(commands[0], "cat") && numCmd==2) {
                curTty = ""; gettty(curTty);
                printf("tty=%s\n", curTty);
                close(0);//close stdin
                curFd = open(commands[1], O_RDONLY); //cat f1
                printf("curFd=%d\n",curFd);
                pid = fork();
                if(!pid)
                    exec(cmdLine);
                else    
                    pid = wait(&status);
                
                close(0);
                open(curTty, O_RDONLY);//open stdin
            }
            else{
                pid = fork();
                if(!pid)
                    exec(cmdLine);
                else
                    pid = wait(&status);            
            }
        }
    }    
}