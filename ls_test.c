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

#define PARAM_a    1              //参数a ok
#define PARAM_l    2              //参数l ok
#define PARAM_i    4              //参数i ok
#define PARAM_r    8              //参数r ok
#define PARAM_t    16             //参数t ok
#define PARAM_R    32             //参数R
#define PARAM_s    64             //参数s ok

void read_dir(int flag_param,const char *path,char**filename,int*count);
void arr_file(int flag_param,char*path,char**filename,int*count);
void file_content(int flag_param,char*path,char**filename,int count);

int main(int argc,char*argv[])
{
    int i,j,k,num,count;//num为输入参数的数量，count为文件名的数量
    char*path=(char*)malloc(sizeof(char)*100);//路径
    char*filename[100]=(char**)malloc(sizeof(char*)*100);//文件名
    char param[8];//输入的参数
    int flag_param=0;
    struct stat*buf;

    num=0;
    j=0;
    //读取输入的参数
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
    //用特殊方法将输入的参数融入flag_param
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
    //判断参数是否含路径
    if(num+1==argc)//不含则默认当前目录路径
    {
        getcwd(path,100);
        arr_file(flag_param,path,filename,&count);
        file_content(flag_param,path,filename,count);
        free(path);
        free(filename);
		return 0;
    }
    else
    {
        strcpy(path,argv[num+1]);
        stat(path,buf);
        //判断是否为目录
        if(S_ISDIR(buf->st_mode))
        {
            arr_file(flag_param,path,filename,&count);
            file_content(flag_param,path,filename,count);
        }
        else
        {
            display(flag_param,path);
        }
        free(path);
        free(filename);
        return 0;
    }
}
void read_dir(int flag_param,const char *path,char**filename,int*count)//读取目录，并判断了-a参数
{
    DIR*dir;
    struct dirent*ptr=opendir(path);
    int i=0;
    if(flag_param&PARAM_a)
    {
        while(ptr!=NULL)
        {
            *filename[i]=ptr->d_name;
            i++;
        }
        closedir(dir);
        *count=i;
    }
    else
    {
        while(ptr!=NULL)
        {
            if(ptr->d_name[0]!='.')
            {
                *filename[i]=ptr->d_name;
                i++;
            }
        }
        closedir(dir);
        *count=i;
    }
}
void arr_file(int flag_param,char*path,char**filename,int*count)
{
    struct stat *buf;
    __time_t filetime[100]=(__time_t*)malloc(sizeof(__time_t)*100);//文件最后修改时间
    read_dir(flag_param,path,filename,count);
    if(flag_param&PARAM_t)
    {
        for(int i=0;i<*count;i++)
        {
            stat(filename[i],buf);
            filetime[i]=buf->st_mtime;
        }
        for(int i=0;i<*count;i++)
        {
            for(int j=i;j<*count;j++)
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
        for(int i=0;i<*count;i++)
		{
			for(int j=i;j<*count;j++)
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
        for(int i=0;i<*count/2;i++)
        {
            char*temp;
            strcpy(temp,filename[i]);
            strcpy(filename[i],filename[*count-1-i]);
            strcpy(filename[*count-1-i],temp);
        }
    }
}
void file_content(int flag_param,char*path,char**filename,int count)
{
    char colorname[NAME_MAX + 30];
    struct stat*buf;
    char buf_time[32];
	struct passwd *psd;
	struct group *grp;
    
    if(flag_param&PARAM_l)
    {
        for(int i=0;i<count;i++)
        {
            stat(filename[i],buf);
            //索引信息
            if(flag_param&PARAM_i)
            {
                printf("%d ",buf->st_ino);
            }
            //文件大小
            if(flag_param&PARAM_s)
            {
                int total=0;
                if(flag_param&PARAM_a)
                {
                    for(i=0;i<count;i++)
                    {
                        stat(filename[i],buf);
                        total=total+buf->st_blocks/2;
                    }
                }     
                else
                {
                    for(i=0;i<count;i++)
                    {
                        stat(filename[i],buf);
                        if(filename[i][2]!='.')
                        {
                            total=total+buf->st_blocks/2;
                        }
                    }
                }
                printf("%10d ",total);
            }
            //文件类型
            if(S_ISLNK(buf->st_mode)){
		        printf("l");
	        } else if(S_ISREG(buf->st_mode)){
		        printf("-");
	        } else if(S_ISDIR(buf->st_mode)){
		        printf("d");
	        } else if(S_ISCHR(buf->st_mode)){
		        printf("c");
	        } else if(S_ISBLK(buf->st_mode)){
		        printf("b");
	        } else if(S_ISFIFO(buf->st_mode)){
		        printf("f");
	        } else if(S_ISSOCK(buf->st_mode)){
	        	printf("s");
	        }
            //拥有者权限
            if(buf->st_mode & S_IRUSR)   
		        printf("r");
	        else 
	        	printf("-");
	        if(buf->st_mode & S_IWUSR)
	        	printf("w");
	        else 
	        	printf("-");
        
	        if(buf->st_mode & S_IXUSR)
	        	printf("x");
	        else
		        printf("-");
            //组权限
            if(buf->st_mode & S_IRGRP)
	        	printf("r");
	        else 
	        	printf("-");

            if(buf->st_mode & S_IWGRP)
	        	printf("w");
	        else 
	        	printf("-");
	
	        if(buf->st_mode & S_IXGRP)
	        	printf("x");
            else 
		        printf("-");
            //其他用户权限
            if(buf->st_mode & S_IROTH)
		        printf("r");
	        else 
	        	printf("-");
    
            if(buf->st_mode & S_IWOTH)
	        	printf("w");
	        else 
	        	printf("-");
	
	        if(buf->st_mode & S_IXOTH)
	        	printf("x");
	        else 
	        	printf("-");

            printf("\t");
            //通过用户和组id得到用户的信息和其所在组的信息
            psd = getpwuid(buf->st_uid);
	        grp = getgrgid(buf->st_gid);
 
	        printf("%4d ",buf->st_nlink);    //打印文件的硬链接数
	        printf("%-8s",psd->pw_name);    //打印用户的名字
	        printf("%-8s", grp->gr_name);   //打印用户组的名字

	        printf("%6d", buf->st_size);     //打印文件大小
	        strcpy(buf_time,ctime(buf->st_mtime));//把时间转换成普通表示格式
 
	        buf_time[strlen(buf_time)-1]='\0';    //去掉换行符
	        printf("  %s", buf_time);//输出时间 
	        printf(" %s\n", filename[i]);
        }
    }
    else
    {
        for(int i=0;i<count;i++)
        {
            printf("%s  ",filename[i]);
        }
    }
}