/********** test.c file *************/
#include "ucode.c"

char toUpper(char c){
    if('a' <= c && c <= 'z')
        return (c-'a'+'A');
    else
        return c;
}

char *arrToUpper(char *s, int len){
    int i = 0;
    while(i < len){
        s[i] = toUpper(s[i]);
        i++;
    }
    return s;
}

int main(int argc, char *argv[ ])
{
    int i;
    char line[4096];   

    switch(argc){
        case 1: //l2u
            while(getline(line))
            {
                if(!strcmp(line,""))
                    break;
                printf("%s", arrToUpper(line, 4096));
                printc('\n');
            }    
            break;
        case 3: //l2u f1 f2
            close(0);//stdin
            close(1);//stdout
            open(argv[1], O_RDONLY);
            open(argv[2], O_WRONLY|O_CREAT);
            while(getline(line))
                printf("%s", arrToUpper(line, 4096));
            break;
    }
    return 0;
  //check args //redirect
    //files --redirect files to stdin/out
    //no files --stdin->out
}