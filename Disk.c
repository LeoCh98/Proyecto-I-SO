#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#define BUFFER 1024
#define MiB (1024 * 1024)
#define GiB (MiB * 1024)

int main(int argc, char *argv[])
{
    int fd;

    char* information;
    information = (char*)malloc(1024);

    printf("%s, %s, %s, %s, %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);

    fd = open(argv[1], O_CREAT | O_WRONLY, 0666);

    if(fd == -1){
        perror("Error al crear el archivo temporal.");
        exit(1);
    }

    char *path = argv[2];
    char *option = argv[3];

    struct statvfs stat;
    if (statvfs(path, &stat) != 0)
    {
        perror("Error getting information on this filesystem..");
        return 0;
    }

    unsigned long long fs_block_size = stat.f_frsize;

    unsigned long long total = (unsigned long long)stat.f_blocks * fs_block_size;
    unsigned long long available = (unsigned long long)stat.f_bfree * fs_block_size;
    unsigned long long used = total - available;

    if (argc == 3)
    {
        snprintf(information, BUFFER, "Summary disk:\nDisk total: %.2f GiB\nDisk usage: %.2f GiB\nDisk available: %.2f GiB\n", 
        (double)total / GiB, (double)used / GiB, (double)available / GiB);
    } else if (argc == 4) 
    {
        if (strcmp(option, "-tm") == 0)
        {
            snprintf(information, BUFFER, "Disk usage: %.3f MiB\nDisk available: %.3f MiB\n", 
            (double)used / MiB, (double)available / MiB);
        } 
        if (strcmp(option, "-tg") == 0)
        {
            snprintf(information, BUFFER, "Disk usage: %.2f GiB\nDisk available: %.2f GiB\n", 
            (double)used / GiB, (double)available / GiB);
        }
    }

    write(fd, information, BUFFER);

    close(fd);
    
    printf("Información: %s", information);

    return 0;
}