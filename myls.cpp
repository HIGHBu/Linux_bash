#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>

/* Options控制 */
#define OP_a (1L << 0) // 列出包括隐含文件在内的所有文件
#define OP_i (1L << 1) // 显示inode
#define OP_l (1L << 2) // 长格式显示

#define MAX_FILE_ONCE 128  // 一次最多有128个file参数
#define MAX_FILE_NAME 1024 // 每个file名称最长为1024
#define MAX_FILE_NUM 1024  // 假设一个文件夹最多1024个文件

void order(char (*file)[MAX_FILE_NAME], int filenum);
void list_file(char (*param)[MAX_FILE_NAME], int *file, int filenumn, int op);
void list_dir(char (*param)[MAX_FILE_NAME], int *dir, int dirnum);
void list_content_dir(char *dirname, char *path);
int list_single(char *name);
void list_attr(struct stat fstat);
void info(char (*filename)[MAX_FILE_NAME], int filenum);

/* 控制用的参数 */
int options = 0;    // 用户输入的参数
int longformat = 6; // 长格式控制
char cwd[MAX_FILE_NAME];
int long_flag = 0;

int main(int argc, char **argv)
{
    int paramnumber = 0, filenum = 0, dirnum = 0;
    char param[MAX_FILE_ONCE][MAX_FILE_NAME]; // 输入的文件/目录
    int file[MAX_FILE_ONCE] = {0};
    int dir[MAX_FILE_ONCE] = {0};

    struct stat fstat;
    if (argc > MAX_FILE_ONCE)
    {
        printf("Error: Too many file to print once!\n");
        return 1;
    }

    for (int i = 1; i < argc; i++)
    { // 顺序扫描输入的参数
        if (argv[i][0] == '-')
        { // 输入的是Option
            for (int j = 1; j < strlen(argv[i]); j++)
            {
                switch (argv[i][j])
                {
                case 'a':
                    options |= OP_a;
                    break;
                case 'i':
                    options |= OP_i;
                    break;
                case 'l':
                    options |= OP_l;
                    long_flag = 1;
                    break;
                default:
                    printf("Error: Unknown option!\n");
                    return 1;
                }
            }
        }
        else
        { // 输入的是文件/路径
            if (strlen(argv[i]) >= MAX_FILE_NAME)
            {
                printf("Error: Path too long!\n");
                return 1;
            }
            strcpy(param[paramnumber++], argv[i]);
        }
    }

    if (paramnumber == 0)
    { // 没有文件输入，默认为当前目录
        strcpy(param[paramnumber++], "./");
    }

    /* 遍历所有param并判断是文件还是目录 */
    for (int i = 0; i < paramnumber; i++)
    {
        if (lstat(param[i], &fstat) == -1)
        { // 文件或目录不存在
            printf("myls: cannot access '%s': No such file or directory\n", param[i]);
            return 1;
        }
        // 分别处理文件和目录
        if (S_ISDIR(fstat.st_mode))
            dir[dirnum++] = i;
        else
            file[filenum++] = i;
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        printf("Error: CWD too long!\n");
        return 1;
    }

    /* 处理文件类型的输出 */
    list_file(param, file, filenum, 0);

    /* 处理目录类型的输出 */
    list_dir(param, dir, dirnum);
}

void order(char (*file)[MAX_FILE_NAME], int filenum)
{
    char tmp[MAX_FILE_NAME];
    int i, j;
    for (i = 1; i < filenum; i++)
    {
        strcpy(tmp, file[i]);
        for (j = i; j > 0 && strcmp(file[j - 1], tmp) > 0; j--)
            strcpy(file[j], file[j - 1]);
        strcpy(file[j], tmp);
    }
}

//按顺序列出文件
void list_file(char (*param)[MAX_FILE_NAME], int *file, int filenum, int op)
{
    if (filenum <= 0)
    {
        return;
    }
    char filename[filenum][MAX_FILE_NAME];
    if (!op)
    { // 由main调用，需要参数 file
        for (int i = 0; i < filenum; i++)
        {
            strcpy(filename[i], param[file[i]]);
        }
    }
    else
    { // 由list_dir调用，不需要参数 file
        for (int i = 0; i < filenum; i++)
        {
            strcpy(filename[i], param[i]);
        }
    }

    if (op)
    {
        /* 统计块数*/
        struct stat fstat;
        int totalblk = 0;
        for (int i = 0; i < filenum; i++)
        {
            lstat(filename[i], &fstat);

            /* 不统计不需要输出文件的大小 */
            if (filename[i][0] == '.')
            {
                if (!(options & OP_a))
                {
                    continue;
                }
            }
            totalblk += fstat.st_blocks;
        }

        if (long_flag)
            printf("total:  %-6d\n", totalblk);
    }
    order(filename, filenum); // 字典序
    info(filename, filenum);  // 进行输出
    putchar('\n');
}

//按顺序列出目录内的内容
void list_dir(char (*param)[MAX_FILE_NAME], int *dir, int dirnum)
{
    if (dirnum <= 0)
    {
        return;
    }
    for (int i = 0; i < dirnum; i++)
    {
        list_content_dir(param[dir[i]], "");
        chdir(cwd);
    }
}

void list_content_dir(char *dirname, char *path)
{
    DIR *dp;
    struct dirent *entry;
    char filename[MAX_FILE_NUM][MAX_FILE_NAME], _path[MAX_FILE_NAME];
    int dirRev[MAX_FILE_NUM];
    int filenum = 0;

    if ((dp = opendir(dirname)) == NULL)
    {
        printf("myls: cannot access '%s': No such file or directory\n", dirname);
        return;
    }

    strcpy(_path, path);
    chdir(dirname);

    /* 读取文件夹中的目录项 */
    while ((entry = readdir(dp)) != NULL)
    {
        strcpy(filename[filenum], entry->d_name);
        filenum += 1;
    }
    closedir(dp);
    printf("%s:\n", dirname);
    list_file(filename, NULL, filenum, 1);
    putchar('\n');
}

/**
 * @brief 列出一个单项
 *
 * @param name 该项的名称
 * @return int 0表示正常输出，1表示未输出
 */
int list_single(char *name)
{
    struct stat fstat;
    /* 跳过不输出的文件 */
    if (name[0] == '.')
    {
        if (!(options & OP_a))
        {
            return 1;
        }
    }
    lstat(name, &fstat);
    /* 输出inode */
    if (options & OP_i)
        printf("%8llu ", fstat.st_ino);

    /* 是否长格式 */
    if (long_flag)
    {
        list_attr(fstat);
    }
    if (S_ISDIR(fstat.st_mode))
    { //目录
        printf("%s", name);
    }
    else if (S_ISLNK(fstat.st_mode))
    { //符号链接
        printf("%s", name);
    }
    else if (S_ISSOCK(fstat.st_mode))
    { // socket文件
        printf("%s", name);
    }
    else if (S_ISFIFO(fstat.st_mode))
    { // 管道文件
        printf("%s", name);
    }
    else if (fstat.st_mode & S_IXUSR)
    { // 一般可执行文件
        printf("%s", name);
    }
    else
    {
        printf("%s", name);
    }
    return 0;
}

/**
 * @brief 列出文件的长格式（不输出名称，只输出属性）
 */
void list_attr(struct stat fstat)
{
    char time[30];      //时间信息
    /* 打印文件类型 */
    if (S_ISDIR(fstat.st_mode)) //目录
        printf("d");
    else if (S_ISLNK(fstat.st_mode)) //符号链接
        printf("l");
    else if (S_ISREG(fstat.st_mode)) //普通文件
        printf("-");
    else if (S_ISCHR(fstat.st_mode)) //字符设备
        printf("c");
    else if (S_ISBLK(fstat.st_mode)) //块设备
        printf("b");
    else if (S_ISFIFO(fstat.st_mode)) //管道文件
        printf("f");
    else if (S_ISSOCK(fstat.st_mode)) // socket文件
        printf("s");

    /* 打印user权限 */
    putchar(fstat.st_mode & S_IRUSR ? 'r' : '-');
    putchar(fstat.st_mode & S_IWUSR ? 'w' : '-');
    putchar(fstat.st_mode & S_IXUSR ? 'x' : '-');
    /* 打印group权限 */
    putchar(fstat.st_mode & S_IRGRP ? 'r' : '-');
    putchar(fstat.st_mode & S_IWGRP ? 'w' : '-');
    putchar(fstat.st_mode & S_IXGRP ? 'x' : '-');
    /* 打印other权限 */
    putchar(fstat.st_mode & S_IROTH ? 'r' : '-');
    putchar(fstat.st_mode & S_IWOTH ? 'w' : '-');
    putchar(fstat.st_mode & S_IXOTH ? 'x' : '-');
    putchar(' ');

    /* 打印链接数 */
    printf("%2d", fstat.st_nlink);
    putchar(' ');

    /* 打印文件的大小 */
    printf("%6lld", fstat.st_size);
    putchar(' ');

    /* 打印时间 */
    strcpy(time, ctime(&fstat.st_mtime)); //默认mtime

    time[strlen(time) - 1] = '\0'; //去掉'\n'
    printf(" %s ", time + 4);
}

//用于输出的总函数
void info(char (*filename)[MAX_FILE_NAME], int filenum)
{
    struct stat fstat;
    int totalblk = 0;

    /* 逐一控制输出 */
    for (int i = 0; i < filenum; i++)
    {
        if (!list_single(filename[i]))
        {
            /* 结尾分割的控制 */
            if (i == filenum - 1)
                continue; // 最后一个跳过即可
            if (long_flag)
                printf("\n");
            else
                printf(" ");
        }
    }
}