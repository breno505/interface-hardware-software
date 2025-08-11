#include <stdio.h>
#include <stdlib.h>

#define DISCO_VIRTUAL "meu_disco.img"
#define BYTES_PARA_LER 32

int main() {
    unsigned char buffer[BYTES_PARA_LER];
    FILE *arquivo_disco;
    size_t bytes_lidos;

    // 1. Tenta abrir o disco virtual em modo de leitura binária ("rb")
    arquivo_disco = fopen(DISCO_VIRTUAL, "rb");
    if (arquivo_disco == NULL) {
        perror("ERRO: Não foi possível abrir o arquivo de disco");
        return 1;
    }

    // 2. Tenta ler os primeiros 32 bytes do arquivo
    bytes_lidos = fread(buffer, 1, BYTES_PARA_LER, arquivo_disco);
    if (bytes_lidos == 0) {
        printf("AVISO: Não foi possível ler nenhum byte. O arquivo está vazio?\n");
    }

    printf("✅ Prova de Conceito bem-sucedida!\n");
    printf("Os primeiros %zu bytes do arquivo '%s' são:\n", bytes_lidos, DISCO_VIRTUAL);

    // 3. Imprime os bytes lidos em formato hexadecimal
    for (size_t i = 0; i < bytes_lidos; i++) {
        printf("%02X ", buffer[i]); // %02X formata como hexadecimal de 2 dígitos
        if ((i + 1) % 16 == 0) { // Quebra a linha a cada 16 bytes
            printf("\n");
        }
    }
    printf("\n");

    // 4. Fecha o arquivo
    fclose(arquivo_disco);

    return 0;
}