/**
 * @author Heitor Barreto e Vinícius Lopes
 * @date Julho de 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compilador.h"

/* --- VARIÁVEIS GLOBAIS DO PARSER --- */
Token token_atual;
int erro_sintatico_encontrado = 0;
int modulo_principal_encontrado = 0;

/* --- TABELA DE SÍMBOLOS --- */
TabelaSimbolos* tabela_simbolos = NULL;

void inicializar_tabela_simbolos() {
    tabela_simbolos = (TabelaSimbolos*) alocar_memoria(sizeof(TabelaSimbolos));
    tabela_simbolos->primeira = NULL;
    tabela_simbolos->total_entradas = 0;
}

void adicionar_variavel(const char* nome, TipoDado tipo, const char* funcao_escopo,
                       LimitadorTamanho limitador, int tem_limitador) {
    /* Verifica se variável já existe (nomes devem ser únicos globalmente) */
    if (buscar_variavel(nome) != NULL) {
        printf("ALERTA: Variável '%s' já foi declarada anteriormente na linha %d.\n", nome, token_atual.linha);
        return;
    }

    EntradaTabela* nova = (EntradaTabela*) alocar_memoria(sizeof(EntradaTabela));

    size_t len_nome = strlen(nome) + 1;
    nova->nome = (char*) alocar_memoria(len_nome);
    strncpy(nova->nome, nome, len_nome);

    nova->tipo = tipo;
    nova->valor = NULL;

    size_t len_funcao = strlen(funcao_escopo) + 1;
    nova->funcao_escopo = (char*) alocar_memoria(len_funcao);
    strncpy(nova->funcao_escopo, funcao_escopo, len_funcao);

    nova->limitador = limitador;
    nova->tem_limitador = tem_limitador;
    nova->proxima = tabela_simbolos->primeira;

    tabela_simbolos->primeira = nova;
    tabela_simbolos->total_entradas++;
}

EntradaTabela* buscar_variavel(const char* nome) {
    EntradaTabela* atual = tabela_simbolos->primeira;
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            return atual;
        }
        atual = atual->proxima;
    }
    return NULL;
}

void exibir_tabela_simbolos() {
    printf("\n------------- TABELA DE SÍMBOLOS -------------\n");
    printf("%-15s | %-10s | %-15s | %-10s | %s\n", "NOME", "TIPO", "FUNÇÃO/ESCOPO", "LIMITADOR", "VALOR");
    printf("----------------------------------------------------------------------\n");

    EntradaTabela* atual = tabela_simbolos->primeira;
    while (atual != NULL) {
        const char* tipo_str;
        char limitador_str[32] = "N/A";

        switch (atual->tipo) {
            case TIPO_INTEIRO: tipo_str = "inteiro"; break;
            case TIPO_TEXTO: tipo_str = "texto"; break;
            case TIPO_DECIMAL: tipo_str = "decimal"; break;
            default: tipo_str = "desconhecido"; break;
        }

        if (atual->tem_limitador) {
            if (atual->tipo == TIPO_TEXTO) {
                snprintf(limitador_str, sizeof(limitador_str), "[%d]", atual->limitador.tamanho1);
            } else if (atual->tipo == TIPO_DECIMAL) {
                snprintf(limitador_str, sizeof(limitador_str), "[%d.%d]", atual->limitador.tamanho1, atual->limitador.tamanho2);
            }
        }

        printf("%-15s | %-10s | %-15s | %-10s | %s\n",
               atual->nome, tipo_str, atual->funcao_escopo, limitador_str,
               atual->valor ? atual->valor : "N/A");
        atual = atual->proxima;
    }
    printf("----------------------------------------------------------------------\n");
    printf("Total de variáveis: %d\n", tabela_simbolos->total_entradas);
}

void destruir_tabela_simbolos() {
    EntradaTabela* atual = tabela_simbolos->primeira;
    while (atual != NULL) {
        EntradaTabela* proxima = atual->proxima;

        liberar_memoria(atual->nome, strlen(atual->nome) + 1);
        liberar_memoria(atual->funcao_escopo, strlen(atual->funcao_escopo) + 1);
        if (atual->valor) {
            liberar_memoria(atual->valor, strlen(atual->valor) + 1);
        }
        liberar_memoria(atual, sizeof(EntradaTabela));

        atual = proxima;
    }
    liberar_memoria(tabela_simbolos, sizeof(TabelaSimbolos));
}

/* --- PILHA DE BALANCEAMENTO --- */
PilhaBalanceamento* pilha_balanceamento = NULL;

void inicializar_pilha_balanceamento() {
    pilha_balanceamento = (PilhaBalanceamento*) alocar_memoria(sizeof(PilhaBalanceamento));
    pilha_balanceamento->capacidade = 100;
    pilha_balanceamento->itens = (ItemBalanceamento*) alocar_memoria(sizeof(ItemBalanceamento) * 100);
    pilha_balanceamento->topo = -1;
}

void empilhar_delimitador(char delimitador, int linha) {
    if (pilha_balanceamento->topo >= pilha_balanceamento->capacidade - 1) {
        fprintf(stderr, "ERRO: Pilha de balanceamento cheia na linha %d.\n", linha);
        erro_sintatico_encontrado = 1;
        return;
    }

    pilha_balanceamento->topo++;
    pilha_balanceamento->itens[pilha_balanceamento->topo].delimitador = delimitador;
    pilha_balanceamento->itens[pilha_balanceamento->topo].linha = linha;
}

int desempilhar_delimitador(char delimitador_fechamento, int linha) {
    if (pilha_balanceamento->topo < 0) {
        fprintf(stderr, "ERRO SINTÁTICO: Delimitador '%c' sem abertura correspondente na linha %d.\n",
                delimitador_fechamento, linha);
        erro_sintatico_encontrado = 1;
        return 0;
    }

    char delimitador_abertura = pilha_balanceamento->itens[pilha_balanceamento->topo].delimitador;
    int linha_abertura = pilha_balanceamento->itens[pilha_balanceamento->topo].linha;

    char esperado = ' ';
    switch (delimitador_fechamento) {
        case ')': esperado = '('; break;
        case '}': esperado = '{'; break;
        case ']': esperado = '['; break;
        case '"': esperado = '"'; break;
        default: ;
    }

    if (delimitador_abertura != esperado) {
        fprintf(stderr, "ERRO SINTÁTICO: Delimitador '%c' na linha %d não corresponde ao '%c' aberto na linha %d.\n",
                delimitador_fechamento, linha, delimitador_abertura, linha_abertura);
        erro_sintatico_encontrado = 1;
        return 0;
    }

    pilha_balanceamento->topo--;
    return 1;
}

void destruir_pilha_balanceamento() {
    if (pilha_balanceamento) {
        liberar_memoria(pilha_balanceamento->itens, sizeof(ItemBalanceamento) * pilha_balanceamento->capacidade);
        liberar_memoria(pilha_balanceamento, sizeof(PilhaBalanceamento));
    }
}

/* --- FUNÇÕES DO PARSER --- */

void inicializar_parser() {
    inicializar_tabela_simbolos();
    inicializar_pilha_balanceamento();
    inicializar_analisador_semantico();
    erro_sintatico_encontrado = 0;
    modulo_principal_encontrado = 0;
    token_atual = obter_proximo_token();
}

void consumir_token() {
    if (token_atual.tipo != TOKEN_FIM_DE_ARQUIVO && token_atual.tipo != TOKEN_ERRO) {
        destruir_token(token_atual);
        token_atual = obter_proximo_token();
    }
}

int esperar_token(TipoToken tipo_esperado) {
    if (token_atual.tipo == tipo_esperado) {
        consumir_token();
        return 1;
    } else {
        fprintf(stderr, "ERRO SINTÁTICO: Esperado %s, encontrado %s ('%s') na linha %d.\n",
                tipo_token_para_str(tipo_esperado), tipo_token_para_str(token_atual.tipo),
                token_atual.lexema, token_atual.linha);
        erro_sintatico_encontrado = 1;
        return 0;
    }
}

int verificar_ausencia_token(TipoToken token_nao_esperado, const char* contexto) {
    if (token_atual.tipo == token_nao_esperado) {
        fprintf(stderr, "ERRO SINTÁTICO: Token '%s' não deveria estar presente após %s na linha %d.\n",
                token_atual.lexema, contexto, token_atual.linha);
        erro_sintatico_encontrado = 1;
        return 0;
    }
    return 1;
}

int analisar_programa() {
    /* Programa = (Funcao | Declaracao)* */
    while (token_atual.tipo != TOKEN_FIM_DE_ARQUIVO && !erro_sintatico_encontrado) {
        if (token_atual.tipo == TOKEN_PRINCIPAL || token_atual.tipo == TOKEN_FUNCAO) {
            if (!analisar_funcao()) {
                return 0;
            }
        } else if (token_atual.tipo == TOKEN_INTEIRO || token_atual.tipo == TOKEN_TEXTO || token_atual.tipo == TOKEN_DECIMAL) {
            if (!analisar_declaracao_variavel("global")) {
                return 0;
            }
        } else {
            fprintf(stderr, "ERRO SINTÁTICO: Token inesperado '%s' na linha %d. Esperado função ou declaração de variável.\n",
                    token_atual.lexema, token_atual.linha);
            erro_sintatico_encontrado = 1;
            return 0;
        }
    }

    if (!modulo_principal_encontrado) {
        fprintf(stderr, "ERRO SINTÁTICO: Módulo Principal Inexistente.\n");
        erro_sintatico_encontrado = 1;
        return 0;
    }

    /* Verifica se há delimitadores não fechados */
    if (pilha_balanceamento->topo >= 0) {
        char delim = pilha_balanceamento->itens[pilha_balanceamento->topo].delimitador;
        int linha = pilha_balanceamento->itens[pilha_balanceamento->topo].linha;
        fprintf(stderr, "ERRO SINTÁTICO: Delimitador '%c' aberto na linha %d não foi fechado.\n", delim, linha);
        erro_sintatico_encontrado = 1;
        return 0;
    }

    return !erro_sintatico_encontrado;
}

int analisar_funcao() {
    char nome_funcao[256];
    int linha_funcao = linha_atual;

    if (token_atual.tipo == TOKEN_PRINCIPAL) {
        strcpy(nome_funcao, "principal");
        modulo_principal_encontrado = 1;
        adicionar_funcao_declarada("principal", linha_funcao);
        consumir_token();

        /* principal() não tem parâmetros */
        if (!esperar_token(TOKEN_PARENTESES_ESQ)) return 0;
        empilhar_delimitador('(', token_atual.linha - 1);
        if (!esperar_token(TOKEN_PARENTESES_DIR)) return 0;
        if (!desempilhar_delimitador(')', token_atual.linha - 1)) return 0;

    } else if (token_atual.tipo == TOKEN_FUNCAO) {
        consumir_token();

        if (token_atual.tipo != TOKEN_ID_FUNCAO) {
            fprintf(stderr, "ERRO SINTÁTICO: Esperado nome de função após 'funcao' na linha %d.\n", token_atual.linha);
            erro_sintatico_encontrado = 1;
            return 0;
        }

        strcpy(nome_funcao, token_atual.lexema);
        adicionar_funcao_declarada(nome_funcao, linha_funcao);
        consumir_token();

        if (!esperar_token(TOKEN_PARENTESES_ESQ)) return 0;
        empilhar_delimitador('(', token_atual.linha - 1);

        /* Parâmetros (se houver) */
        if (token_atual.tipo != TOKEN_PARENTESES_DIR) {
            do {
                TipoDado tipo_param;
                if (token_atual.tipo == TOKEN_INTEIRO) tipo_param = TIPO_INTEIRO;
                else if (token_atual.tipo == TOKEN_TEXTO) tipo_param = TIPO_TEXTO;
                else if (token_atual.tipo == TOKEN_DECIMAL) tipo_param = TIPO_DECIMAL;
                else {
                    fprintf(stderr, "ERRO SINTÁTICO: Esperado tipo de dado para o parâmetro na linha %d.\n", token_atual.linha);
                    erro_sintatico_encontrado = 1;
                    return 0;
                }
                consumir_token(); // Consome o tipo (inteiro, texto, etc.)

                if (token_atual.tipo != TOKEN_ID_VARIAVEL) {
                    fprintf(stderr, "ERRO SINTÁTICO: Esperado nome de variável para o parâmetro na linha %d.\n", token_atual.linha);
                    erro_sintatico_encontrado = 1;
                    return 0;
                }

                // Adiciona o parâmetro à tabela de símbolos (sem limitador, conforme especificação)
                adicionar_variavel(token_atual.lexema, tipo_param, nome_funcao, (LimitadorTamanho){0, 0}, 0);
                consumir_token(); // Consome o nome da variável

                // Se houver uma vírgula, espera o próximo parâmetro
                if (token_atual.tipo == TOKEN_VIRGULA) {
                    consumir_token();
                } else {
                    break; // Sai do loop se não houver mais vírgulas
                }
            } while (1);
        }

        if (!esperar_token(TOKEN_PARENTESES_DIR)) return 0;
        if (!desempilhar_delimitador(')', token_atual.linha - 1)) return 0;
    }

    /* Corpo da função */
    if (!analisar_bloco(nome_funcao)) return 0;

    return 1;
}

int analisar_declaracao_variavel(const char* funcao_escopo) {
    TipoDado tipo;
    LimitadorTamanho limitador = {0, 0};
    int tem_limitador = 0;

    /* Determina o tipo */
    if (token_atual.tipo == TOKEN_INTEIRO) {
        tipo = TIPO_INTEIRO;
    } else if (token_atual.tipo == TOKEN_TEXTO) {
        tipo = TIPO_TEXTO;
    } else if (token_atual.tipo == TOKEN_DECIMAL) {
        tipo = TIPO_DECIMAL;
    } else {
        fprintf(stderr, "ERRO SINTÁTICO: Esperado tipo de dado na linha %d.\n", token_atual.linha);
        erro_sintatico_encontrado = 1;
        return 0;
    }
    consumir_token();

    /* Lista de variáveis */
    do {
        if (token_atual.tipo != TOKEN_ID_VARIAVEL) {
            fprintf(stderr, "ERRO SINTÁTICO: Esperado nome de variável na linha %d.\n", token_atual.linha);
            erro_sintatico_encontrado = 1;
            return 0;
        }

        char nome_variavel[256];
        strcpy(nome_variavel, token_atual.lexema);
        consumir_token();

                /* Verifica limitadores de tamanho */
        if (token_atual.tipo == TOKEN_COLCHETES_ESQ) {
            empilhar_delimitador('[', token_atual.linha);
            consumir_token();

            if (token_atual.tipo != TOKEN_LITERAL_NUMERO) {
                fprintf(stderr, "ERRO SINTÁTICO: Esperado número no limitador de tamanho na linha %d.\n", token_atual.linha);
                erro_sintatico_encontrado = 1;
                return 0;
            }

            tem_limitador = 1;

            if (tipo == TIPO_DECIMAL) {
                char* ponto = strchr(token_atual.lexema, '.');
                if (ponto) {
                    // Caso o lexema seja "10.2" (um token único)
                    limitador.tamanho1 = atoi(token_atual.lexema);
                    limitador.tamanho2 = atoi(ponto + 1);
                    consumir_token(); // Consome o token "10.2"
                } else {
                    // Caso o lexema seja apenas a parte inteira "10"
                    limitador.tamanho1 = atoi(token_atual.lexema);
                    consumir_token(); // Consome o "10"
                    // E verifica se o próximo token é um ponto
                    if (token_atual.tipo == TOKEN_PONTO) {
                        consumir_token(); // Consome o "."
                        if (token_atual.tipo != TOKEN_LITERAL_NUMERO) {
                            fprintf(stderr, "ERRO SINTÁTICO: Esperado número após ponto no limitador decimal na linha %d.\n", token_atual.linha);
                            erro_sintatico_encontrado = 1;
                            return 0;
                        }
                        limitador.tamanho2 = atoi(token_atual.lexema);
                        consumir_token(); // Consome a parte decimal "2"
                    }
                }
            } else {
                // Lógica original para tipos texto e inteiro
                limitador.tamanho1 = atoi(token_atual.lexema);
                consumir_token();
            }

            if (!esperar_token(TOKEN_COLCHETES_DIR)) return 0;
            if (!desempilhar_delimitador(']', token_atual.linha - 1)) return 0;
        }

        /* Adiciona variável na tabela de símbolos */
        adicionar_variavel(nome_variavel, tipo, funcao_escopo, limitador, tem_limitador);

        /* Atribuição inicial (opcional) */
        if (token_atual.tipo == TOKEN_ATRIBUICAO) {
            consumir_token();
            if (!analisar_expressao()) return 0;
        }

        if (token_atual.tipo == TOKEN_VIRGULA) {
            consumir_token();
        } else {
            break;
        }
    } while (1);

    if (!esperar_token(TOKEN_PONTO_VIRGULA)) return 0;
    return 1;
}

int analisar_comando(const char* funcao_escopo) {
    switch (token_atual.tipo) {
        case TOKEN_LEIA:
            consumir_token();
            if (!esperar_token(TOKEN_PARENTESES_ESQ)) return 0;
            empilhar_delimitador('(', token_atual.linha - 1);

            /* Lista de variáveis */
            do {
                if (token_atual.tipo != TOKEN_ID_VARIAVEL) {
                    fprintf(stderr, "ERRO SINTÁTICO: Esperado nome de variável na linha %d.\n", token_atual.linha);
                    erro_sintatico_encontrado = 1;
                    return 0;
                }

                // Verificação semântica da variável
                verificar_variavel_declarada(token_atual.lexema, token_atual.linha);

                consumir_token();

                if (token_atual.tipo == TOKEN_VIRGULA) {
                    consumir_token();
                } else {
                    break;
                }
            } while (1);

            if (!esperar_token(TOKEN_PARENTESES_DIR)) return 0;
            if (!desempilhar_delimitador(')', token_atual.linha - 1)) return 0;

            if (!esperar_token(TOKEN_PONTO_VIRGULA)) return 0;
            break;

        case TOKEN_ESCREVA:
            consumir_token();
            if (!esperar_token(TOKEN_PARENTESES_ESQ)) return 0;
            empilhar_delimitador('(', token_atual.linha - 1);

            // Verifica se existem argumentos para serem analisados
            if (token_atual.tipo != TOKEN_PARENTESES_DIR) {
                do {
                    // Agora, qualquer expressão válida pode ser um argumento
                    if (!analisar_expressao()) return 0;

                    if (token_atual.tipo == TOKEN_VIRGULA) {
                        consumir_token();
                    } else {
                        break;
                    }
                } while (1);
            }

            if (!esperar_token(TOKEN_PARENTESES_DIR)) return 0;
            if (!desempilhar_delimitador(')', token_atual.linha - 1)) return 0;
            if (!esperar_token(TOKEN_PONTO_VIRGULA)) return 0;
            break;

        case TOKEN_SE:
            consumir_token();
            if (!esperar_token(TOKEN_PARENTESES_ESQ)) return 0;
            empilhar_delimitador('(', token_atual.linha - 1);

            if (!analisar_condicao()) return 0;

            if (!esperar_token(TOKEN_PARENTESES_DIR)) return 0;
            if (!desempilhar_delimitador(')', token_atual.linha - 1)) return 0;

            /* Verificar que não há ponto e vírgula após se(...) */
            if (!verificar_ausencia_token(TOKEN_PONTO_VIRGULA, "condição do 'se'")) return 0;

            /* Comando verdadeiro */
            if (token_atual.tipo == TOKEN_CHAVES_ESQ) {
                if (!analisar_bloco(funcao_escopo)) return 0;
            } else {
                if (!analisar_comando(funcao_escopo)) return 0;
            }

            /* Senao (opcional) */
            if (token_atual.tipo == TOKEN_SENAO) {
                consumir_token();
                if (token_atual.tipo == TOKEN_CHAVES_ESQ) {
                    if (!analisar_bloco(funcao_escopo)) return 0;
                } else {
                    if (!analisar_comando(funcao_escopo)) return 0;
                }
            }
            break;

        case TOKEN_PARA:
            consumir_token();
            if (!esperar_token(TOKEN_PARENTESES_ESQ)) return 0;
            empilhar_delimitador('(', token_atual.linha - 1);

            /* Inicialização */
            if (token_atual.tipo == TOKEN_ID_VARIAVEL) {
                consumir_token();
                if (!esperar_token(TOKEN_ATRIBUICAO)) return 0;
                if (!analisar_expressao()) return 0;
            }

            if (!esperar_token(TOKEN_PONTO_VIRGULA)) return 0;

            /* Condição */
            if (!analisar_condicao()) return 0;
            if (!esperar_token(TOKEN_PONTO_VIRGULA)) return 0;

            /* Incremento */
            if (token_atual.tipo == TOKEN_ID_VARIAVEL) {
                /* Variável seguida de atribuição ou incremento/decremento */
                char nome_var[256];
                strcpy(nome_var, token_atual.lexema);
                consumir_token();

                if (token_atual.tipo == TOKEN_ATRIBUICAO) {
                    consumir_token();
                    if (!analisar_expressao()) return 0;
                } else if (token_atual.tipo == TOKEN_INCREMENT || token_atual.tipo == TOKEN_DECREMENT) {
                    consumir_token(); /* Consome ++ ou -- */
                } else {
                    fprintf(stderr, "ERRO SINTÁTICO: Esperado atribuição ou incremento/decremento na terceira parte do 'para' na linha %d.\n", token_atual.linha);
                    erro_sintatico_encontrado = 1;
                    return 0;
                }
            } else if (token_atual.tipo == TOKEN_INCREMENT || token_atual.tipo == TOKEN_DECREMENT) {
                /* Incremento/decremento antes da variável */
                consumir_token();
                if (token_atual.tipo != TOKEN_ID_VARIAVEL) {
                    fprintf(stderr, "ERRO SINTÁTICO: Esperado nome de variável após incremento/decremento na linha %d.\n", token_atual.linha);
                    erro_sintatico_encontrado = 1;
                    return 0;
                }
                consumir_token();
            }

            if (!esperar_token(TOKEN_PARENTESES_DIR)) return 0;
            if (!desempilhar_delimitador(')', token_atual.linha - 1)) return 0;

            /* Verificar que não há ponto e vírgula após para(...) */
            if (!verificar_ausencia_token(TOKEN_PONTO_VIRGULA, "declaração do 'para'")) return 0;

            /* Corpo do laço */
            if (token_atual.tipo == TOKEN_CHAVES_ESQ) {
                if (!analisar_bloco(funcao_escopo)) return 0;
            } else {
                if (!analisar_comando(funcao_escopo)) return 0;
            }
            break;

        case TOKEN_RETORNO:
            consumir_token();
            if (!analisar_expressao()) return 0;
            if (!esperar_token(TOKEN_PONTO_VIRGULA)) return 0;
            break;

        case TOKEN_ID_VARIAVEL:
            /* Atribuição */
        {
            char nome_var[256];
            strcpy(nome_var, token_atual.lexema);
            int linha_atribuicao = token_atual.linha;
            consumir_token();
            if (!esperar_token(TOKEN_ATRIBUICAO)) return 0;

            // Captura informações do valor para análise semântica
            char valor[256];
            TipoToken tipo_valor = token_atual.tipo;
            strcpy(valor, token_atual.lexema);

            if (!analisar_expressao()) return 0;

            // Análise semântica da atribuição
            analisar_semantica_atribuicao(nome_var, valor, tipo_valor, linha_atribuicao);

            if (!esperar_token(TOKEN_PONTO_VIRGULA)) return 0;
        }
            break;

        case TOKEN_ID_FUNCAO:
            /* Chamada de função */
        {
            char nome_funcao[256];
            strcpy(nome_funcao, token_atual.lexema);
            int linha_chamada = token_atual.linha;
            consumir_token();

            // Verificação semântica da função
            verificar_funcao_declarada(nome_funcao, linha_chamada);

            if (!esperar_token(TOKEN_PARENTESES_ESQ)) return 0;
            empilhar_delimitador('(', token_atual.linha - 1);

            /* Parâmetros (opcional) */
            if (token_atual.tipo != TOKEN_PARENTESES_DIR) {
                do {
                    if (!analisar_expressao()) return 0;

                    if (token_atual.tipo == TOKEN_VIRGULA) {
                        consumir_token();
                    } else {
                        break;
                    }
                } while (1);
            }

            if (!esperar_token(TOKEN_PARENTESES_DIR)) return 0;
            if (!desempilhar_delimitador(')', token_atual.linha - 1)) return 0;
            if (!esperar_token(TOKEN_PONTO_VIRGULA)) return 0;
        }
            break;
    }
}
int analisar_bloco(const char* funcao_escopo) {
    if (token_atual.tipo != TOKEN_CHAVES_ESQ) {
         fprintf(stderr, "ERRO SINTÁTICO: Esperado '{' para iniciar o bloco na linha %d.\n", token_atual.linha);
         erro_sintatico_encontrado = 1;
         return 0;
    }
    empilhar_delimitador('{', token_atual.linha);
    consumir_token();

    while (token_atual.tipo != TOKEN_CHAVES_DIR && token_atual.tipo != TOKEN_FIM_DE_ARQUIVO && !erro_sintatico_encontrado) {
        if (token_atual.tipo == TOKEN_INTEIRO || token_atual.tipo == TOKEN_TEXTO || token_atual.tipo == TOKEN_DECIMAL) {
            if (!analisar_declaracao_variavel(funcao_escopo)) return 0;
        } else {
            if (!analisar_comando(funcao_escopo)) return 0;
        }
    }

    if (!esperar_token(TOKEN_CHAVES_DIR)) return 0;
    if (!desempilhar_delimitador('}', token_atual.linha - 1)) return 0;

    return 1;
}

int analisar_expressao() {
    /* Expressão: Termo ((+|-) Termo)* */
    if (!analisar_termo()) return 0;

    while (token_atual.tipo == TOKEN_OP_SOMA || token_atual.tipo == TOKEN_OP_SUBTRACAO) {
        consumir_token();
        if (!analisar_termo()) return 0;
    }

    return 1;
}

int analisar_termo() {
    /* Termo: Fator ((*|/|^) Fator)* */
    if (!analisar_fator()) return 0;

    while (token_atual.tipo == TOKEN_OP_MULTIPLICACAO ||
           token_atual.tipo == TOKEN_OP_DIVISAO ||
           token_atual.tipo == TOKEN_OP_EXPONENCIACAO) {
        consumir_token();
        if (!analisar_fator()) return 0;
    }

    return 1;
}

int analisar_fator() {
    /* Fator: NUMERO | VARIAVEL | TEXTO | FUNCAO(...) | (Expressao) */

    if (token_atual.tipo == TOKEN_LITERAL_NUMERO || token_atual.tipo == TOKEN_LITERAL_TEXTO) {
        consumir_token();
        return 1;
    }
    else if (token_atual.tipo == TOKEN_ID_VARIAVEL) {
        // Verificação semântica da variável
        verificar_variavel_declarada(token_atual.lexema, token_atual.linha);
        consumir_token();
        return 1;
    }
    else if (token_atual.tipo == TOKEN_ID_FUNCAO) {
        /* Chamada de função */
        char nome_funcao[256];
        strcpy(nome_funcao, token_atual.lexema);
        int linha_chamada = token_atual.linha;
        consumir_token();

        // Verificação semântica da função
        verificar_funcao_declarada(nome_funcao, linha_chamada);

        if (!esperar_token(TOKEN_PARENTESES_ESQ)) return 0;
        empilhar_delimitador('(', token_atual.linha - 1);

        /* Parâmetros (opcional) */
        if (token_atual.tipo != TOKEN_PARENTESES_DIR) {
            do {
                if (!analisar_expressao()) return 0;

                if (token_atual.tipo == TOKEN_VIRGULA) {
                    consumir_token();
                } else {
                    break;
                }
            } while (1);
        }

        if (!esperar_token(TOKEN_PARENTESES_DIR)) return 0;
        if (!desempilhar_delimitador(')', token_atual.linha - 1)) return 0;
        return 1;
    }
    else if (token_atual.tipo == TOKEN_PARENTESES_ESQ) {
        /* (Expressão) */
        empilhar_delimitador('(', token_atual.linha);
        consumir_token();

        if (!analisar_expressao()) return 0;

        if (!esperar_token(TOKEN_PARENTESES_DIR)) return 0;
        if (!desempilhar_delimitador(')', token_atual.linha - 1)) return 0;
        return 1;
    }
    else {
        fprintf(stderr, "ERRO SINTÁTICO: Fator inválido '%s' na linha %d.\n",
                token_atual.lexema, token_atual.linha);
        erro_sintatico_encontrado = 1;
        return 0;
    }
}


int analisar_condicao() {
    /* Captura informações do primeiro operando */
    char operando1[256];
    TipoToken tipo_operando1 = token_atual.tipo;
    strcpy(operando1, token_atual.lexema);
    int linha_comparacao = token_atual.linha;

    /* Primeira expressão */
    if (!analisar_expressao()) return 0;

    /* Operador relacional */
    if (token_atual.tipo == TOKEN_OP_IGUAL || token_atual.tipo == TOKEN_OP_DIFERENTE ||
        token_atual.tipo == TOKEN_OP_MENOR || token_atual.tipo == TOKEN_OP_MENOR_IGUAL ||
        token_atual.tipo == TOKEN_OP_MAIOR || token_atual.tipo == TOKEN_OP_MAIOR_IGUAL) {

        char operador[4];
        strcpy(operador, token_atual.lexema);
        consumir_token();

        /* Captura informações do segundo operando */
        char operando2[256];
        TipoToken tipo_operando2 = token_atual.tipo;
        strcpy(operando2, token_atual.lexema);

        /* Segunda expressão */
        if (!analisar_expressao()) return 0;

        /* Verificação semântica da comparação */
        analisar_semantica_comparacao(operando1, tipo_operando1, operando2, tipo_operando2, operador, linha_comparacao);

        /* Operadores lógicos (opcional) */
        while (token_atual.tipo == TOKEN_OP_E || token_atual.tipo == TOKEN_OP_OU) {
            consumir_token();

            /* Captura próximo operando */
            strcpy(operando1, token_atual.lexema);
            tipo_operando1 = token_atual.tipo;
            linha_comparacao = token_atual.linha;

            /* Nova condição relacional */
            if (!analisar_expressao()) return 0;

            if (token_atual.tipo == TOKEN_OP_IGUAL || token_atual.tipo == TOKEN_OP_DIFERENTE ||
                token_atual.tipo == TOKEN_OP_MENOR || token_atual.tipo == TOKEN_OP_MENOR_IGUAL ||
                token_atual.tipo == TOKEN_OP_MAIOR || token_atual.tipo == TOKEN_OP_MAIOR_IGUAL) {

                strcpy(operador, token_atual.lexema);
                consumir_token();

                strcpy(operando2, token_atual.lexema);
                tipo_operando2 = token_atual.tipo;

                if (!analisar_expressao()) return 0;

                /* Verificação semântica da comparação */
                analisar_semantica_comparacao(operando1, tipo_operando1, operando2, tipo_operando2, operador, linha_comparacao);

            } else {
                fprintf(stderr, "ERRO SINTÁTICO: Esperado operador relacional após operador lógico na linha %d.\n", token_atual.linha);
                erro_sintatico_encontrado = 1;
                return 0;
            }
        }

        return 1;
    } else {
        fprintf(stderr, "ERRO SINTÁTICO: Esperado operador relacional na condição na linha %d.\n", token_atual.linha);
        erro_sintatico_encontrado = 1;
        return 0;
    }
}