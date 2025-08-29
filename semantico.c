/**
 * @author Heitor Barreto e Vinícius Lopes
 * @date Agosto de 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compilador.h"

/* --- VARIÁVEIS GLOBAIS SEMÂNTICAS --- */
int erro_semantico_encontrado = 0;
int alerta_semantico_emitido = 0;

/* --- ESTRUTURAS PARA ANÁLISE SEMÂNTICA --- */

typedef struct {
    char* nome_funcao;
    int linha_declaracao;
    int foi_chamada;
    struct FuncaoDeclarada* proxima;
} FuncaoDeclarada;

typedef struct {
    FuncaoDeclarada* primeira;
    int total_funcoes;
} TabelaFuncoes;

static TabelaFuncoes* tabela_funcoes = NULL;

/* --- INICIALIZAÇÃO --- */

void inicializar_analisador_semantico() {
    erro_semantico_encontrado = 0;
    alerta_semantico_emitido = 0;

    tabela_funcoes = (TabelaFuncoes*) alocar_memoria(sizeof(TabelaFuncoes));
    tabela_funcoes->primeira = NULL;
    tabela_funcoes->total_funcoes = 0;
}

void destruir_analisador_semantico() {
    if (tabela_funcoes) {
        FuncaoDeclarada* atual = tabela_funcoes->primeira;
        while (atual != NULL) {
            FuncaoDeclarada* proxima = atual->proxima;
            liberar_memoria(atual->nome_funcao, strlen(atual->nome_funcao) + 1);
            liberar_memoria(atual, sizeof(FuncaoDeclarada));
            atual = proxima;
        }
        liberar_memoria(tabela_funcoes, sizeof(TabelaFuncoes));
    }
}

/* --- FUNÇÕES AUXILIARES --- */

void adicionar_funcao_declarada(const char* nome, int linha) {
    FuncaoDeclarada* nova = (FuncaoDeclarada*) alocar_memoria(sizeof(FuncaoDeclarada));

    size_t len_nome = strlen(nome) + 1;
    nova->nome_funcao = (char*) alocar_memoria(len_nome);
    strncpy(nova->nome_funcao, nome, len_nome);

    nova->linha_declaracao = linha;
    nova->foi_chamada = 0;
    nova->proxima = tabela_funcoes->primeira;

    tabela_funcoes->primeira = nova;
    tabela_funcoes->total_funcoes++;
}

FuncaoDeclarada* buscar_funcao_declarada(const char* nome) {
    FuncaoDeclarada* atual = tabela_funcoes->primeira;
    while (atual != NULL) {
        if (strcmp(atual->nome_funcao, nome) == 0) {
            return atual;
        }
        atual = atual->proxima;
    }
    return NULL;
}

void marcar_funcao_chamada(const char* nome) {
    FuncaoDeclarada* funcao = buscar_funcao_declarada(nome);
    if (funcao) {
        funcao->foi_chamada = 1;
    }
}

const char* tipo_para_string(TipoDado tipo) {
    switch (tipo) {
        case TIPO_INTEIRO: return "inteiro";
        case TIPO_TEXTO: return "texto";
        case TIPO_DECIMAL: return "decimal";
        default: return "desconhecido";
    }
}

TipoDado inferir_tipo_literal(const char* lexema) {
    if (lexema[0] == '"') {
        return TIPO_TEXTO;
    }

    // Verifica se contém ponto decimal
    if (strchr(lexema, '.') != NULL) {
        return TIPO_DECIMAL;
    }

    return TIPO_INTEIRO;
}

int tipos_compativeis_atribuicao(TipoDado tipo_variavel, TipoDado tipo_valor) {
    return tipo_variavel == tipo_valor;
}

int tipos_compativeis_comparacao(TipoDado tipo1, TipoDado tipo2) {
    // Para texto, apenas == e <> são válidos
    if (tipo1 == TIPO_TEXTO || tipo2 == TIPO_TEXTO) {
        return tipo1 == tipo2; // Ambos devem ser texto
    }

    // Para números (inteiro e decimal), todos operadores são válidos
    return (tipo1 == TIPO_INTEIRO || tipo1 == TIPO_DECIMAL) &&
           (tipo2 == TIPO_INTEIRO || tipo2 == TIPO_DECIMAL);
}

/* --- VERIFICAÇÕES SEMÂNTICAS --- */

int verificar_variavel_declarada(const char* nome_variavel, int linha) {
    EntradaTabela* entrada = buscar_variavel(nome_variavel);
    if (entrada == NULL) {
        fprintf(stderr, "ALERTA SEMÂNTICO: Variável '%s' não foi declarada (linha %d).\n",
                nome_variavel, linha);
        alerta_semantico_emitido = 1;
        return 0;
    }
    return 1;
}

int verificar_funcao_declarada(const char* nome_funcao, int linha) {
    FuncaoDeclarada* funcao = buscar_funcao_declarada(nome_funcao);
    if (funcao == NULL) {
        fprintf(stderr, "ALERTA SEMÂNTICO: Função '%s' não foi declarada (linha %d).\n",
                nome_funcao, linha);
        alerta_semantico_emitido = 1;
        return 0;
    }

    marcar_funcao_chamada(nome_funcao);
    return 1;
}

int verificar_atribuicao_tipos(const char* nome_variavel, TipoDado tipo_valor, int linha) {
    EntradaTabela* entrada = buscar_variavel(nome_variavel);
    if (entrada == NULL) {
        return verificar_variavel_declarada(nome_variavel, linha);
    }

    if (!tipos_compativeis_atribuicao(entrada->tipo, tipo_valor)) {
        fprintf(stderr, "ALERTA SEMÂNTICO: Incompatibilidade de tipos na atribuição (linha %d). "
                        "Variável '%s' é do tipo '%s', mas está recebendo valor do tipo '%s'.\n",
                linha, nome_variavel, tipo_para_string(entrada->tipo), tipo_para_string(tipo_valor));
        alerta_semantico_emitido = 1;
        return 0;
    }

    return 1;
}

int verificar_comparacao_tipos(TipoDado tipo1, TipoDado tipo2, const char* operador, int linha) {
    if (!tipos_compativeis_comparacao(tipo1, tipo2)) {
        if (tipo1 == TIPO_TEXTO || tipo2 == TIPO_TEXTO) {
            fprintf(stderr, "ALERTA SEMÂNTICO: Operador '%s' não pode ser usado para comparar texto com número (linha %d).\n",
                    operador, linha);
        } else {
            fprintf(stderr, "ALERTA SEMÂNTICO: Tipos incompatíveis na comparação '%s' vs '%s' com operador '%s' (linha %d).\n",
                    tipo_para_string(tipo1), tipo_para_string(tipo2), operador, linha);
        }
        alerta_semantico_emitido = 1;
        return 0;
    }

    // Verifica se operador é válido para texto
    if ((tipo1 == TIPO_TEXTO || tipo2 == TIPO_TEXTO) &&
        strcmp(operador, "==") != 0 && strcmp(operador, "<>") != 0) {
        fprintf(stderr, "ALERTA SEMÂNTICO: Operador '%s' não é válido para tipo texto. "
                        "Use apenas '==' ou '<>' (linha %d).\n", operador, linha);
        alerta_semantico_emitido = 1;
        return 0;
    }

    return 1;
}

int verificar_operacao_matematica_tipos(TipoDado tipo1, TipoDado tipo2, const char* operador, int linha) {
    if (tipo1 == TIPO_TEXTO || tipo2 == TIPO_TEXTO) {
        fprintf(stderr, "ALERTA SEMÂNTICO: Operador matemático '%s' não pode ser usado com tipo texto (linha %d).\n",
                operador, linha);
        alerta_semantico_emitido = 1;
        return 0;
    }

    return 1;
}

int verificar_limitadores_texto(const char* nome_variavel, const char* valor_texto, int linha) {
    EntradaTabela* entrada = buscar_variavel(nome_variavel);
    if (entrada == NULL || entrada->tipo != TIPO_TEXTO || !entrada->tem_limitador) {
        return 1; // Sem limitador ou não é texto
    }

    // Remove aspas para contar caracteres
    int tamanho_valor = strlen(valor_texto) - 2; // Remove as aspas duplas
    if (tamanho_valor > entrada->limitador.tamanho1) {
        fprintf(stderr, "ALERTA SEMÂNTICO: Texto atribuído à variável '%s' excede o tamanho máximo de %d caracteres (linha %d).\n",
                nome_variavel, entrada->limitador.tamanho1, linha);
        alerta_semantico_emitido = 1;
        return 0;
    }

    return 1;
}

int verificar_limitadores_decimal(const char* nome_variavel, const char* valor_decimal, int linha) {
    EntradaTabela* entrada = buscar_variavel(nome_variavel);
    if (entrada == NULL || entrada->tipo != TIPO_DECIMAL || !entrada->tem_limitador) {
        return 1; // Sem limitador ou não é decimal
    }

    char* ponto = strchr(valor_decimal, '.');
    int casas_antes = ponto ? (ponto - valor_decimal) : strlen(valor_decimal);
    int casas_depois = ponto ? strlen(ponto + 1) : 0;

    if (casas_antes > entrada->limitador.tamanho1) {
        fprintf(stderr, "ALERTA SEMÂNTICO: Valor decimal para variável '%s' possui %d casas antes do ponto, "
                        "mas o limite é %d (linha %d).\n",
                nome_variavel, casas_antes, entrada->limitador.tamanho1, linha);
        alerta_semantico_emitido = 1;
        return 0;
    }

    if (casas_depois > entrada->limitador.tamanho2) {
        fprintf(stderr, "ALERTA SEMÂNTICO: Valor decimal para variável '%s' possui %d casas depois do ponto, "
                        "mas o limite é %d (linha %d).\n",
                nome_variavel, casas_depois, entrada->limitador.tamanho2, linha);
        alerta_semantico_emitido = 1;
        return 0;
    }

    return 1;
}

void verificar_funcoes_nao_utilizadas() {
    FuncaoDeclarada* atual = tabela_funcoes->primeira;
    while (atual != NULL) {
        if (!atual->foi_chamada && strcmp(atual->nome_funcao, "principal") != 0) {
            fprintf(stderr, "ALERTA SEMÂNTICO: Função '%s' foi declarada na linha %d mas nunca foi utilizada.\n",
                    atual->nome_funcao, atual->linha_declaracao);
            alerta_semantico_emitido = 1;
        }
        atual = atual->proxima;
    }
}

/* --- FUNÇÕES DE ANÁLISE SEMÂNTICA --- */

TipoDado analisar_tipo_expressao(Token* tokens, int inicio, int fim) {
    // Esta função seria mais complexa em uma implementação completa
    // Por simplicidade, vamos inferir o tipo baseado no primeiro token relevante

    for (int i = inicio; i <= fim; i++) {
        if (tokens[i].tipo == TOKEN_LITERAL_NUMERO) {
            return inferir_tipo_literal(tokens[i].lexema);
        } else if (tokens[i].tipo == TOKEN_LITERAL_TEXTO) {
            return TIPO_TEXTO;
        } else if (tokens[i].tipo == TOKEN_ID_VARIAVEL) {
            EntradaTabela* entrada = buscar_variavel(tokens[i].lexema);
            if (entrada) {
                return entrada->tipo;
            }
        }
    }

    return TIPO_INTEIRO; // Tipo padrão se não conseguir inferir
}

void analisar_semantica_atribuicao(const char* nome_variavel, const char* valor, TipoToken tipo_valor, int linha) {
    TipoDado tipo_inferido;

    if (tipo_valor == TOKEN_LITERAL_TEXTO) {
        tipo_inferido = TIPO_TEXTO;
        verificar_limitadores_texto(nome_variavel, valor, linha);
    } else if (tipo_valor == TOKEN_LITERAL_NUMERO) {
        tipo_inferido = inferir_tipo_literal(valor);
        if (tipo_inferido == TIPO_DECIMAL) {
            verificar_limitadores_decimal(nome_variavel, valor, linha);
        }
    } else if (tipo_valor == TOKEN_ID_VARIAVEL) {
        EntradaTabela* entrada = buscar_variavel(valor);
        if (entrada) {
            tipo_inferido = entrada->tipo;
        } else {
            verificar_variavel_declarada(valor, linha);
            return;
        }
    } else {
        tipo_inferido = TIPO_INTEIRO; // Padrão para expressões
    }

    verificar_atribuicao_tipos(nome_variavel, tipo_inferido, linha);
}

void analisar_semantica_comparacao(const char* operando1, TipoToken tipo1,
                                   const char* operando2, TipoToken tipo2,
                                   const char* operador, int linha) {
    TipoDado tipo_op1, tipo_op2;

    // Determina tipo do primeiro operando
    if (tipo1 == TOKEN_LITERAL_TEXTO) {
        tipo_op1 = TIPO_TEXTO;
    } else if (tipo1 == TOKEN_LITERAL_NUMERO) {
        tipo_op1 = inferir_tipo_literal(operando1);
    } else if (tipo1 == TOKEN_ID_VARIAVEL) {
        EntradaTabela* entrada = buscar_variavel(operando1);
        if (entrada) {
            tipo_op1 = entrada->tipo;
        } else {
            verificar_variavel_declarada(operando1, linha);
            return;
        }
    } else {
        tipo_op1 = TIPO_INTEIRO;
    }

    // Determina tipo do segundo operando
    if (tipo2 == TOKEN_LITERAL_TEXTO) {
        tipo_op2 = TIPO_TEXTO;
    } else if (tipo2 == TOKEN_LITERAL_NUMERO) {
        tipo_op2 = inferir_tipo_literal(operando2);
    } else if (tipo2 == TOKEN_ID_VARIAVEL) {
        EntradaTabela* entrada = buscar_variavel(operando2);
        if (entrada) {
            tipo_op2 = entrada->tipo;
        } else {
            verificar_variavel_declarada(operando2, linha);
            return;
        }
    } else {
        tipo_op2 = TIPO_INTEIRO;
    }

    verificar_comparacao_tipos(tipo_op1, tipo_op2, operador, linha);
}

void exibir_relatorio_semantico() {
    printf("\n------------- RELATÓRIO SEMÂNTICO -------------\n");

    if (!alerta_semantico_emitido && !erro_semantico_encontrado) {
        printf("✓ Análise semântica concluída sem alertas ou erros.\n");
    } else {
        if (alerta_semantico_emitido) {
            printf("⚠ Alertas semânticos foram emitidos durante a análise.\n");
        }
        if (erro_semantico_encontrado) {
            printf("✗ Erros semânticos foram encontrados.\n");
        }
    }

    // Verifica funções não utilizadas
    verificar_funcoes_nao_utilizadas();

    printf("Total de funções declaradas: %d\n", tabela_funcoes->total_funcoes);
    printf("----------------------------------------------\n");
}