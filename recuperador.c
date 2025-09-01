#include <stdio.h> // Funções de entrada e saída
#include <stdlib.h> // Funções de utilidade geral

// CONFIGURAÇÕES GLOBAIS

#define DISK_IMAGE "meu_disco.img" // Arquivo de imagem de disco que será analisado
#define OUTPUT_DIR "fotos_recuperadas" // Nome da pasta onde os arquivos recuperados serão salvos
#define BUFFER_SIZE 512 // O tamanho de cada "pedaço" que lemos do disco de uma vez
#define RECOVERY_SIZE (2 * 1024 * 1024) // Limite de segurança de 2MB caso o rodapé do JPEG não seja encontrado

int main() {
    // DECLARAÇÃO DE VARIÁVEIS

    unsigned char buffer[BUFFER_SIZE]; // Buffer para armazenar temporariamente os blocos de dados lidos do disco
    FILE *disk_file; // Conexão com o arquivo de imagem de disco
    FILE *output_file = NULL; // Conexão para o novo arquivo JPEG que vamos criar
    char output_filename[100]; // String para guardar o nome do arquivo de saída
    int files_found = 0; // Contador para o número de arquivos que já foram recuperados

    // PREPARAÇÃO DO AMBIENTE

    // Garante que o diretório de saída exista antes da execução
    char command[100];
    sprintf(command, "mkdir -p %s", OUTPUT_DIR); // O comando 'mkdir -p' cria a pasta e não gera erro se ela já existir
    system(command);

    // Abre o arquivo de imagem em modo "rb" (read binary / leitura binária)
    disk_file = fopen(DISK_IMAGE, "rb");
    if (disk_file == NULL) { // Se a abertura falhar, exibe um erro e encerra o programa
        perror("ERRO: Não foi possível abrir o arquivo de disco");
        return 1; // Retorna 1, indicando que o programa terminou com erro
    }

    printf("Iniciando varredura inteligente em '%s'...\n", DISK_IMAGE);

    long long current_offset = 0; // Rastreia a posição atual dentro do arquivo de imagem
    size_t bytes_read; // Armazena quantos bytes foram lidos em cada chamada do fread

    // LOOP PRINCIPAL DE VARREDURA

    // O loop continua enquanto a função fread conseguir ler blocos de dados do disco.
    // Quando chegar ao fim do arquivo, fread retornará 0 e o loop terminará.
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, disk_file)) > 0) {
        // Para cada bloco lido, varre o buffer byte por byte em busca da assinatura JPEG.
        for (int i = 0; i < bytes_read; i++) {
            // Condição para encontrar um cabeçalho JPEG (FF D8 FF).
            // A verificação `i + 2 < bytes_read` previne ler fora dos limites do buffer.
            if (i + 2 < bytes_read &&
                buffer[i]   == 0xFF &&
                buffer[i+1] == 0xD8 &&
                buffer[i+2] == 0xFF) {

                // LÓGICA DE RECUPERAÇÃO

                long long found_address = current_offset + i; // Calcula o endereço absoluto do "fantasma" no disco
                printf("\n✅ FANTASMA ENCONTRADO! Assinatura JPEG no endereço: 0x%llX\n", found_address);

                // Cria o nome do arquivo de saída (ex: "fotos_recuperadas/recuperado_0.jpg")
                sprintf(output_filename, "%s/recuperado_%d.jpg", OUTPUT_DIR, files_found);

                // Abre o novo arquivo em modo "wb" (write binary / escrita binária)
                output_file = fopen(output_filename, "wb");
                if (output_file == NULL) { // Se falhar, exibe erro e continua a busca
                    perror("ERRO: Não foi possível criar o arquivo de saída");
                    continue;
                }
                
                printf(" -> Iniciando recuperação para '%s'...\n", output_filename);

                // Escreve o final do buffer atual (que contém o cabeçalho) no novo arquivo
                fwrite(buffer + i, 1, bytes_read - i, output_file);
                
                long long bytes_to_recover = RECOVERY_SIZE - (bytes_read - i); // Define o limite máximo de recuperação
                unsigned char recovery_buffer[BUFFER_SIZE]; // Cria um buffer separado para a recuperação
                size_t recovery_bytes_read;
                int footer_found = 0; // Flag para indicar se o rodapé do JPEG foi encontrado

                // Loop de cópia: continua lendo do disco até atingir o limite OU encontrar o rodapé.
                while (bytes_to_recover > 0 &&
                      (recovery_bytes_read = fread(recovery_buffer, 1, BUFFER_SIZE, disk_file)) > 0) {
                    
                    long long bytes_to_write = (bytes_to_recover > recovery_bytes_read) ? recovery_bytes_read : bytes_to_recover;

                    // Procura pelo rodapé (FF D9) dentro do buffer que acabamos de ler
                    for (int j = 0; j < recovery_bytes_read - 1; j++) {
                        if (recovery_buffer[j] == 0xFF && recovery_buffer[j + 1] == 0xD9) {
                            bytes_to_write = j + 2; // Ajusta o tamanho para escrever apenas até o rodapé
                            footer_found = 1; // Ativa a flag
                            break; // Sai do loop 'for' de busca
                        }
                    }
                    
                    fwrite(recovery_buffer, 1, bytes_to_write, output_file); // Escreve os dados no arquivo de saída
                    bytes_to_recover -= bytes_to_write; // Atualiza o contador de bytes restantes

                    if (footer_found) { // Se o rodapé foi encontrado, paramos de copiar
                        break; // Sai do loop 'while' de cópia
                    }
                }

                fclose(output_file); // Fecha e salva o arquivo recém-criado
                if (footer_found) {
                    printf(" -> Recuperação precisa concluída para '%s'.\n", output_filename);
                } else {
                    printf(" -> Recuperação por tamanho fixo concluída para '%s' (rodapé não encontrado).\n", output_filename);
                }
                files_found++; // Incrementa o contador de arquivos encontrados
            }
        }
        current_offset += bytes_read; // Atualiza a posição no disco com o número de bytes processados
    }

    // FINALIZAÇÃO

    printf("\nVarredura completa. Total de %d fantasmas encontrados e recuperados.\n", files_found);
    fclose(disk_file); // Fecha a conexão com o arquivo de imagem de disco

    return 0; // Retorna 0, indicando que o programa terminou com sucesso
}
