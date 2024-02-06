#include<unistd.h>//getcwd和chdir的头文件
#include<dirent.h>//opendir,readdir和closedir的头文件
#include<sys/stat.h>//stat的头文件
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
int main(int argc,char*argv[])
{
    char*list=(char*)malloc(sizeof(char)*100);
    getcwd(list,100);
    DIR*dir=opendir(list);
    struct dirent*ptr;
    while((ptr=readdir(dir))!=NULL)
    {
        printf("%s ",ptr->d_name);
    }
    closedir(dir);
    free(list);
}