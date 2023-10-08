# Servidor e Cliente C++ com Boost.Asio

Este é um projeto de exemplo que demonstra a implementação de um servidor e cliente em C++ usando a biblioteca Boost.Asio em ambientes Linux.
O servidor recebe dados da rede via comunicação TCP e os armazena em arquivos locais. O cliente envia informações para o servidor.

## Requisitos

Certifique-se de ter os seguintes requisitos antes de executar o projeto:

- Compilador C++ (g++ ou outro de sua preferência) - `sudo apt install -y build-essential`
- Biblioteca Boost (Boost.Asio) - `sudo apt install -y libboost-all-dev`
- Linux (ou ambiente compatível)

## Compilação e Execução

### Servidor

1. **Compilação**:

   Você pode compilar o servidor usando o g++ ou o CMake. Aqui estão os comandos para cada opção:

   Usando g++:

   ```shell
   g++ -o servidor servidor.cpp -lboost_system -lboost_thread
   ```

   Usando Make:

   ```shell
   make server
   ```

2. **Execução**:

   Para iniciar o servidor, execute o binário compilado:

   ```shell
   ./server
   ```

   O servidor começará a ouvir na porta especificada no arquivo de configuração.

### Cliente

1. **Compilação**:

   Você pode compilar o cliente da mesma maneira que o servidor:

   Usando g++:

   ```shell
   g++ -o cliente cliente.cpp -lboost_system
   ```

   Usando CMake:

   ```shell
   make client
   ```

2. **Execução**:

   Para iniciar o cliente, execute o binário compilado:

   ```shell
   ./client
   ```

   O cliente se conectará ao servidor na porta especificada no arquivo de configuração e enviará dados para ele.

## Configuração

1. **Arquivo de Configuração**:

   O servidor lê suas configurações de um arquivo chamado `config.txt`. Certifique-se de configurar a porta e o tamanho máximo dos arquivos neste arquivo antes de iniciar o servidor.

   Exemplo de `config.txt`:

   ```
   Port=12345
   MaxFileSize=1024
   ```

2. **Nome do Arquivo**:

   O nome dos arquivos gerados pelo servidor é configurável. Eles são nomeados com base em um prefixo definido no arquivo `config.txt`, ao qual é anexada uma marca de tempo no momento da abertura do arquivo.

## Funcionalidades Implementadas

- Comunicação TCP entre servidor e cliente.
- Recebimento de dados do cliente e armazenamento em arquivos locais.
- Configuração da porta e tamanho máximo dos arquivos através de um arquivo de configuração.
- Geração de nomes de arquivo com base em um prefixo e uma marca de tempo.

## Desenvolvedor

| <img src="https://github.com/Calebe94.png" width="200px">        |
|:----------------------------------------------------------------:|
| [Edimar Calebe Castanho (Calebe94)](https://github.com/Calebe94) |

## Licença

Este projeto está licenciado sob a [GNU General Public License v3.0 or later](https://www.gnu.org/licenses/gpl-3.0.en.html).
