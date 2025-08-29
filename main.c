/**
* @author Heitor Barreto e Vinícius Lopes
 * @date Julho de 2025
 */

#include <stdio.h>
#include "compilador.h"
#include <windows.h>

int main() {

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    arquivo_fonte = fopen("codigo_fonte.txt", "r");
    if (arquivo_fonte == NULL) {
        perror("Erro ao abrir o arquivo 'codigo_fonte.txt'");
        return 1;
    }

    /* --- ETAPA 1: EXIBIÇÃO DA ANÁLISE LÉXICA --- */
    printf("=== ANÁLISE LÉXICA ===\n\n");
    printf("%-10s | %-30s | %s\n", "LINHA", "TIPO DE TOKEN", "LEXEMA");
    printf("-----------------------------------------------------------------\n");

    Token token_lexico;
    do {
        token_lexico = obter_proximo_token();
        printf("%-10d | %-30s | %s\n", token_lexico.linha, tipo_token_para_str(token_lexico.tipo), token_lexico.lexema);

        // Se encontrar um erro léxico, para e não continua para o sintático.
        if (token_lexico.tipo == TOKEN_ERRO) {
            fflush(stdout); // Garante que a tabela seja impressa antes da mensagem de erro
            fprintf(stderr, "\nERRO LÉXICO: %s\n", token_lexico.lexema);
            destruir_token(token_lexico);
            fclose(arquivo_fonte);
            exibir_status_memoria();
            return 1; // Termina o programa com erro
        }
        destruir_token(token_lexico);
    } while (token_lexico.tipo != TOKEN_FIM_DE_ARQUIVO);

    printf("\n\n");

    /* --- ETAPA 2: ANÁLISE SINTÁTICA --- */

    // REBOBINA o arquivo e reseta a linha para o analisador sintático começar do início
    rewind(arquivo_fonte);
    linha_atual = 1;

    printf("=== ANÁLISE SINTÁTICA ===\n\n");

    /* Inicializa o analisador sintático */
    inicializar_parser();

    /* Realiza a análise sintática completa */
    printf("Iniciando análise sintática...\n");
    int sucesso = analisar_programa();

    /* Libera o último token */
    if (token_atual.lexema) {
        destruir_token(token_atual);
    }

    /* Exibe resultados */
    if (sucesso && !erro_sintatico_encontrado) {
        printf("\n✓ ANÁLISE SINTÁTICA CONCLUÍDA COM SUCESSO!\n");
        printf("✓ Programa sintaticamente correto.\n");

        /* Exibe tabela de símbolos APENAS se não houver erros */
        if (tabela_simbolos && tabela_simbolos->total_entradas > 0) {
            exibir_tabela_simbolos();
        }

        /* --- ETAPA 3: ANÁLISE SEMÂNTICA --- */
        printf("\n=== ANÁLISE SEMÂNTICA ===\n\n");
        printf("Executando verificações semânticas...\n");

        /* Exibe relatório semântico */
        exibir_relatorio_semantico();

    } else {
        printf("\n✗ ANÁLISE SINTÁTICA FALHOU!\n");
        printf("✗ Erros sintáticos encontrados no programa.\n");
    }

    /* Limpa recursos */
    fclose(arquivo_fonte);

    if (tabela_simbolos) {
        destruir_tabela_simbolos();
    }

    if (pilha_balanceamento) {
        destruir_pilha_balanceamento();
    }

    /* Limpa recursos semânticos */
    destruir_analisador_semantico();

    /* Exibe relatório de memória */
    exibir_status_memoria();

    return (sucesso && !erro_sintatico_encontrado) ? 0 : 1;
}