#include <stdio.h>
#include "compilador.h"
#include <windows.h>

int main() {
    // Configura o console para exibir caracteres UTF-8 corretamente no Windows.    
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    arquivo_fonte = fopen("codigo_fonte.txt", "r");
    if (arquivo_fonte == NULL) {
        perror("Erro ao abrir o arquivo 'codigo_fonte.txt'");
        return 1;
    }

    printf("Análise Léxica: \n\n");
    printf("%-10s | %-30s | %s\n", "LINHA", "TIPO DE TOKEN", "LEXEMA");
    printf("-----------------------------------------------------------------\n");
    // Loop principal: continua pegando tokens até o fim do arquivo ou um erro.
    for (;;) {
        Token token = obter_proximo_token();
        printf("%-10d | %-30s | %s\n", token.linha, tipo_token_para_str(token.tipo), token.lexema);

        // Se um erro léxico for encontrado, exibe a mensagem de erro e para.
        if (token.tipo == TOKEN_ERRO) {
            fflush(stdout);
            fprintf(stderr, "\nERRO LÉXICO: %s\n", token.lexema);
            destruir_token(token);
            break;
        }
        // Se o token de fim de arquivo for encontrado, o processo terminou com sucesso.
        if (token.tipo == TOKEN_FIM_DE_ARQUIVO) {
            destruir_token(token);
            break;
        }
        // Libera a memória alocada para o token atual antes de pegar o próximo.
        destruir_token(token);
    }

    fclose(arquivo_fonte); // Fecha o arquivo.
    exibir_status_memoria(); // Exibe o relatório de memória.

    return 0;
}