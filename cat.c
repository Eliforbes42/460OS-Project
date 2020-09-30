/********** test.c file *************/
#include "ucode.c"

int main(int argc, char *argv[ ])
{
    int i, n, r;
    char *bang = "!!!!!!!!!!!!!!!!\n",
         *purr = "ESF : CAT PURRRS\n", *tooMany = "Too many arguments!\n",
         line[4096];
    struct stat fstat, *sp = &fstat;
    int pid = getpid();
    write(2, bang, 17); //write to stderr
    //lseek(2, 0, 0);
    write(2, purr, 17); //write to stderr
    write(2, bang, 17); //write to stderr
  
    switch(argc){
        case 1: //ex. cat  -- take stdin - spit to stdout   
        while(gets(line))
                printf("\n%s",line);
            break;         
        case 2:// ex. cat f1
            //close stdin - open f1 as stdin - spit to stdout            
            while(getline(line))
                    printf("%s",line);
            // while(n = read(0, line, 4096))
            //     write(1, line, n);
            break;
        case 3://ex. cat > f1 || cat >> f1 || cat < f1
            //close stdout
            //open f1 as stdout
            //read input & spit to 'stdout'(f1) 
            if(!strcmp(argv[1], ">>")){
                r = stat(argv[2], sp);
                lseek(1, sp->st_size, 0); //0==SEEK_SET
            }
            char *direction = argv[1];
            
            while(n = read(0, line, 4096))//gets(line))
            {
                write(1, line, n);//printf("%s",line);
                if(strcmp(direction, "<"))
                    write(2, line, strlen(line)); //write to screen
            }
            break;          
        case 4://ex. cat f1 > f2 || cat f1 >> f2
            //close stdin & stdout
            //open f1 as stdin //open f2 as stdout (create if necessary)
            //spit stdin to stdout end to end (block by block?)
            
            if(!strcmp(argv[2], ">>")){
                r = stat(argv[3], sp);
                lseek(1, sp->st_size, 0);//0==SEEK_SET
            }
            while(n = read(0, line, 4096))
                write(1, line, n);
            break;
        default:// write(2, tooMany, strlen(tooMany));
        while(n = read(0, line, 4096))
                write(1, line, n);
            break;
    }
    
//   printf("ESF: argc = %d\n", argc);
//   for (i=0; i<argc; i++)
//     printf("ESF: argv[%d] = %s\n", i, argv[i]);
//   printf("ESF: exit\n");
}