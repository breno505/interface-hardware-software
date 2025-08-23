#include <stdio.h>
#include <stdlib.h>

#define DISK_IMAGE "meu_disco.img"
#define OUTPUT_DIR "fotos_recuperadas"
#define BUFFER_SIZE 512
#define RECOVERY_SIZE (2 * 1024 * 1024)

int main() {
    unsigned char buffer[BUFFER_SIZE];
    FILE *disk_file;
    FILE *output_file = NULL;
    char output_filename[100];
    int files_found = 0;


    char command[100];
    sprintf(command, "mkdir -p %s", OUTPUT_DIR);
    system(command);

    disk_file = fopen(DISK_IMAGE, "rb");
    if (disk_file == NULL) {
        perror("ERRO: Não foi possível abrir o arquivo de disco");
        return 1;
    }

    printf("Iniciando varredura em '%s'...\n", DISK_IMAGE);

    long long current_offset = 0;
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, disk_file)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (i + 2 < bytes_read && 
                buffer[i]   == 0xFF && 
                buffer[i+1] == 0xD8 && 
                buffer[i+2] == 0xFF) {

                long long found_address = current_offset + i;
                printf("\n✅ FANTASMA ENCONTRADO! Assinatura JPEG no endereço: 0x%llX\n", found_address);

                sprintf(output_filename, "%s/recuperado_%d.jpg", OUTPUT_DIR, files_found);

                output_file = fopen(output_filename, "wb");
                if (output_file == NULL) {
                    perror("ERRO: Não foi possível criar o arquivo de saída");
                    continue; 
                }
                
                printf(" -> Iniciando escavação para '%s'...\n", output_filename);

                fwrite(buffer + i, 1, bytes_read - i, output_file);
                
                long long bytes_to_recover = RECOVERY_SIZE - (bytes_read - i);
                unsigned char recovery_buffer[BUFFER_SIZE];
                size_t recovery_bytes_read;

                while (bytes_to_recover > 0 && 
                      (recovery_bytes_read = fread(recovery_buffer, 1, BUFFER_SIZE, disk_file)) > 0) {
                    
                    long long bytes_to_write = (bytes_to_recover > recovery_bytes_read) ? recovery_bytes_read : bytes_to_recover;
                    fwrite(recovery_buffer, 1, bytes_to_write, output_file);
                    bytes_to_recover -= bytes_to_write;
                }

                fclose(output_file);
                printf(" -> Escavação concluída para '%s'.\n", output_filename);
                files_found++;

            }
        }
        current_offset += bytes_read;
    }

    printf("\nVarredura completa. Total de %d fantasmas encontrados e recuperados.\n", files_found);
    fclose(disk_file);

    return 0;
}