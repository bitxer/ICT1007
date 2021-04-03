#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

const char* file_type(int mode)
{
    //Check type of file using internal functions from stat.h
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
    //Doesn't match any known file type, return unknown
    return "UNKNOWN";
}

void printPermission(int mode)
{
    //Check the permission bits and print out the appropriate permissions
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
    //Resolve UID/GID to actual names using pwd.h(user) or grp.h(group)
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
        //Shouldn't ever reach here
        return "Unavailable";
    }
}


void printFileProperties(struct stat stats)
{
    //Takes a stat object and prints out all the details in it
    struct tm dt;


    printf("File Type: %s\n", file_type(stats.st_mode)); //FILE TYPE
    printf("File Size: %lu\n", stats.st_size);//SIZE

    printf("Owner Name: %s\n", getName(stats.st_uid,1));//USER ID
    printf("Group Name: %s\n", getName(stats.st_gid,2));// GROUP ID

    printf("Permission string: "); //PERMISSIONS in UNIX FORMAT
    printPermission(stats.st_mode);

    printf("i-node Number: %lu\n", stats.st_ino); //INODE No
    printf("Device ID: %lu\n", stats.st_dev); //Device Number
    printf("Number of links: %lu\n", stats.st_nlink); //Number of links

    dt = *(localtime(&stats.st_atime)); //Take last access time and format is as a localtime object (Follows your system GMT)
    printf("Last access on: %02d-%02d-%d %02d:%02d:%02d\n", dt.tm_mday, dt.tm_mon+1, dt.tm_year + 1900, dt.tm_hour, dt.tm_min, dt.tm_sec);

    dt = *(localtime(&stats.st_mtime));//See above but for last modified time
    printf("Last modification on: %02d-%02d-%d %02d:%02d:%02d\n", dt.tm_mday, dt.tm_mon+1, dt.tm_year + 1900, dt.tm_hour, dt.tm_min, dt.tm_sec);

    dt = *(localtime(&stats.st_ctime));//See above but for last status change
    printf("Last status change on: %02d-%02d-%d %02d:%02d:%02d\n", dt.tm_mday, dt.tm_mon+1, dt.tm_year + 1900, dt.tm_hour, dt.tm_min, dt.tm_sec);
    printf("==============================\n");

}

int main(int argc, char *argv[])
{
    //Check for any arguments
    if (argc >= 2)
    {
        //Help flag, if -h is an argument, return the help UI
        if (strcmp((argv[1]),"-h") == 0)
        {
            printf("Usage: ./Q2.c returns details of files in the current directory\n");
            printf("./Q2.c [FILE] [FILE] [FILE] ... [FILE] returns details of files specified\n");
        }
        else
        {
            //parse through the arguments
            for (int i = 1; i < argc; i++)
            {
                struct stat stats;

                //Get the stat object using the stat function
                if (stat(argv[i], &stats) == 0)
                {
                    printf("File Name: %s\n", argv[i]);
                    //Pass the stat object to this function to print out the details
                    printFileProperties(stats);
                }
                else
                {
                    //Stat function failed
                    printf("Unable to read file %s. \n", argv[i]);
                    printf("=============================\n");
                }
            }
        }
    }
    else
    {
        //No argument, check current directory
        DIR *d;
        struct dirent *dir;

        //Open current directory
        d = opendir(".");
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                //Only process actual files NOT . and ..
                if (strcmp ((dir->d_name),".") != 0 && strcmp((dir->d_name),"..") != 0)
                {
                    struct stat stats;
                    //Check if the file is an actual file
                    if (stat(dir->d_name, &stats) == 0)
                    {
                        printf("File Name: %s\n", dir->d_name);
                        //Pass the stat object to this function to print out the details
                        printFileProperties(stats);
                    }
                    else
                    {
                        //Isn't an actual file
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
