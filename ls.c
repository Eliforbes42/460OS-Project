/** Program C8.2: ls.c: run as a.out [filename] **/

 #include <time.h>
 #include "ucode.c"

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

struct stat mystat, *sp;

//my non-destructive take on basename()
int baseName(char * name){
    char bn[256];
    int i = strlen(name)-1, j = 0, k = 0, found = 0;
    while(i && !found){
        if(i == strlen(name)-1 && name[i] == '/')//remove trailing '/'
        {
            i--;
            if(i<0){
                printf("/\n");
                return 1;
            }
        }
        if(name[i] == '/'){
            found = 1;
            continue;//i.e. break;
        }
        i--;
    }
    i++;
    while(name[i]){
        bn[j++] = name[i++];
    }
    bn[j] = 0;
    printf("%s ", bn);
    return 1;
}

int ls_file(char *fname) // list a single file
{
    struct stat fstat, *sp = &fstat;
    int r, i;
    char sbuf[4096], ftime[64], tname[256];

    r = stat(fname, sp); // lstat the file

    if ((sp->st_mode & 0xF000) == 0x4000)//S_ISDIR(sp->st_mode))
        mputc('d');//printf("%c",'d'); // print file type as d
    if ((sp->st_mode & 0xF000) == 0x8000)//S_ISREG(sp->st_mode))
        mputc('-');//printf("%c",'-'); // print file type as -
    if ((sp->st_mode & 0xF000) == 0xA000)//S_ISLNK(sp->st_mode))
        mputc('l');//printf("%c",'l'); // print file type as l

    for (i=8; i>=0; i--){
        if (sp->st_mode & (1<<i))
            mputc(t1[i]);//printf("%c", t1[i]); // print permission bit as r w x
        else
            mputc(t2[i]);//printf("%c", t2[i]); // print permission bit as -
    }
    printf(" %d ", sp->st_nlink); // link count
    printf("%d ", sp->st_uid); // uid
    printf("%d ", sp->st_size); // file size
    
    //strcpy(ftime, ctime(&sp->st_ctime));
    //ftime[strlen(ftime)-1] = 0; // kill \n at end
    //printf("%s ",ftime); // time in calendar form
    
    baseName(fname); //my own BaseName function!
    //printf("%s", basename(fname);// file basename
    
    if (sp->st_mode == 0777){//S_ISLNK(sp->st_mode))// if symbolic link
        r = readlink(fname, sbuf);
        printf(" -> %s", sbuf); // -> linked pathname
    }
    mputc('\n');
}
int ls_dir(char *dname) // list a DIR
{
    char name[256], buf[1024], *cp; // EXT2 filename: 1-255 chars
    DIR *dp;
    DIR *ep;
    int fd, n;
    // open DIR to read names
    fd = open(dname, O_RDONLY); // opendir() syscall
    n = read(fd, buf, 1024);
    
    dp = (struct ext2_dir_entry_2 *)buf; //dir pointer
    cp = buf; //char pointer within dir

    while (cp+dp->rec_len < buf + 1024){ // readdir() syscall
        strcpy(name, ep->name);
        if (!strcmp(name, ".") || !strcmp(name, ".."))
            continue; // skip over . and .. entries
        strcpy(name, dname);
        if(name[strlen(name)-1] != '/')
            strcat(name, "/");
        strcat(name, dp->name);
        ls_file(name); // call list_file()

        cp += dp->rec_len;
        dp = (struct ext2_dir_entry_2 *)cp;
    }
}
int main(int argc, char *argv[])
{
    struct stat mystat, *sp;
    int r;
    char *s;
    char filename[1024], cwd[1024];
    
    s = argv[1]; // ls [filename]
    if (argc == 1) // no parameter: ls CWD
        s = "./";
    sp = &mystat;
    if ((r = stat(s, sp)) < 0){ // stat() syscall
        //perror("ls"); 
        exit(1);
    }
    strcpy(filename, s);
    printf("filename=%s-\ns=%s-\n",filename, s);
    if (s[0] != '/'){ // filename is relative to CWD
        getcwd(cwd); // get CWD path
        printf("LS : from %s\n", cwd);
        strcpy(filename, cwd);
        if(s[0] != '.'){
            if(filename[strlen(filename)-1] != '/')
                strcat(filename, "/");
            strcat(filename,s); // construct $CWD/filename
        }
    }
    if ((sp->st_mode & 0xF000) == 0x4000)//S_ISDIR(sp->st_mode))if (S_ISDIR(sp->st_mode))
        ls_dir(filename); // list DIR
    else
        ls_file(filename); // list single file
        
}