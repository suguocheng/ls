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

#define PARAM_NONE 0
#define PARAM_A    1              //参数a
#define PARAM_L    2              //参数l
#define PARAM_I    4              //参数i
#define PARAM_R    8              //参数r
#define PARAM_T    16             //参数t
#define PARAM_RR   32             //参数R
#define PARAM_S    64             //参数s
#define MAXROWLEN  155            //每行所用最大格数
void ls();
int main(int argc,char*argv[])
{
    int i, j, k, num;
    char*path=(char*)malloc(sizeof(char)*100);
    char param[32];
    int flag_param=PARAM_NONE;
    struct stat buf;

    num=0;
    j=0;

    signal(SIGINT,sighandler);

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
            flag_param|=PARAM_A;
        }
        else if(param[i]=='l')
        {
            flag_param|=PARAM_L;
        }
        else if(param[i]=='R')
        {
            flag_param|=PARAM_RR;
        }
        else if(param[i]=='r')
        {
            flag_param|=PARAM_R;
        }
        else if(param[i]=='t')
        {
            flag_param|=PARAM_T;
        }
        else if(param[i]=='i')
        {
            flag_param|=PARAM_I;
        }
        else if(param[i]=='s')
        {
            flag_param|=PARAM_S;
        }
    }

    if(num+1==argc)
    {
        getcwd(path,100);
        display_dir(flag_param,path);
		return 0;
    }
    else
    {
        strcpy(path,argv[num+1]);
        if(S_ISDIR(buf.st_mode))
        {
            display_dir(flag_param,path);
        }
        else
        {
            display(flag_param,path);
        }
        return 0;
    }
}