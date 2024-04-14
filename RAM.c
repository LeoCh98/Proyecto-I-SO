#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_LINE_LENGTH 256

float getUsageReal(int pid)
{
    char path[MAX_LINE_LENGTH];
    FILE *file;
    long rss;
    snprintf(path, sizeof(path), "/proc/%d/statm", pid);
    file = fopen(path, "r");
    if (!file)
    {
        perror("Error getting file..");
        return -1.0;
    }

    if (fscanf(file, "%*s %ld", &rss) != 1)
    {
        perror("Error reading file..");
        fclose(file);
        return -1.0;
    }
    fclose(file);

    long page_size = sysconf(_SC_PAGESIZE);

    float memory_usage = (rss * page_size) / (float)1024;
    return memory_usage;
}

float getUsageVirtual(int pid)
{
    char path[MAX_LINE_LENGTH];
    FILE *file;
    long vmsize;
    snprintf(path, sizeof(path), "/proc/%d/statm", pid);
    file = fopen(path, "r");
    if (!file)
    {
        perror("Error getting file..");
        return -1.0;
    }

    if (fscanf(file, "%ld", &vmsize) != 1)
    {
        perror("Error reading file..");
        fclose(file);
        return -1.0;
    }
    fclose(file);

    float memory_usage = vmsize / (float)1024;
    return memory_usage;
}

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "memoria") == 0 && strcmp(argv[2], "-r") == 0)
    {
        printf("PID\tPercentage of real memory used\n");
        FILE *fp = fopen("/proc/meminfo", "r");
        if (fp == NULL)
        {
            perror("Failed to open /proc/meminfo directory");
            return 1;
        }
        char line[MAX_LINE_LENGTH];
        long total_memory;
        while (fgets(line, sizeof(line), fp))
        {
            if (sscanf(line, "MemTotal: %ld kB", &total_memory) == 1)
            {
                break;
            }
        }
        fclose(fp);

        DIR *dir;
        struct dirent *entry;
        dir = opendir("/proc");
        if (!dir)
        {
            perror("Failed to open /proc directory");
            return 1;
        }
        while ((entry = readdir(dir)) != NULL)
        {
            int pid = atoi(entry->d_name);
            if (pid != 0)
            {
                float memory_usage = getUsageReal(pid);
                if (memory_usage != -1.0)
                {
                    printf("%d\t%.1f%%\n", pid, (memory_usage / total_memory) * 100);
                }
            }
        }
        closedir(dir);
    }
    else if (strcmp(argv[1], "memoria") == 0 && strcmp(argv[2], "-v") == 0)
    {
        int pid = atoi(argv[3]);
        float memory_usage = getUsageVirtual(pid);
        if (memory_usage != -1.0)
        {
            printf("PID: %d, Percentage of virtual memory used: %.1f%%\n", pid, memory_usage);
        }
        else
        {
            printf("Error getting process information with PID: %d\n", pid);
            return 1;
        }
    }
    return 0;
}