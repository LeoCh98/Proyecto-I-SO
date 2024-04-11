#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>

#define MiB (1024 * 1024)
#define GiB (MiB * 1024)

int main(int argc, char *argv[])
{
    char *path = argv[1];
    char *option = argv[2];

    struct statvfs stat;
    if (statvfs(path, &stat) != 0)
    {
        perror("Error getting information on this filesystem..");
        return 1;
    }

    unsigned long long fs_block_size = stat.f_frsize;

    unsigned long long total = (unsigned long long)stat.f_blocks * fs_block_size;
    unsigned long long available = (unsigned long long)stat.f_bfree * fs_block_size;
    unsigned long long used = total - available;

    if (argc == 2)
    {
        printf("Summary disk:\n");
        printf("Disk total: %.2f GiB\n", (double)total / GiB);
        printf("Disk usage: %.2f GiB\n", (double)used / GiB);
        printf("Disk available: %.2f GiB\n", (double)available / GiB);
    }

    if (strcmp(option, "-tm") == 0)
    {
        printf("Disk usage: %.3f MiB\n", (double)used / MiB);
        printf("Disk available: %.3f MiB\n", (double)available / MiB);
    }
    if (strcmp(option, "-tg") == 0)
    {
        printf("Disk usage: %.2f GiB\n", (double)used / GiB);
        printf("Disk available: %.2f GiB\n", (double)available / GiB);
    }

    return 0;
}