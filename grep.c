/********** test.c file *************/
#include "ucode.c"

int containStr(char *s, char *c){
    
    int i, j, k, 
    sLen = strlen(s), 
    pLen = strlen(c), 
    position = -1;


    if (pLen > sLen)
        return -1; //error out
    

    for (i = 0; i <= sLen - pLen; i++) {
        position = k = i;
        for (j = 0; j < pLen; j++) {
            if (c[j] == s[k])
                k++;
            else 
                break;        
        }
        if (j == pLen)
            return position;        
    }
    return -1;
}

int main(int argc, char *argv[ ])
{
    char line[1024];
    
    switch(argc){
        case 3: //grep PATTERN file1
            close(0);//stdin            
            open(argv[2], O_RDONLY);   //open file1 & fall into case2
        case 2: //grep PATTERN
            while(getline(line)){
                if(!strcmp(line,""))
                    break;
                if(containStr(line, argv[1])!=-1)
                    printf("%s", line);
                //printc('\n');
                strcpy(line,"");
            } 
            break;
    }
    return 0;
}