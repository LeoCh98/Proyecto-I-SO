#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFER 1024

int main(int argc, char *argv[]){
    int pid;

    int n;
    char buffer[BUFFER] = {0};

    if (argc < 2){

        printf("El programa ha sido ejecutado sin un recurso a solicitar.\n\n");

    } else if (strcmp(argv[1],"cpu") == 0){ // Llamado para datos de la CPU

        int pipefd_CPU[2];    
        pipe(pipefd_CPU);

        pid = fork();

        if(pid == -1){
            printf("Error al invocar al proceso hijo.");
            exit(1);
        }

        if(pid == 0){
            close(pipefd_CPU[0]);

            // Esta función se encarga de cargar todos los COUTS en el descriptor de escritura.
            dup2(pipefd_CPU[1], STDOUT_FILENO);

            char* cpu_A[] = {
                "cpu",
                argv[1], // Recurso
                argv[2], // PID #1
                argv[3], // PID #2
                argv[4], // PID #3
                argv[5], // PID #3
                argv[6], // PID #3
                NULL
            };

            execv("./cpu", cpu_A);
            printf("Error, no inicializó el proceso hijo.");
        } else {
            close(pipefd_CPU[1]);

            printf("+-----[ESTADISTICAS CPU]-----+\n");
            while((n = read(pipefd_CPU[0], buffer, sizeof(buffer))) > 0){
                printf("%s", buffer);
            }             
        }

    } else if (strcmp(argv[1],"memoria") == 0){ // Llamado para datos de la memoria RAM

        int pipefd_RAM[2];    
        pipe(pipefd_RAM);

        pid = fork();

        if(pid == -1){
            printf("Error al invocar al proceso hijo.");
            exit(1);
        }

        if(pid == 0){
            close(pipefd_RAM[0]);

            // Esta función se encarga de cargar todos los COUTS en el descriptor de escritura.
            dup2(pipefd_RAM[1], STDOUT_FILENO);

            char* memoria_A[] = {
                "ram",
                argv[1], // Recurso
                argv[2], // Parámetro
                argv[3], // PID
                NULL
            };

            execv("./ram", memoria_A);
            printf("Error, no inicializó el proceso hijo.");
            exit(1);

        } else {
            close(pipefd_RAM[1]);

            printf("+-----[ESTADISTICAS RAM]-----+\n");
            while((n = read(pipefd_RAM[0], buffer, sizeof(buffer))) > 0){
                printf("%s", buffer);
            }               
        }

    } else if (strncmp(argv[1], "/mnt", 4) == 0){ // Llamado para datos de la RAM
        int pipefd_Disk[2];    
        pipe(pipefd_Disk);

        pid = fork();

        if(pid == -1){
            printf("Error al invocar al proceso hijo.");
            exit(1);
        }

        if(pid == 0){
            close(pipefd_Disk[0]);

            // Esta función se encarga de cargar todos los COUTS en el descriptor de escritura.
            dup2(pipefd_Disk[1], STDOUT_FILENO);

            char* disk_A[] = {
                "disk", 
                argv[1], // Recurso
                argv[2], // Parámetro
                NULL
            };
            execv("./disco", disk_A);
            printf("Error, no actualizó el proceso.");
        } else {
            close(pipefd_Disk[1]);

            printf("+-----[ESTADISTICAS DISCO]-----+\n");
            while((n = read(pipefd_Disk[0], buffer, sizeof(buffer))) > 0){
                printf("%s", buffer);
            }                         
        }

    } else {

        printf("El parámetro enviado no es uno válido.\n\n");

    }

    return 0;
}