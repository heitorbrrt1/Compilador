#include <stdio.h>
#include "compilador.h"

int main() {
    arquivo_fonte = fopen("codigo_fonte.txt", "r");
    if (arquivo_fonte == NULL) {
        perror("Erro ao abrir o arquivo 'codigo_fonte.txt'");
        return 1;
    }

    printf("Iniciando análise léxica...\n\n");
    printf("%-10s | %-30s | %s\n", "LINHA", "TIPO DE TOKEN", "LEXEMA");
    printf("-----------------------------------------------------------------\n");

    for (;;) {
        Token token = obter_proximo_token();
        printf("%-10d | %-30s | %s\n", token.linha, tipo_token_para_str(token.tipo), token.lexema);

        if (token.tipo == TOKEN_ERRO) {
            fprintf(stderr, "\nERRO LÉXICO: %s\n", token.lexema);
            destruir_token(token);
            break;
        }

        if (token.tipo == TOKEN_FIM_DE_ARQUIVO) {
            destruir_token(token);
            break;
        }

        destruir_token(token);
    }

    fclose(arquivo_fonte);
    exibir_status_memoria();

    return 0;
}
