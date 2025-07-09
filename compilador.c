#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "compilador.h"

// --- CONTROLE DE MEMÓRIA ---

#define MEMORIA_MAXIMA_KB 2048
long MEMORIA_TOTAL_DISPONIVEL = MEMORIA_MAXIMA_KB * 1024;
long memoria_alocada_atual = 0;
long memoria_pico_utilizada = 0;
int alerta_memoria_emitido = 0;

void* alocar_memoria(size_t tamanho) {
    if (memoria_alocada_atual + tamanho > MEMORIA_TOTAL_DISPONIVEL) {
        fprintf(stderr, "ERRO FATAL: Tentativa de alocação excede a memória máxima. Memória Insuficiente.\n");
        exit(EXIT_FAILURE);
    }
    void* ptr = malloc(tamanho);
    if (ptr == NULL) {
        fprintf(stderr, "ERRO FATAL: Falha ao alocar memória com malloc. Memória Insuficiente.\n");
        exit(EXIT_FAILURE);
    }
    memoria_alocada_atual += tamanho;
    if (memoria_alocada_atual > memoria_pico_utilizada) {
        memoria_pico_utilizada = memoria_alocada_atual;
    }
    double percentual_uso = (double)memoria_alocada_atual / MEMORIA_TOTAL_DISPONIVEL;
    if (percentual_uso >= 0.9 && percentual_uso < 1.0 && !alerta_memoria_emitido) {
        printf("ALERTA: Uso de memória atingiu %.2f%% da capacidade total.\n", percentual_uso * 100);
        alerta_memoria_emitido = 1;
    }
    return ptr;
}

void liberar_memoria(void* ptr, size_t tamanho) {
    if (ptr != NULL) {
        free(ptr);
        memoria_alocada_atual -= tamanho;
    }
}

void exibir_status_memoria() {
    printf("\n------------- RELATÓRIO DE MEMÓRIA -------------\n");
    printf("Memória Total Disponivel: %ld KB\n", MEMORIA_TOTAL_DISPONIVEL / 1024);
    printf("Pico de Memória Utilizada: %ld bytes (%.2f KB)\n", memoria_pico_utilizada, (double)memoria_pico_utilizada / 1024);
    printf("Memória Restante ao Final: %ld bytes\n", memoria_alocada_atual);
    printf("------------------------------------------------\n");
}

// --- ANALISADOR LÉXICO ---

FILE* arquivo_fonte;
int linha_atual = 1;

const char* tipo_token_para_str(TipoToken tipo) {
    switch (tipo) {
        case TOKEN_PRINCIPAL: return "PALAVRA_RESERVADA_PRINCIPAL";
        case TOKEN_FUNCAO: return "PALAVRA_RESERVADA_FUNCAO";
        case TOKEN_RETORNO: return "PALAVRA_RESERVADA_RETORNO";
        case TOKEN_LEIA: return "PALAVRA_RESERVADA_LEIA";
        case TOKEN_ESCREVA: return "PALAVRA_RESERVADA_ESCREVA";
        case TOKEN_SE: return "PALAVRA_RESERVADA_SE";
        case TOKEN_SENAO: return "PALAVRA_RESERVADA_SENAO";
        case TOKEN_PARA: return "PALAVRA_RESERVADA_PARA";
        case TOKEN_INTEIRO: return "TIPO_INTEIRO";
        case TOKEN_TEXTO: return "TIPO_TEXTO";
        case TOKEN_DECIMAL: return "TIPO_DECIMAL";
        case TOKEN_ID_VARIAVEL: return "ID_VARIAVEL";
        case TOKEN_ID_FUNCAO: return "ID_FUNCAO";
        case TOKEN_LITERAL_NUMERO: return "LITERAL_NUMERO";
        case TOKEN_LITERAL_TEXTO: return "LITERAL_TEXTO";
        case TOKEN_OP_SOMA: return "OPERADOR_SOMA";
        case TOKEN_OP_SUBTRACAO: return "OPERADOR_SUBTRACAO";
        case TOKEN_OP_MULTIPLICACAO: return "OPERADOR_MULTIPLICACAO";
        case TOKEN_OP_DIVISAO: return "OPERADOR_DIVISAO";
        case TOKEN_OP_EXPONENCIACAO: return "OPERADOR_EXPONENCIACAO";
        case TOKEN_OP_IGUAL: return "OPERADOR_IGUAL";
        case TOKEN_OP_DIFERENTE: return "OPERADOR_DIFERENTE";
        case TOKEN_OP_MENOR: return "OPERADOR_MENOR";
        case TOKEN_OP_MENOR_IGUAL: return "OPERADOR_MENOR_IGUAL";
        case TOKEN_OP_MAIOR: return "OPERADOR_MAIOR";
        case TOKEN_OP_MAIOR_IGUAL: return "OPERADOR_MAIOR_IGUAL";
        case TOKEN_OP_E: return "OPERADOR_E_LOGICO";
        case TOKEN_OP_OU: return "OPERADOR_OU_LOGICO";
        case TOKEN_ATRIBUICAO: return "OPERADOR_ATRIBUICAO";
        case TOKEN_PARENTESES_ESQ: return "PARENTESES_ESQUERDO";
        case TOKEN_PARENTESES_DIR: return "PARENTESES_DIREITO";
        case TOKEN_CHAVES_ESQ: return "CHAVES_ESQUERDA";
        case TOKEN_CHAVES_DIR: return "CHAVES_DIREITA";
        case TOKEN_COLCHETES_ESQ: return "COLCHETES_ESQUERDO";
        case TOKEN_COLCHETES_DIR: return "COLCHETES_DIREITO";
        case TOKEN_PONTO_VIRGULA: return "PONTO_E_VIRGULA";
        case TOKEN_VIRGULA: return "VIRGULA";
        case TOKEN_PONTO: return "PONTO_DECIMAL";
        case TOKEN_FIM_DE_ARQUIVO: return "FIM_DE_ARQUIVO";
        case TOKEN_ERRO: return "TOKEN_ERRO";
        default: return "DESCONHECIDO";
    }
}

Token criar_token(TipoToken tipo, char* lexema, int linha) {
    Token token;
    token.tipo = tipo;
    token.linha = linha;
    size_t len = strlen(lexema) + 1;
    token.lexema = (char*) alocar_memoria(len);
    strncpy(token.lexema, lexema, len);
    return token;
}

void destruir_token(Token token) {
    if (token.lexema) {
        liberar_memoria(token.lexema, strlen(token.lexema) + 1);
    }
}

int proximo_char() {
    int c = fgetc(arquivo_fonte);
    if (c == '\n') {
        linha_atual++;
    }
    return c;
}

void devolver_char(int c) {
    if (c == '\n') {
        linha_atual--;
    }
    ungetc(c, arquivo_fonte);
}

TipoToken verificar_palavra_reservada(const char* str) {
    if (strcmp(str, "principal") == 0) return TOKEN_PRINCIPAL;
    if (strcmp(str, "funcao") == 0) return TOKEN_FUNCAO;
    if (strcmp(str, "retorno") == 0) return TOKEN_RETORNO;
    if (strcmp(str, "leia") == 0) return TOKEN_LEIA;
    if (strcmp(str, "escreva") == 0) return TOKEN_ESCREVA;
    if (strcmp(str, "se") == 0) return TOKEN_SE;
    if (strcmp(str, "senao") == 0) return TOKEN_SENAO;
    if (strcmp(str, "para") == 0) return TOKEN_PARA;
    if (strcmp(str, "inteiro") == 0) return TOKEN_INTEIRO;
    if (strcmp(str, "texto") == 0) return TOKEN_TEXTO;
    if (strcmp(str, "decimal") == 0) return TOKEN_DECIMAL;
    return TOKEN_ERRO;
}

Token obter_proximo_token() {
    int c;
    char buffer[256];
    int i = 0;

    while ((c = proximo_char()) != EOF) {
        if (isspace(c)) continue;
        switch (c) {
            case '+': return criar_token(TOKEN_OP_SOMA, "+", linha_atual);
            case '-': return criar_token(TOKEN_OP_SUBTRACAO, "-", linha_atual);
            case '*': return criar_token(TOKEN_OP_MULTIPLICACAO, "*", linha_atual);
            case '/': return criar_token(TOKEN_OP_DIVISAO, "/", linha_atual);
            case '^': return criar_token(TOKEN_OP_EXPONENCIACAO, "^", linha_atual);
            case '(': return criar_token(TOKEN_PARENTESES_ESQ, "(", linha_atual);
            case ')': return criar_token(TOKEN_PARENTESES_DIR, ")", linha_atual);
            case '{': return criar_token(TOKEN_CHAVES_ESQ, "{", linha_atual);
            case '}': return criar_token(TOKEN_CHAVES_DIR, "}", linha_atual);
            case '[': return criar_token(TOKEN_COLCHETES_ESQ, "[", linha_atual);
            case ']': return criar_token(TOKEN_COLCHETES_DIR, "]", linha_atual);
            case ';': return criar_token(TOKEN_PONTO_VIRGULA, ";", linha_atual);
            case ',': return criar_token(TOKEN_VIRGULA, ",", linha_atual);
            case '.': return criar_token(TOKEN_PONTO, ".", linha_atual);
            case '=':
                if ((c = proximo_char()) == '=') return criar_token(TOKEN_OP_IGUAL, "==", linha_atual);
                else { devolver_char(c); return criar_token(TOKEN_ATRIBUICAO, "=", linha_atual); }
            case '<':
                c = proximo_char();
                if (c == '=') return criar_token(TOKEN_OP_MENOR_IGUAL, "<=", linha_atual);
                else if (c == '>') return criar_token(TOKEN_OP_DIFERENTE, "<>", linha_atual);
                else { devolver_char(c); return criar_token(TOKEN_OP_MENOR, "<", linha_atual); }
            case '>':
                if ((c = proximo_char()) == '=') return criar_token(TOKEN_OP_MAIOR_IGUAL, ">=", linha_atual);
                else { devolver_char(c); return criar_token(TOKEN_OP_MAIOR, ">", linha_atual); }
            case '&':
                if ((c = proximo_char()) == '&') return criar_token(TOKEN_OP_E, "&&", linha_atual);
                else {
                    devolver_char(c);
                    sprintf(buffer, "Caractere inesperado: '&' na linha %d", linha_atual);
                    return criar_token(TOKEN_ERRO, buffer, linha_atual);
                }
            case '|':
                if ((c = proximo_char()) == '|') return criar_token(TOKEN_OP_OU, "||", linha_atual);
                else {
                    devolver_char(c);
                    sprintf(buffer, "Caractere inesperado: '|' na linha %d", linha_atual);
                    return criar_token(TOKEN_ERRO, buffer, linha_atual);
                }
            case '"':
                i = 0;
                while((c = proximo_char()) != '"' && c != EOF && i < 255) buffer[i++] = c;
                if (c == EOF) return criar_token(TOKEN_ERRO, "String literal não fechada", linha_atual);
                buffer[i] = '\0';
                return criar_token(TOKEN_LITERAL_TEXTO, buffer, linha_atual);
            default: ;
        }

        if (c == '!') {
            i = 0;
            buffer[i++] = c;
            c = proximo_char();
            if (!islower(c)) {
                sprintf(buffer, "Nome de variável inválido na linha %d. Esperado a-z após '!'.", linha_atual);
                return criar_token(TOKEN_ERRO, buffer, linha_atual);
            }
            buffer[i++] = c;
            while (isalnum(c = proximo_char()) && i < 255) buffer[i++] = c;
            devolver_char(c);
            buffer[i] = '\0';
            return criar_token(TOKEN_ID_VARIAVEL, buffer, linha_atual);
        }

        if (isdigit(c)) {
            i = 0;
            int tem_ponto = 0;
            buffer[i++] = c;
            while ((isdigit(c = proximo_char()) || c == '.') && i < 255) {
                if (c == '.') {
                    if (tem_ponto) break;
                    tem_ponto = 1;
                }
                buffer[i++] = c;
            }
            devolver_char(c);
            buffer[i] = '\0';
            return criar_token(TOKEN_LITERAL_NUMERO, buffer, linha_atual);
        }

        if (isalpha(c) || c == '_') {
            i = 0;
            buffer[i++] = c;

            if (c == '_') {
                if ((c = proximo_char()) == '_') {
                    buffer[i++] = c;
                    c = proximo_char();
                    if (!isalnum(c)) {
                        sprintf(buffer, "Nome de função inválido na linha %d. Esperado caractere alfanumérico após '__'.", linha_atual);
                        return criar_token(TOKEN_ERRO, buffer, linha_atual);
                    }
                    buffer[i++] = c;
                    while ((isalnum(c = proximo_char()) || c == '_') && i < 255) {
                        buffer[i++] = c;
                    }
                    devolver_char(c);
                    buffer[i] = '\0';
                    return criar_token(TOKEN_ID_FUNCAO, buffer, linha_atual);
                } else {
                    devolver_char(c);
                    sprintf(buffer, "Identificador inválido '_' na linha %d.", linha_atual);
                    return criar_token(TOKEN_ERRO, buffer, linha_atual);
                }
            }

            while ((isalnum(c = proximo_char()) || c == '_') && i < 255) {
                buffer[i++] = c;
            }
            devolver_char(c);
            buffer[i] = '\0';

            TipoToken tipo_reservado = verificar_palavra_reservada(buffer);
            if (tipo_reservado != TOKEN_ERRO) {
                return criar_token(tipo_reservado, buffer, linha_atual);
            }

            char erro_msg[512];
            sprintf(erro_msg, "Identificador ou palavra reservada inválida '%s' na linha %d.", buffer, linha_atual);
            return criar_token(TOKEN_ERRO, erro_msg, linha_atual);
        }

        char erro_msg[512];
        if (isprint(c)) {
            sprintf(erro_msg, "Caractere não reconhecido '%c' na linha %d.", c, linha_atual);
        } else {
            sprintf(erro_msg, "Caractere não reconhecido (ASCII: %d) na linha %d.", c, linha_atual);
        }
        return criar_token(TOKEN_ERRO, erro_msg, linha_atual);
    }

    return criar_token(TOKEN_FIM_DE_ARQUIVO, "EOF", linha_atual);
}