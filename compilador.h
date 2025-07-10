#ifndef COMPILADOR_H
#define COMPILADOR_H

#include <stdio.h>

// --- CONTROLE DE MEMORIA ---

/**
 * @brief Aloca uma quantidade de memória de forma segura.
 *
 * Esta função verifica se a alocação de memória não ultrapassará o limite
 * definido e interrompe o programa se não houver memória suficiente.
 * @param tamanho A quantidade de bytes a ser alocada.
 * @return Um ponteiro para a memória alocada.
 */
void* alocar_memoria(size_t tamanho);

/**
 * @brief Libera a memória previamente alocada.
 *
 * @param ptr O ponteiro para a memória a ser liberada.
 * @param tamanho A quantidade de bytes que foi alocada para o ponteiro.
 */
void liberar_memoria(void* ptr, size_t tamanho);

/**
 * @brief Exibe um relatório final sobre o uso de memória do programa.
 *
 * Mostra a memória total, o pico de uso e a memória restante.
 */
void exibir_status_memoria();

// --- ANALISADOR LEXICO ---

/**
 * @enum TipoToken
 * @brief Enumeração que define todos os possíveis tipos de tokens que o analisador léxico pode reconhecer.
 *
 * Cada token na linguagem-fonte terá um desses tipos.
 */
typedef enum {
    TOKEN_PRINCIPAL, TOKEN_FUNCAO, TOKEN_RETORNO, TOKEN_LEIA, TOKEN_ESCREVA, TOKEN_SE, TOKEN_SENAO, TOKEN_PARA,
    TOKEN_INTEIRO, TOKEN_TEXTO, TOKEN_DECIMAL, TOKEN_ID_VARIAVEL, TOKEN_ID_FUNCAO,
    TOKEN_LITERAL_NUMERO, TOKEN_LITERAL_TEXTO, TOKEN_OP_SOMA, TOKEN_OP_SUBTRACAO,
    TOKEN_OP_MULTIPLICACAO, TOKEN_OP_DIVISAO, TOKEN_OP_EXPONENCIACAO, TOKEN_OP_IGUAL,
    TOKEN_OP_DIFERENTE, TOKEN_OP_MENOR, TOKEN_OP_MENOR_IGUAL, TOKEN_OP_MAIOR, TOKEN_OP_MAIOR_IGUAL,
    TOKEN_OP_E, TOKEN_OP_OU, TOKEN_ATRIBUICAO, TOKEN_PARENTESES_ESQ, TOKEN_PARENTESES_DIR,
    TOKEN_CHAVES_ESQ, TOKEN_CHAVES_DIR, TOKEN_COLCHETES_ESQ, TOKEN_COLCHETES_DIR,
    TOKEN_PONTO_VIRGULA, TOKEN_VIRGULA, TOKEN_PONTO, TOKEN_FIM_DE_ARQUIVO, TOKEN_ERRO
} TipoToken;

/**
 * @struct Token
 * @brief Estrutura que representa um token individual.
 *
 * Contém o tipo do token, seu valor textual (lexema) e a linha onde foi encontrado.
 */
typedef struct {
    TipoToken tipo;
    char* lexema;
    int linha;
} Token;

extern FILE* arquivo_fonte; // Ponteiro para o arquivo de código-fonte sendo analisado.
extern int linha_atual;     // Contador da linha atual no arquivo-fonte.

/**
 * @brief Converte um TipoToken para sua representação em string.
 *
 * Útil para exibir o tipo de token de forma legível.
 * @param tipo O tipo de token a ser convertido.
 * @return Uma string representando o tipo de token.
 */
const char* tipo_token_para_str(TipoToken tipo);

/**
 * @brief Converte um TipoToken para sua representação em string.
 *
 * Útil para exibir o tipo de token de forma legível.
 * @param tipo O tipo de token a ser convertido.
 * @return Uma string representando o tipo de token.
 */
Token criar_token(TipoToken tipo, char* lexema, int linha);

/**
 * @brief Libera a memória alocada para um token.
 * @param token O token a ser destruído.
 */
void destruir_token(Token token);

/**
 * @brief Lê o arquivo-fonte e retorna o próximo token encontrado.
 *
 * É o coração do analisador léxico.
 * @return O próximo token do arquivo.
 */
Token obter_proximo_token();

#endif