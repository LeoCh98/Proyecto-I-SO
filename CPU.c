#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 1024

char* getPercentByProcess(int pid)
{
    char statPath[256];
    snprintf(statPath, sizeof(statPath), "/proc/%d/stat", pid);
    FILE *file = fopen(statPath, "r");

    char processName[256];
    fscanf(file, "%*d (%[^)])", processName);
    fclose(file);

    unsigned long long process_utime, process_stime, process_starttime;
    FILE *stat_file = fopen(statPath, "r");
    fscanf(stat_file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %llu %llu %*d %*d %*d %*d %*d %*d %llu",
           &process_utime, &process_stime, &process_starttime);
    fclose(stat_file);

    FILE *uptime_file = fopen("/proc/uptime", "r");

    double system_uptime_sec;
    fscanf(uptime_file, "%lf", &system_uptime_sec);
    fclose(uptime_file);

    long clk_tck = sysconf(_SC_CLK_TCK);

    double process_utime_sec = process_utime / (double)clk_tck;
    double process_stime_sec = process_stime / (double)clk_tck;
    double process_starttime_sec = process_starttime / (double)clk_tck;
    double process_elapsed_sec = system_uptime_sec - process_starttime_sec;
    double process_usage_sec = process_utime_sec + process_stime_sec;
    double process_usage = (process_usage_sec * 100) / process_elapsed_sec;

    char* information;
    information = (char*)malloc(1024);
    
    snprintf(information, 1024, "%s's PID:%d total CPU usage is: %.2f%%\n", processName, pid, process_usage);

    return information;
}

double getTotalPercent()
{
    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL)
    {
        printf("Failed to open /proc directory\n");
        return -1;
    }

    double total_cpu_usage = 0.0;
    struct dirent *entry;
    while ((entry = readdir(proc_dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            int pid = atoi(entry->d_name);
            if (pid != 0)
            {
                char stat_path[256];
                snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);
                FILE *stat_file = fopen(stat_path, "r");
                if (stat_file != NULL)
                {
                    unsigned long long process_utime, process_stime, process_starttime;
                    fscanf(stat_file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %llu %llu %*d %*d %*d %*d %*d %*d %llu",
                           &process_utime, &process_stime, &process_starttime);
                    fclose(stat_file);

                    FILE *uptime_file = fopen("/proc/uptime", "r");
                    if (uptime_file != NULL)
                    {
                        double system_uptime_sec;
                        fscanf(uptime_file, "%lf", &system_uptime_sec);
                        fclose(uptime_file);

                        long clk_tck = sysconf(_SC_CLK_TCK);

                        double process_utime_sec = process_utime / (double)clk_tck;
                        double process_stime_sec = process_stime / (double)clk_tck;
                        double process_starttime_sec = process_starttime / (double)clk_tck;
                        double process_elapsed_sec = system_uptime_sec - process_starttime_sec;
                        double process_usage_sec = process_utime_sec + process_stime_sec;
                        total_cpu_usage += (process_usage_sec * 100) / process_elapsed_sec;
                    }
                }
            }
        }
    }

    closedir(proc_dir);

    return total_cpu_usage;
}

int main(int argc, char *argv[])
{
    int fd;

    char* information;
    information = (char*)malloc(1024);

    fd = open(argv[1], O_CREAT | O_WRONLY, 0666);

    if(fd == -1){
        perror("Error al crear el archivo temporal.");
        exit(1);
    }

    // GENERACIÓN DE LA INFORMACIÓN

    if (argc == 3 && strcmp(argv[2], "cpu") == 0)
    {
        snprintf(information, MAX_LINE_LENGTH, "The total CPU usage: %.2f%%\n", getTotalPercent());
    }
    else if (argc == 4)
    {
        int pid = atoi(argv[3]);
        information = getPercentByProcess(pid);
    }

    // ESCRITURA EN EL PIPE

    write(fd, information, MAX_LINE_LENGTH);

    close(fd);

    return 0;
}