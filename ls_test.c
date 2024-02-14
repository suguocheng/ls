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

void arr_file(int flag_param,char*path,char**filename);
void display(int flag_param,char*path);
int read_dir(const char *path,char**filename);
int main(int argc,char*argv[])
{
    int i, j, k, num;
    char*path=(char*)malloc(sizeof(char)*100);
    char*filename[100]=(char**)malloc(sizeof(char*)*100);
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
int read_dir(const char *path,char**file)
{
    DIR*dir;
    struct dirent*ptr=opendir(path);
    int i=0;
    while(ptr!=NULL)
    {
        *file[i]=ptr->d_name;
        i++;
    }
    closedir(dir);
    return i;
}
void arr_file(int flag_param,char*path,char**filename)
{
    struct stat *buf;
    __time_t filetime[100]=(__time_t*)malloc(sizeof(__time_t)*100);
    int count=read_dir(path,filename);
    if(flag_param&PARAM_t)
    {
        
        for(int i=0;i<count;i++)
        {
            stat(filename[i],buf);
            filetime[i]=buf->st_mtime;
        }
        for(int i=0;i<count;i++)
        {
            for(int j=i;j<count;j++)
            {
                if(filetime[i]<filetime[j])
                {
                    __time_t t=filetime[i];
                    filetime[i]=filetime[j];
                    filetime[j]=t;
                    char*temp;
                    strcpy(temp,filename[i]);
                    strcpy(filename[i],filename[j]);
                    strcpy(filename[j],temp);
                }
            }
        }
    }
    else
    {
        for(int i=0;i<count;i++)
		{
			for(int j=i;j<count;j++)
			{
				if(strcmp(filename[i],filename[j])>0)
				{
                    char*temp;
					strcpy(temp,filename[i]);
					strcpy(filename[i],filename[j]);
					strcpy(filename[j],temp);
				}
			}
		}
    }
    if(flag_param&PARAM_r)
    {
        for(int i=0;i<count/2;i++)
        {
            char*temp;
            strcpy(temp,filename[i]);
            strcpy(filename[i],filename[count-1-i]);
            strcpy(filename[count-1-i],temp);
        }
    }
}

void display(int flag_param,char*path)
{

}