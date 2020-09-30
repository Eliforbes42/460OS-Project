// login.c : Upon entry, argv[0]=login, argv[1]=/dev/ttyX
#include "ucode.c"

int in, out, err; 
char uname[128], password[128];

int grabLine(char *s, int pwFd){
    int i = 0, done = 0;
    s[i] = 0;

    while(read(pwFd, &s[i++], 1) && !done){
        if(s[i] == '\n' || s[i] == '\r')
            done = 1;
    }   
    if(s[0] == 0)
        return 0;
    else
        return 1;
}

int checkPw(int pwFd, int *in, int *out, int *err, char *av){
    char pwLine[512], prevLine[512];
    char pName[128], pPass[128], pGid[32], pUid[32],
        pFullName[128], pHome[128], pProg[32];
    int i = 0, j = 0, k = 0, l = 1, found = 0, bad = 0;
    
    getline(pwLine);
    while(strlen(pwLine) > 0 && !found){
        //printf("line=%s\n",pwLine);
        j = strlen(pwLine);
        l=1; 
        i = k = 0;
        while(i < j) {
            switch(l){
                case 1: pName[k++] = pwLine[i++];
                    break;
                case 2: pPass[k++] = pwLine[i++];
                    break;
                case 3: pGid[k++] = pwLine[i++];
                    break;
                case 4: pUid[k++] = pwLine[i++];
                    break;
                case 5: pFullName[k++] = pwLine[i++];
                    break;
                case 6: pHome[k++] = pwLine[i++];
                    break;
                case 7: pProg[k++] = pwLine[i++];
                    break;
                default: break;
            }
            if(pwLine[i] == ':' || pwLine[i] == 0){
                switch(l){
                    case 1: pName[k] = 0;
                    //printf("name=%s-\n", pName);
                        break;
                    case 2: pPass[k] = 0;
                    //printf("pass=%s-\n", pPass);
                        break;
                    case 3: pGid[k] = 0;
                    //printf("gid=%s-\n", pGid);
                        break;
                    case 4: pUid[k] = 0;
                    //printf("uid=%s-\n", pUid);
                        break;
                    case 5: pFullName[k] = 0;
                    //printf("fullName=%s-\n", pFullName);
                        break;
                    case 6: pHome[k] = 0;
                    //printf("home=%s-\n", pHome);
                        break;
                    case 7: pProg[k] = 0; pProg[k-1] = 0;
                    //printf("prog=%s-\n", pProg);
                        break;
                    default: break;
                }
                i++; l++; //advance to next char & case
                k = 0; //reset result index
            }
            int r1 = strcmp(uname, pName);
            int r2 = strcmp(password, pPass);
            //printf("%s, %s, %s, %s\n", uname, pName, password, pPass);
           // printf("r1=%d, r2=%d\n", r1,r2);
            if(r1 == 0 && r2 == 0 && l > 7)
            { 
                found = 1;
                int uidInt = atoi(pUid);
                printf("EFLOGIN : Welcome %s!\n", pName);
                printf("EFLOGIN : cd to HOME=%s  change uid to %d\n", pHome,uidInt);
                chuid(uidInt, atoi(pGid));
                chdir(pHome);
                close(pwFd);
                //close(*out); close(*err);
                *in = open(av, O_RDONLY);
               // *out = open(av, O_WRONLY);
               // *err = open(av, O_WRONLY);
                exec(pProg);
                return 1;  
            }  
        }
    ncpy(prevLine, pwLine, strlen(pwLine));
    getline(pwLine);
    if(!strcmp(prevLine, pwLine))
        return 0;
    }
    return 0; //no passwords left -- so no match, return false

        //(6). if (user has a valid account){
        //(7). change uid, gid to user's uid, gid; // chuid()
        //change cwd to user's home DIR // chdir()
        //close opened /etc/passwd file // close()
        //(8). exec to program in user account // exec()
}

main(int argc, char *argv[])
{
    int pw;
//(1). close file descriptors 0,1 inherited from INIT.
    close(0); close(1);
//(2). open argv[1] 3 times as in(0), out(1), err(2).
    in = open(argv[1], O_RDONLY);
    out = open(argv[1], O_WRONLY);
    err = open(argv[1], O_WRONLY);
//(3). // set tty name string in PROC.tty
    fixtty(argv[1]);
    printf("login: tty=%s\n", argv[1]);
    printf("EFLOGIN : open %s as stdin=0, stdout=1, stderr=2\n", argv[1]);
//(4). open /etc/passwd file for READ;
    
    while(1){
        //(5). 
        printf("login:"); gets(uname);
        printf("password:"); gets(password);
        
        close(in);
        pw = open("/etc/passwd", O_RDONLY);
        
        if(checkPw(pw, &in, &out, &err, argv[1]))
            break;
        
        close(pw); //close password file
        in = open(argv[1], O_RDONLY); //re-open stdin for input
        
    }
    close(out); close(err); close(pw);
    in = open(argv[1], O_RDONLY);
    out = open(argv[1], O_WRONLY);
    err = open(argv[1], O_WRONLY);
    printf("login failed, try again\n");

}