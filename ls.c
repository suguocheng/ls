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

typedef struct
{
    char filename[100];//文件名
    struct stat fileinfo;//文件信息
}File;//存储文件名和文件信息的结构体

#define FILECOUNT_MAX 2000        //文件数最大值
#define PATHNAME_MAX  200         //路径名最大长度
#define PARAM_a       1           //参数a ok
#define PARAM_l       2           //参数l 
#define PARAM_i       4           //参数i 
#define PARAM_r       8           //参数r ok
#define PARAM_t       16          //参数t ok
#define PARAM_R       32          //参数R
#define PARAM_s       64          //参数s 

void do_ls(int flag_param,char *dirname);
int main(int argc,char*argv[])//main函数主要负责读取参数并给do_ls提供参数
{
    int i=0,j=0,k=0,num=0,count=0;//i,j,k为循环的计数,num为输入参数的数量，count为文件名的数量
    char param[7];//存放输入参数的数组
    int flag_param=0;//存放参数的数
    //从命令行读取参数
    if(argc>1)
    {
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
    }
    //用位运算方法将输入的参数存入flag_param
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
    if(num+1==argc)//不含则默认当前目录
    {
        printf("%s\n",".");
        do_ls(flag_param,".");
    }
    else//含则使用参数中的目录
    {
        for(int i=1;i<argc;i++)
        {
            if(argv[i][0]!='-')
            {
                printf("%s\n",argv[i]);
                do_ls(flag_param,argv[i]);
            }
        }
    }
}
void do_ls(int flag_param,char *dirname)//do_ls函数主要负责执行ls的功能，如判断参数，读取文件，排列文件，输出文件信息等
{
    File *file=(File*)malloc(sizeof(File)*FILECOUNT_MAX);//为file结构体数组分配空间
    DIR *dir=opendir(dirname);//打开目录
    //判断打开目录是否成功
    if(dir==NULL){
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    struct dirent *ptr=readdir(dir);//读取目录
    int filecount=0;//文件计数器
    //判断参数是否含-a，读取并存储文件名
    if(flag_param&PARAM_a)//参数含-a则包含隐藏文件
    {
        while(ptr!=NULL)
        {
            strcpy(file[filecount].filename,ptr->d_name);
            filecount++;
            ptr=readdir(dir);
        }
        closedir(dir);
    }
    else//参数不含-a则不包含隐藏文件
    {
        while(ptr!=NULL)
        {
            if(ptr->d_name[0]!='.')//判断隐藏文件
            {
                strcpy(file[filecount].filename,ptr->d_name);
                filecount++;
            }
            ptr=readdir(dir);
        }
        closedir(dir);
    }
    char *pathname[FILECOUNT_MAX];//绝对路径名
    for(int i=0;i<FILECOUNT_MAX;i++)
    {
        pathname[i]=(char*)malloc(sizeof(char)*PATHNAME_MAX);
    }
    //将文件名转换为绝对路径名
    for(int i=0;i<filecount;i++)
    {
        strcpy(pathname[i],dirname);
        strcat(pathname[i],"/");
        strcat(pathname[i],file[i].filename);    
    }
    //读取并存储文件信息
    for(int i=0;i<filecount;i++)
    {
        lstat(pathname[i],&file[i].fileinfo);
    }
    //排序
    //判断参数是否含-t
    if(flag_param&PARAM_t)//参数含-t则以文件修改时间排序
    {
        for(int i=0;i<filecount;i++)
        {
            for(int j=i;j<filecount;j++)
            {
                if(file[i].fileinfo.st_mtime<file[j].fileinfo.st_mtime)
                {
                    File temp=file[i];
                    file[i]=file[j];
                    file[j]=temp;
                }
            }
        }
    }
    else//参数不含-t则以文件名排序
    {
        for(int i=0;i<filecount;i++)
		{
			for(int j=i;j<filecount;j++)
			{
				if(strcmp(file[i].filename,file[j].filename)>0)
				{
                    File temp=file[i];
                    file[i]=file[j];
                    file[j]=temp;
				}
			}
		}
    }
    //判断参数是否含-r
    if(flag_param&PARAM_r)
    {
        for(int i=0;i<filecount/2;i++)
        {
            File temp=file[i];
            file[i]=file[filecount-1-i];
            file[filecount-1-i]=temp;
        }
    }
    //判断参数是否含-l
    if(flag_param&PARAM_l)
    {
        //输出文件总大小
        int total=0;
        //判断参数是否含-a
        if(flag_param&PARAM_a)//参数含-a则计算隐藏文件大小
        {
            for(int i=0;i<filecount;i++)
            {
                total=total+file[i].fileinfo.st_blocks/2;
            }
        }     
        else//参数不含-a则不计算隐藏文件大小
        {
            for(int i=0;i<filecount;i++)
            {
                if(file[i].filename[0]!='.')
                {
                    total=total+file[i].fileinfo.st_blocks/2;
                }
            }
        }
        //输出文件总大小
        printf("总计 %-10d\n",total);
        //输出文件信息
        char buf_time[64];
        struct passwd *psd;
	    struct group *grp;
        for(int i=0;i<filecount;i++)
        {
            //索引信息
            if(flag_param&PARAM_i)
            {
                printf("%ld ",file[i].fileinfo.st_ino);
            }
            //文件大小
            if(flag_param&PARAM_s)
            {
                printf("%7ld ",file[i].fileinfo.st_blocks/2);
            }
            //文件类型
            if(S_ISLNK(file[i].fileinfo.st_mode)){
		        printf("l");
	        } else if(S_ISREG(file[i].fileinfo.st_mode)){
		        printf("-");
	        } else if(S_ISDIR(file[i].fileinfo.st_mode)){
		        printf("d");
	        } else if(S_ISCHR(file[i].fileinfo.st_mode)){
		        printf("c");
	        } else if(S_ISBLK(file[i].fileinfo.st_mode)){
		        printf("b");
	        } else if(S_ISFIFO(file[i].fileinfo.st_mode)){
		        printf("f");
	        } else if(S_ISSOCK(file[i].fileinfo.st_mode)){
	        	printf("s");
	        }
            //拥有者权限
            if(file[i].fileinfo.st_mode&S_IRUSR)   
		        printf("r");
	        else 
	        	printf("-");
	        if(file[i].fileinfo.st_mode&S_IWUSR)
	        	printf("w");
	        else 
	        	printf("-");
        
	        if(file[i].fileinfo.st_mode&S_IXUSR)
	        	printf("x");
	        else
		        printf("-");
            //组权限
            if(file[i].fileinfo.st_mode&S_IRGRP)
	        	printf("r");
	        else 
	        	printf("-");

            if(file[i].fileinfo.st_mode&S_IWGRP)
	        	printf("w");
	        else 
	        	printf("-");
	
	        if(file[i].fileinfo.st_mode&S_IXGRP)
	        	printf("x");
            else 
		        printf("-");
            //其他用户权限
            if(file[i].fileinfo.st_mode&S_IROTH)
		        printf("r");
	        else 
	        	printf("-");
    
            if(file[i].fileinfo.st_mode&S_IWOTH)
	        	printf("w");
	        else 
	        	printf("-");
	
	        if(file[i].fileinfo.st_mode&S_IXOTH)
	        	printf("x");
	        else 
	        	printf("-");
            
            //通过用户和组id得到用户的信息和其所在组的信息
            psd=getpwuid(file[i].fileinfo.st_uid);
	        grp=getgrgid(file[i].fileinfo.st_gid);
 
	        printf("%4ld ",file[i].fileinfo.st_nlink);    //打印文件的硬链接数
	        printf("%-17s",psd->pw_name);    //打印用户的名字
	        printf("%-17s",grp->gr_name);   //打印用户组的名字

	        printf("%15ld",file[i].fileinfo.st_size);     //打印文件大小
	        strcpy(buf_time,ctime(&file[i].fileinfo.st_mtime));//把时间转换成普通表示格式
 
	        buf_time[strlen(buf_time)-1]='\0';    //去掉换行符
	        printf("  %s",buf_time);//输出时间 
	        printf(" %s\n",file[i].filename);
        }
    }
    else
    {
        //判断参数是否含-s
        if(flag_param&PARAM_s)//参数含-s则计算并输出文件大小
        {
            //输出文件总大小
            int total=0;
            //判断参数是否含-a
            if(flag_param&PARAM_a)//参数含-a则计算隐藏文件大小
            {
                for(int i=0;i<filecount;i++)
                {
                    total=total+file[i].fileinfo.st_blocks/2;
                }
            }     
            else//参数不含-a则不计算隐藏文件大小
            {
                for(int i=0;i<filecount;i++)
                {
                    if(file[i].filename[0]!='.')
                    {
                        total=total+file[i].fileinfo.st_blocks/2;
                    }
                }
            }
            printf("总计 %-10d\n",total);
        }
        for(int i=0;i<filecount;i++)
        {
            //索引信息
            if(flag_param&PARAM_i)//判断参数是否含-i
            {
                printf("%ld ",file[i].fileinfo.st_ino);
            }
            //文件大小
            if(flag_param&PARAM_s)
            {
                printf("%2ld ",file[i].fileinfo.st_blocks/2);
            }
            printf("%s ",file[i].filename);
        }
    }
    //判断参数是否含-R
    if(flag_param&PARAM_R)
    {
        for(int i=0;i<filecount;i++)
        {
            if(S_ISDIR(file[i].fileinfo.st_mode))
            {
                if(strcmp(file[i].filename,".")!=0&&strcmp(file[i].filename,"..")!=0)
                {
                    char newpathname[PATHNAME_MAX];
                    strcpy(newpathname,dirname);
                    strcat(newpathname,"/");
                    strcat(newpathname,file[i].filename);
                    printf("\n\n%s\n",newpathname);
                    do_ls(flag_param,newpathname);
                }
            }
        }
    }
    free(file);
    for(int i=0;i<FILECOUNT_MAX;i++)
    {
        free(pathname[i]);
    }
}