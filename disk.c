#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <unistd.h>

#define MiB (1024 * 1024)
#define GiB (MiB * 1024)

int main(int argc, char *argv[])
{
    char *path = argv[1];
    char *option = argv[2];

    struct statvfs stat;

    if (statvfs(path, &stat) != 0)
    {
        perror("Error al obtener la información de este directorio de archivos.");
        return 1;
    }

    unsigned long long fs_block_size = stat.f_frsize;

    unsigned long long total = (unsigned long long)stat.f_blocks * fs_block_size;
    unsigned long long available = (unsigned long long)stat.f_bfree * fs_block_size;
    unsigned long long used = total - available;

    printf("Estadisticas del disco:\n");

    if (argc == 2)
    {
        printf("- Disco total: %.2f GiB\n", (double)total / GiB);
        printf("- Disco usado: %.2f GiB\n", (double)used / GiB);
        printf("- Disco disponible: %.2f GiB\n", (double)available / GiB);
    } else if (argc == 3){
        if (strcmp(option, "-tm") == 0)
        {
            printf("- Disco usado: %.3f MiB\n", (double)used / MiB);
            printf("- Disco disponible: %.3f MiB\n", (double)available / MiB);
        } else if (strcmp(option, "-tg") == 0)
        {
            printf("- Disco usado: %.2f GiB\n", (double)used / GiB);
            printf("- Disco disponible: %.2f GiB\n", (double)available / GiB);
        } else {
            printf("El parámetro ingresado no es válido.\n");
        }
    }

    return 0;
}