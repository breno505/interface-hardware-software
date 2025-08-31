#include <stdio.h>
#include <stdlib.h> 

#define DISK_IMAGE "meu_disco.img"
#define BUFFER_SIZE 512 // Ler o disco em blocos de 512 bytes

int main() {
    unsigned char buffer[BUFFER_SIZE];
    FILE *disk_file;
    size_t bytes_read;
    long long current_offset = 0;

    // 1. Abrir o disco virtual para leitura binária
    disk_file = fopen(DISK_IMAGE, "rb");
    if (disk_file == NULL) {
        perror("ERRO: Não foi possível abrir o arquivo de disco");
        return 1;
    }

    printf("Iniciando varredura em '%s'...\n", DISK_IMAGE);

    // 2. Loop principal: lê o disco em blocos até o fim
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, disk_file)) > 0) {

        // 3. Loop secundário: varre o bloco que acabamos de ler
        for (int i = 0; i < bytes_read; i++) {

            // 4. A Lógica de Busca: Procurando pelo início de um JPEG (FF D8 FF)
            if (i + 2 < bytes_read && 
                buffer[i]   == 0xFF && 
                buffer[i+1] == 0xD8 && 
                buffer[i+2] == 0xFF) {

                // Assinatura encontrada.
                long long found_address = current_offset + i;
                
                printf("\n--------------------------------------------------\n");
                printf("✅ FANTASMA ENCONTRADO! Assinatura JPEG no endereço (offset): 0x%llX\n", found_address);
                printf("--------------------------------------------------\n");

                // Encontrar o primeiro e parar, por enquanto.
                fclose(disk_file);
                return 0; 
            }
        }
        
        // Atualiza em que parte do disco estamos
        current_offset += bytes_read;
    }

    printf("\nVarredura completa. Nenhum fantasma JPEG encontrado.\n");
    fclose(disk_file);

    return 0;
}