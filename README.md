
Recuperação Digital

Disciplina: Interface Hardware/Software
Autor: Breno Murilo Alexandre Reis Santos

---

1. Visão Geral

O "Recuperador digital" é uma ferramenta de baixo nível para recuperação de arquivos, escrita em C para ambientes Linux (Testada somente em Ubuntu). 
O projeto demonstra na prática o conceito de file carving (escultura de arquivos), uma técnica de forense digital usada para recuperar dados que foram "deletados" de um dispositivo de armazenamento.
A ferramenta opera ignorando a estrutura do sistema de arquivos e, em vez disso, lê o disco uma imagem de disco como um fluxo contínuo de bytes, procurando por assinaturas de arquivos conhecidos para trazê-los de volta.

2. Como Funciona?

Quando um arquivo é deletado, o sistema operacional, por eficiência, não apaga os dados. Ele simplesmente remove a referência para aquele arquivo no seu "índice", marcando o espaço como livre. Os dados, no entanto, permanecem no disco como "fantasmas digitais" até serem sobrescritos.

Este programa funciona em duas fases:

    Radar (Mapeamento): Varre o disco em busca de assinaturas de cabeçalho (FF D8 FF) que marcam o início de um arquivo JPEG.

    Escavadeira (Extração): Ao encontrar um cabeçalho, ele começa a copiar os dados para um novo arquivo, procurando ativamente pela assinatura de rodapé (FF D9) para garantir uma recuperação precisa.

3. Funcionalidades Atuais

    Recuperação precisa de arquivos JPEG (.jpg e .jpeg).

    Busca por cabeçalho e rodapé para determinar os limites exatos do arquivo.

    Utiliza um limite de segurança de 2MB para recuperação, caso o rodapé do arquivo esteja corrompido ou não seja encontrado.

    Organiza todos os arquivos recuperados em um diretório de saída (fotos_recuperadas).

---

4. Guia Rápido de Execução

Siga este passo a passo para testar a ferramenta.
Pré-requisitos

    Ubuntu (outros sistemas operacionais baseados em Linux devem funcionar, mas todos os meus testes foram no Ubuntu).

    O pacote build-essential instalado (sudo apt install build-essential).

Passo 1: Compilação

Clone o repositório e compile o código-fonte para gerar o binário executável.

# Clone o repositório para a sua máquina local

# Navegue para o diretório do projeto
cd interface-hardware-software

# Compile o código-fonte utilizando o GCC
gcc recuperador.c -o recuperador.exec

Passo 2: Criação do Ambiente de Teste

AVISO: Esta ferramenta foi projetada para operar em imagens de disco. A execução em dispositivos de bloco reais (/dev/sda, etc.) pode levar à perda de dados e só deve ser feita por usuários avançados.

# 0. Apaga o disco vazio e a pasta de recuperação
rm -f meu_disco.img
rm -rf fotos_recuperadas/
rm -rf pasta_de_fotos

# 1. Crie uma imagem de disco de 100MB preenchida com zeros
dd if=/dev/zero of=meu_disco.img bs=1M count=100

# 2. Formate a imagem com um sistema de arquivos ext4
#    Confirme a operação com 'y' quando solicitado
mkfs.ext4 meu_disco.img

Passo 3: Popular e Preparar a Imagem de Disco

Para simular a perda de dados, vamos montar a imagem, copiar arquivos para dentro dela e então removê-los.

# 1. Crie um diretório que servirá como ponto de montagem
mkdir -p pasta_de_fotos

# 2. Monte o sistema de arquivos contido na imagem no ponto de montagem
sudo mount meu_disco.img pasta_de_fotos

# 3. Altere a propriedade do ponto de montagem para o seu usuário
sudo chown $USER:$USER pasta_de_fotos/

# 4. Copie arquivos JPEG para o diretório 'pasta_de_fotos'
#    (Pode ser feito manualmente através do gerenciador de arquivos)

# 5. Verifique se os arquivos foram copiados e sincronize
ls -lh pasta_de_fotos/
# IMPORTANTE - Não esqueça de sincronizar
sync


# 6. Remova os arquivos do sistema de arquivos da imagem
sudo rm pasta_de_fotos/*.jpg
sudo rm pasta_de_fotos/*.jpeg

# 7. Desmonte o ponto de montagem para finalizar a preparação
sudo umount pasta_de_fotos

Passo 4: Executar a Ferramenta de Recuperação

Com o ambiente de teste preparado, execute o programa.

./recuperador.exec

Passo 5: Verificar os Resultados

A ferramenta criará o diretório fotos_recuperadas (se não existir) e salvará todos os arquivos recuperados nele.

# Liste os arquivos recuperados
ls fotos_recuperadas/

Verifique o conteúdo do diretório e abra os arquivos de imagem para confirmar o sucesso da recuperação.