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

    int fd;
    char buffer[BUFFER];

    char* temp = "/tmp/temp";

    if (argc < 2){

        printf("Programa ejecutado sin un parámetro válido.\n\n");

    } else if (strcmp(argv[1],"cpu") == 0){ // Llamado para datos de la CPU

        pid = fork();

        if(pid == -1){
            printf("Error al invocar al proceso hijo.");
            exit(1);
        }

        if(pid == 0){
            char* cpu_A[] = {
                "cpu",
                temp,
                argv[1],
                argv[2],
                NULL
            };

            execv("./cpu", cpu_A);
            printf("Error, no inicializó el proceso hijo.");
        } else {
            wait(NULL);

            fd = open(temp, O_RDONLY);
            if (fd == -1) {
                perror("Error al abrir archivo temporal.");
                exit(1);
            }

            read(fd, buffer, sizeof(buffer));

            printf("%s\n\n", buffer);

            close(fd);
            unlink(temp);                
        }

    } else if (strcmp(argv[1],"memoria") == 0){ // Llamado para datos de la memoria RAM

        pid = fork();

        if(pid == -1){
            printf("Error al invocar al proceso hijo.");
            exit(1);
        }

        if(pid == 0){
            char* memoria_A[] = {
                "ram",
                temp,
                argv[1],
                argv[2],
                argv[3],
                NULL
            };

            execv("./ram", memoria_A);
            printf("Error, no inicializó el proceso hijo.");

        } else {
            wait(NULL);

            fd = open(temp, O_RDONLY);
            if (fd == -1) {
                perror("Error al abrir archivo temporal.");
                exit(1);
            }

            read(fd, buffer, sizeof(buffer));
            printf("%s\n\n", buffer);

            close(fd);
            unlink(temp);                
        }

    } else if (strcmp(argv[1],"disco") == 0){ // Llamado para datos de la RAM

        pid = fork();

        if(pid == -1){
            printf("Error al invocar al proceso hijo.");
            exit(1);
        }

        if(pid == 0){
            char* disk_A[] = {"disk", NULL};
            execv("./disk", disk_A);
            printf("Error, no actualizó el proceso.");
        } else {
            wait(NULL);

            char* temp = "/tmp/temp";

            fd = open(temp, O_RDONLY);
            if (fd == -1) {
                perror("Error al abrir archivo temporal.");
                exit(1);
            }

            read(fd, buffer, sizeof(buffer));

            printf("%s\n\n", buffer);

            close(fd);
            unlink(temp);                
        }

    } else {

        printf("El parámetro enviado no es uno válido.\n\n");

    }

    return 0;
}