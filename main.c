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

    printf("=== ANÁLISE LÉXICO-SINTÁTICA ===\n\n");

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
    } else {
        printf("\n✗ ANÁLISE SINTÁTICA FALHOU!\n");
        printf("✗ Erros sintáticos encontrados no programa.\n");
    }

    /* Exibe tabela de símbolos */
    if (tabela_simbolos && tabela_simbolos->total_entradas > 0) {
        exibir_tabela_simbolos();
    }

    /* Limpa recursos */
    fclose(arquivo_fonte);

    if (tabela_simbolos) {
        destruir_tabela_simbolos();
    }

    if (pilha_balanceamento) {
        destruir_pilha_balanceamento();
    }

    /* Exibe relatório de memória */
    exibir_status_memoria();

    return sucesso ? 0 : 1;
}