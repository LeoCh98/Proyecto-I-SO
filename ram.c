#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define BUFFER 256

float getUsageReal(int pid)
{
    char path[BUFFER];
    FILE *file;
    long rss;
    snprintf(path, sizeof(path), "/proc/%d/statm", pid);
    file = fopen(path, "r");
    if (!file)
    {
        perror("Error al obtener el archivo.");
        return -1.0;
    }

    // ------------------
    
    char statPath[BUFFER];
    snprintf(statPath, sizeof(statPath), "/proc/%d/stat", pid);
    FILE *fileN = fopen(statPath, "r");

    if(fileN==NULL){
        return -1;
    }

    char processName[BUFFER];
    fscanf(fileN, "%*d (%[^)])", processName);
    fclose(fileN);

    printf("%s\t\t\t\t", processName);

    // ------------------

    if (fscanf(file, "%*s %ld", &rss) != 1)
    {
        perror("Error al leer el archivo.");
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
    char path[BUFFER];
    FILE *file;
    long vmsize;
    snprintf(path, sizeof(path), "/proc/%d/statm", pid);
    file = fopen(path, "r");
    if (!file)
    {
        return -1.0;
    }

    if (fscanf(file, "%ld", &vmsize) != 1)
    {
        fclose(file);
        return -1.0;
    }
    fclose(file);

    // ------------------
    
    char statPath[BUFFER];
    snprintf(statPath, sizeof(statPath), "/proc/%d/stat", pid);
    FILE *fileN = fopen(statPath, "r");

    if(fileN==NULL){
        return -1;
    }

    char processName[BUFFER];
    fscanf(fileN, "%*d (%[^)])", processName);
    fclose(fileN);

    printf("%s ", processName);

    // ------------------

    float memory_usage = vmsize / (float)1024;
    return memory_usage;
}

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        printf("Error. No se ha especificado algún tipo de memoria (memoria <-r> / <-v> <PID>).\n");
        return 0;
    }

    if (strcmp(argv[1], "memoria") == 0 && strcmp(argv[2], "-r") == 0)
    {
        printf("Nombre\t\t\t\tPID\t\tPorcentaje de memoria real\n");
        FILE *fp = fopen("/proc/meminfo", "r");
        if (fp == NULL)
        {
            perror("Falló al abrir el directorio /proc/meminfo.");
            return 1;
        }
        char line[BUFFER];
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
            perror("Fallo an intentar abrir el directorio /proc.\n");
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
                    printf("%d\t\t%.1f%%\n", pid, (memory_usage / total_memory));
                }
            }
        }
        closedir(dir);
    }
    else if (strcmp(argv[1], "memoria") == 0 && strcmp(argv[2], "-v") == 0)
    {
        if(argc == 4){
            int pid = atoi(argv[3]);
            float memory_usage = getUsageVirtual(pid);
            if (memory_usage != -1.0)
            {
                printf("[PID: %d], su porcentaje de uso de memoria virtual es: %.1f%%\n", pid, memory_usage/72);
            }
            else
            {
                printf("Error. No se ha encontrado ningún proceso con PID: %d.\n", pid);
            }
        } else {
            // Acá tendría que ir para imprimir todos los procesos con su respectivo uso de memoria virtual.
            printf("Error. No se ha ingresado ningún PID (-v <PID>).\n");
        }
        
    }
    return 0;
}