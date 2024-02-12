#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
#include <signal.h>

#define PARAM_a    1              //参数a
#define PARAM_l    2              //参数l
#define PARAM_i    4              //参数i
#define PARAM_r    8              //参数r
#define PARAM_t    16             //参数t
#define PARAM_R    32             //参数R
#define PARAM_s    64             //参数s

void my_readdir(const char *path);
int main(int argc,char*argv[])
{
    int i, j, k, num;
    char*path=(char*)malloc(sizeof(char)*100);
    char param[8];
    int flag_param=0;
    struct stat*buf;

    num=0;
    j=0;

    for(i=1;i<argc;i++)
    {
        if(argv[i][0]=='-')
        {
            for(k=1;k<strlen(argv[i]);j++,k++)
            {
                param[j]=argv[i][k];
            }
            num++;
        }
    }

    for(i=0;i<j;i++)
    {
        if(param[i]=='a')
        {
            flag_param|=PARAM_a;
        }
        else if(param[i]=='l')
        {
            flag_param|=PARAM_l;
        }
        else if(param[i]=='R')
        {
            flag_param|=PARAM_R;
        }
        else if(param[i]=='r')
        {
            flag_param|=PARAM_r;
        }
        else if(param[i]=='t')
        {
            flag_param|=PARAM_t;
        }
        else if(param[i]=='i')
        {
            flag_param|=PARAM_i;
        }
        else if(param[i]=='s')
        {
            flag_param|=PARAM_s;
        }
    }

    if(num+1==argc)
    {
        getcwd(path,100);
        display_dir(flag_param,path);
        free(path);
		return 0;
    }
    else
    {
        strcpy(path,argv[num+1]);
        stat(path,buf);
        if(S_ISDIR(buf->st_mode))
        {
            display_dir(flag_param,path);
        }
        else
        {
            display(flag_param,path);
        }
        free(path);
        return 0;
    }
}
void my_readdir(const char *path)
{
    DIR*dir;
    struct dirent*ptr=opendir(path);
    while(ptr!=NULL)
    {
        if(ptr->d_name[0]!='.')
        {
            printf("%s ",ptr->d_name);
        }
    }
    closedir(dir);
}
