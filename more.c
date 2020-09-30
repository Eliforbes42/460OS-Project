/********** test.c file *************/
#include "ucode.c"

int main(int argc, char *argv[ ])
{
    int fdIn, line[128]; char curTty[128];
    switch(argc){
        case 1: //more
            gettty(curTty);
            //close(0)
            fdIn = open(curTty, O_RDONLY);//reopen stdin

            while(getline(line))
            {
                printf("%s", line);
                fgetc(fdIn);
            }
            close(fdIn);
            break;
        case 2: //more f1
            fdIn = open(argv[1], O_RDONLY);
            while(fgetline(line, fdIn)){
                printf("%s",line);
                getc();
            }
            break;
        default: break;
    }
}