#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

int getPercentByProcess(int pid)
{
    char statPath[256];
    snprintf(statPath, sizeof(statPath), "/proc/%d/stat", pid);
    FILE *file = fopen(statPath, "r");

    if(file==NULL){
        return -1;
    }

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

    printf("%s [PID: %d], porcentaje total de uso de la CPU: %.2f%%\n", processName, pid, process_usage);

    return 0;
}

double getTotalPercent()
{
    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL)
    {
        printf("Ha fallado la apertura del directorio /proc\n");
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
    if (argc == 2 && strcmp(argv[1], "cpu") == 0)
    {
        printf("Porcentaje total de uso de CPU: %.2f%%\n", getTotalPercent());
    }
    else if (argc >= 3)
    {
        for(int i = 2; i < argc; i++){
            int pid = atoi(argv[i]);
            if(getPercentByProcess(pid) == -1){
                printf("No se ha encontrado información sobre un proceso con PID: %d.\n", pid);
            }
        }
    }

    return 0;
}