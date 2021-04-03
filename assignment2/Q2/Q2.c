#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

const char* file_type(int mode)
{
    if (S_ISREG(mode))
    {
        return "Regular File";
    }
    else if (S_ISDIR(mode))
    {
        return "Directory";
    }
    else if (S_ISCHR(mode))
    {
        return "Character Device";
    }
    else if (S_ISBLK(mode))
    {
        return "Block Device";
    }
    else if (S_ISFIFO(mode))
    {
        return "FIFO (named pipe)";
    }
    else if (S_ISLNK(mode))
    {
        return "Symbolic Link";
    }
    else if (S_ISSOCK(mode))
    {
        return "Socket";
    }
    return "UNKNOWN";
}

void printPermission(int mode)
{
    printf("%s",(S_ISDIR(mode) ? "d" : "-"));
    printf("%s",(mode & S_IRUSR) ? "r" : "-");
    printf("%s",(mode & S_IWUSR) ? "w" : "-");
    printf("%s",(mode & S_IXUSR) ? "x" : "-");
    printf("%s",(mode & S_IRGRP) ? "r" : "-");
    printf("%s",(mode & S_IWGRP) ? "w" : "-");
    printf("%s",(mode & S_IXGRP) ? "x" : "-");
    printf("%s",(mode & S_IROTH) ? "r" : "-");
    printf("%s",(mode & S_IWOTH) ? "w" : "-");
    printf("%s",(mode & S_IXOTH) ? "x" : "-");
    printf("\n");
}

const char* getName(int uid, int mode)
{
    struct passwd *pws;
    struct group *grp;
    //Mode 1 for User, Mode 2 for Group
    if (mode == 1)
    {
        pws = getpwuid(uid);
        return pws->pw_name;
    }
    else if (mode == 2)
    {
        grp = getgrgid(uid);
        return grp->gr_name;
    }
    else
    {
        return "Unavailable";
    }
}


void printFileProperties(struct stat stats)
{
    struct tm dt;


    printf("File Type: %s\n", file_type(stats.st_mode)); //FILE TYPE
    //PERMISSIONS
    printf("File Size: %lu\n", stats.st_size);//SIZE

    printf("Owner Name: %s\n", getName(stats.st_uid,1));
    printf("Group Name: %s\n", getName(stats.st_gid,2));

    printf("Permission string: ");
    printPermission(stats.st_mode);

    printf("i-node Number: %lu\n", stats.st_ino); //INODE No
    printf("Device ID: %lu\n", stats.st_dev); //Device Number
    printf("Number of links: %lu\n", stats.st_nlink); //Number of links

    dt = *(localtime(&stats.st_atime));
    printf("Last access on: %02d-%02d-%d %02d:%02d:%02d\n", dt.tm_mday, dt.tm_mon+1, dt.tm_year + 1900, dt.tm_hour, dt.tm_min, dt.tm_sec);

    dt = *(localtime(&stats.st_mtime));
    printf("Last modification on: %02d-%02d-%d %02d:%02d:%02d\n", dt.tm_mday, dt.tm_mon+1, dt.tm_year + 1900, dt.tm_hour, dt.tm_min, dt.tm_sec);

    dt = *(localtime(&stats.st_ctime));
    printf("Last status change on: %02d-%02d-%d %02d:%02d:%02d\n", dt.tm_mday, dt.tm_mon+1, dt.tm_year + 1900, dt.tm_hour, dt.tm_min, dt.tm_sec);
    printf("==============================\n");

}

int main(int argc, char *argv[])
{
    if (argc >= 2)
    {
        if (strcmp((argv[1]),"-h") == 0)
        {
            printf("Usage: ./Q2.c returns details of files in the current directory\n");
            printf("./Q2.c [FILE] [FILE] [FILE] ... [FILE] returns details of files specified\n");
        }
        else
        {
            for (int i = 1; i < argc; i++)
            {
                struct stat stats;

                if (stat(argv[i], &stats) == 0)
                {
                    printf("File Name: %s\n", argv[i]);
                    printFileProperties(stats);
                }
                else
                {
                    printf("Unable to read file %s. \n", argv[i]);
                    printf("=============================\n");
                }
            }
        }
        //THERE is an argument, check these files
    }
    else
    {
        //No argument, check current directory
        DIR *d;
        struct dirent *dir;

        d = opendir(".");
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                if (strcmp ((dir->d_name),".") != 0 && strcmp((dir->d_name),"..") != 0)
                {
                    struct stat stats;

                    if (stat(dir->d_name, &stats) == 0)
                    {
                        printf("File Name: %s\n", dir->d_name);
                        printFileProperties(stats);
                    }
                    else
                    {
                        printf("Unable to read file. \n");
                        printf("=============================\n");
                    }
                }
            }
            closedir(d);
        }
    }
    return (0);
}
