#ifndef COMPILADOR_H
#define COMPILADOR_H

#include <stdio.h>

// --- CONTROLE DE MEMORIA ---
void* alocar_memoria(size_t tamanho);
void liberar_memoria(void* ptr, size_t tamanho);
void exibir_status_memoria();

// --- ANALISADOR LEXICO ---
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

typedef struct {
    TipoToken tipo;
    char* lexema;
    int linha;
} Token;

extern FILE* arquivo_fonte;
extern int linha_atual;

const char* tipo_token_para_str(TipoToken tipo);
Token criar_token(TipoToken tipo, char* lexema, int linha);
void destruir_token(Token token);
Token obter_proximo_token();

#endif
