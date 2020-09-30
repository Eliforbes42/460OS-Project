#include "ucode.c"

int console; //console PROC forked before use in parent --all good
int serial1, serial2; //UART Serial Port PROCs

int parent() //P1's Code
{
    int pid, status;
    while(1){
        printf("INIT: wait for zombie child\n");
        pid = wait(&status);
        printf("pid=%d\n",pid);
        getc();
        printf("serial1=%d\nconsole=%d\n",serial1,console);
        if(pid == serial1) {
            printf("INIT: forks new serial login\n");
            serial1 = fork();
            printf("serial1=%d\n",serial1);
            if(serial1)
                continue;
            else    
                exec("login /dev/ttyS0"); //new serial(ttyS0) login process
        }

        if(pid == console) {     //if console login process died
            printf("INIT: forks a new console login\n");
            console = fork(); //fork another one
            if(console)
                continue;
            else
                exec("login /dev/tty0"); //new console(tty0) login process
        }
        // if(pid == serial2) {
        //     printf("INIT: forks new serial2 login\n");
        //     serial2 = fork();
        //     if(serial2)
        //         continue;
        //     else    
        //         exec("login /dev/ttyS1"); //new serial(ttyS0) login process
        // }
        //printf("INIT: I just buried an orphan child proc %d\n", pid);
    }
}

int serialParent()
{
    int pid, status;
    while(1){
        printf("INIT: wait for zombie child\n");
        pid = wait(&status);
        if(pid == serial1) {
            printf("INIT: forks new serial login\n");
            serial1 = fork();
            if(serial1)
                continue;
            else    
                exec("login /dev/ttyS0"); //new serial(ttyS0) login process
        }
        //printf("INIT: Buried Orphan Child PROC %d\n");//Moment of silence..\n", pid);
    }    
}
int serialParent2()
{
    int pid, status;
    while(1){
        printf("INIT: wait for zombie child\n");
        pid = wait(&status);
        if(pid == serial2){
            printf("INIT: forks new serial login\n");
            serial2 = fork();
            if(serial2)
                continue;
            else    
                exec("login /dev/ttyS1"); //new serial(ttyS0) login process
        }
        //printf("INIT: I just buried an orphan child proc %d\nMoment of silence..\n", pid);
    } 
}

main(){
    int in, out; //file descriptors, terminal I/O
    in = open("dev/tty0", O_RDONLY); //fd0 -- stdin
    out = open("dev/tty0", O_WRONLY); //fd1 -- stdout - what we need//
    printf("INIT: fork a login proc on console\n");

    console = fork();    //fork P2, console(tty0)
   // getc();
    //serial1 = fork();    //fork P3, serial1(ttyS0)
   // getc();
    if(console) //parent
        parent();
    else        //child: exec to login on tty0(console)
    {        
        exec("login /dev/tty0");  
        exec("login /dev/ttyS0");
        //exec("login /dev/ttyS1");
    }    
      


//    serial2 = fork();    //fork P4, serial2(ttyS1)
//    if(serial2) //serialParent2 //could use fn. w/ args?
//         serialParent2();
//    else
//        exec("login /dev/ttyS1");
}