/**
 * @author Heitor Barreto e Vinícius Lopes
 * @date Julho de 2025
 */

#ifndef COMPILADOR_H
#define COMPILADOR_H

#include <stdio.h>

/* --- CONTROLE DE MEMORIA --- */

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

/* --- ANALISADOR LEXICO --- */

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
    TOKEN_OP_E, TOKEN_OP_OU, TOKEN_ATRIBUICAO, TOKEN_INCREMENT, TOKEN_DECREMENT,
    TOKEN_PARENTESES_ESQ, TOKEN_PARENTESES_DIR, TOKEN_CHAVES_ESQ, TOKEN_CHAVES_DIR,
    TOKEN_COLCHETES_ESQ, TOKEN_COLCHETES_DIR, TOKEN_PONTO_VIRGULA, TOKEN_VIRGULA,
    TOKEN_PONTO, TOKEN_FIM_DE_ARQUIVO, TOKEN_ERRO
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

extern FILE* arquivo_fonte; /* Ponteiro para o arquivo de código-fonte sendo analisado. */
extern int linha_atual;     /* Contador da linha atual no arquivo-fonte. */

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

/* --- TABELA DE SÍMBOLOS --- */

/**
 * @enum TipoDado
 * @brief Tipos de dados suportados pela linguagem.
 */
typedef enum {
    TIPO_INTEIRO,
    TIPO_TEXTO,
    TIPO_DECIMAL
} TipoDado;

/**
 * @struct LimitadorTamanho
 * @brief Estrutura para armazenar limitadores de tamanho para texto e decimal.
 */
typedef struct {
    int tamanho1;  /* Para texto: tamanho total. Para decimal: casas antes do ponto */
    int tamanho2;  /* Para decimal: casas depois do ponto. Para texto: não usado */
} LimitadorTamanho;

/**
 * @struct EntradaTabela
 * @brief Entrada individual na tabela de símbolos.
 */
typedef struct EntradaTabela {
    char* nome;
    TipoDado tipo;
    char* valor;
    char* funcao_escopo;
    LimitadorTamanho limitador;
    int tem_limitador;
    struct EntradaTabela* proxima;
} EntradaTabela;

/**
 * @struct TabelaSimbolos
 * @brief Tabela de símbolos implementada como lista ligada.
 */
typedef struct {
    EntradaTabela* primeira;
    int total_entradas;
} TabelaSimbolos;

extern TabelaSimbolos* tabela_simbolos;

/**
 * @brief Inicializa a tabela de símbolos.
 */
void inicializar_tabela_simbolos();

/**
 * @brief Adiciona uma variável na tabela de símbolos.
 * @param nome Nome da variável
 * @param tipo Tipo da variável
 * @param funcao_escopo Nome da função onde foi declarada
 * @param limitador Limitadores de tamanho (se aplicável)
 * @param tem_limitador Se tem limitadores definidos
 */
void adicionar_variavel(const char* nome, TipoDado tipo, const char* funcao_escopo,
                       LimitadorTamanho limitador, int tem_limitador);

/**
 * @brief Busca uma variável na tabela de símbolos.
 * @param nome Nome da variável a buscar
 * @return Ponteiro para a entrada ou NULL se não encontrada
 */
EntradaTabela* buscar_variavel(const char* nome);

/**
 * @brief Exibe o conteúdo da tabela de símbolos.
 */
void exibir_tabela_simbolos();

/**
 * @brief Libera toda a memória da tabela de símbolos.
 */
void destruir_tabela_simbolos();

/* --- ANALISADOR SINTÁTICO --- */

extern Token token_atual;
extern int erro_sintatico_encontrado;

/**
 * @brief Inicializa o analisador sintático.
 */
void inicializar_parser();

/**
 * @brief Consome o token atual e avança para o próximo.
 */
void consumir_token();

/**
 * @brief Verifica se o token atual é do tipo esperado e consome.
 * @param tipo_esperado Tipo de token esperado
 * @return 1 se correto, 0 se erro
 */
int esperar_token(TipoToken tipo_esperado);

/**
 * @brief Inicia a análise sintática do programa.
 * @return 1 se análise bem-sucedida, 0 se erro
 */
int analisar_programa();

/**
 * @brief Analisa uma declaração de função.
 * @return 1 se bem-sucedida, 0 se erro
 */
int analisar_funcao();

/**
 * @brief Analisa uma declaração de variável.
 * @param funcao_escopo Nome da função atual
 * @return 1 se bem-sucedida, 0 se erro
 */
int analisar_declaracao_variavel(const char* funcao_escopo);

/**
 * @brief Analisa um comando (leia, escreva, se, para, etc.).
 * @param funcao_escopo Nome da função atual
 * @return 1 se bem-sucedida, 0 se erro
 */
int analisar_comando(const char* funcao_escopo);

/**
 * @brief Analisa um bloco de comandos entre chaves.
 * @param funcao_escopo Nome da função atual
 * @return 1 se bem-sucedida, 0 se erro
 */
int analisar_bloco(const char* funcao_escopo);

/**
 * @brief Analisa uma expressão matemática completa (precedência baixa: + e -).
 * @return 1 se bem-sucedida, 0 se erro
 */
int analisar_expressao();

/**
 * @brief Analisa um termo matemático (precedência média: *, /, ^).
 * @return 1 se bem-sucedida, 0 se erro
 */
int analisar_termo();

/**
 * @brief Analisa um fator matemático (precedência alta: números, variáveis, parênteses).
 * @return 1 se bem-sucedida, 0 se erro
 */
int analisar_fator();

/**
 * @brief Analisa uma condição (para se, para).
 * @return 1 se bem-sucedida, 0 se erro
 */
int analisar_condicao();

/**
 * @brief Verifica balanceamento de delimitadores.
 */
typedef struct {
    char delimitador;
    int linha;
} ItemBalanceamento;

typedef struct {
    ItemBalanceamento* itens;
    int topo;
    int capacidade;
} PilhaBalanceamento;

extern PilhaBalanceamento* pilha_balanceamento;

/**
 * @brief Inicializa a pilha de balanceamento.
 */
void inicializar_pilha_balanceamento();

/**
 * @brief Empilha um delimitador para verificação de balanceamento.
 * @param delimitador Caractere delimitador
 * @param linha Linha onde foi encontrado
 */
void empilhar_delimitador(char delimitador, int linha);

/**
 * @brief Desempilha e verifica balanceamento.
 * @param delimitador_fechamento Delimitador de fechamento
 * @param linha Linha atual
 * @return 1 se balanceado, 0 se erro
 */
int desempilhar_delimitador(char delimitador_fechamento, int linha);

/**
 * @brief Analisa um comando de incremento/decremento (++, --).
 * @param funcao_escopo Nome da função atual
 * @return 1 se bem-sucedida, 0 se erro
 */
int analisar_incremento_decremento(const char* funcao_escopo);

/**
 * @brief Verifica se um token não deve estar presente (ex: ponto e vírgula após se/para).
 * @param token_nao_esperado Tipo de token que não deveria estar presente
 * @param contexto Descrição do contexto para mensagem de erro
 * @return 1 se correto (token não presente), 0 se erro
 */
int verificar_ausencia_token(TipoToken token_nao_esperado, const char* contexto);

/**
 * @brief Libera a pilha de balanceamento.
 */
void destruir_pilha_balanceamento();

/* --- ANALISADOR SEMÂNTICO --- */

extern int erro_semantico_encontrado;
extern int alerta_semantico_emitido;

/**
 * @brief Inicializa o analisador semântico.
 */
void inicializar_analisador_semantico();

/**
 * @brief Libera recursos do analisador semântico.
 */
void destruir_analisador_semantico();

/**
 * @brief Adiciona uma função à tabela de funções declaradas.
 * @param nome Nome da função
 * @param linha Linha onde foi declarada
 */
void adicionar_funcao_declarada(const char* nome, int linha);

/**
 * @brief Marca uma função como chamada.
 * @param nome Nome da função
 */
void marcar_funcao_chamada(const char* nome);

/**
 * @brief Verifica se uma variável foi declarada.
 * @param nome_variavel Nome da variável
 * @param linha Linha atual
 * @return 1 se declarada, 0 caso contrário
 */
int verificar_variavel_declarada(const char* nome_variavel, int linha);

/**
 * @brief Verifica se uma função foi declarada.
 * @param nome_funcao Nome da função
 * @param linha Linha atual
 * @return 1 se declarada, 0 caso contrário
 */
int verificar_funcao_declarada(const char* nome_funcao, int linha);

/**
 * @brief Verifica compatibilidade de tipos em atribuição.
 * @param nome_variavel Nome da variável
 * @param tipo_valor Tipo do valor sendo atribuído
 * @param linha Linha atual
 * @return 1 se compatível, 0 caso contrário
 */
int verificar_atribuicao_tipos(const char* nome_variavel, TipoDado tipo_valor, int linha);

/**
 * @brief Verifica compatibilidade de tipos em comparação.
 * @param tipo1 Tipo do primeiro operando
 * @param tipo2 Tipo do segundo operando
 * @param operador Operador de comparação
 * @param linha Linha atual
 * @return 1 se compatível, 0 caso contrário
 */
int verificar_comparacao_tipos(TipoDado tipo1, TipoDado tipo2, const char* operador, int linha);

/**
 * @brief Analisa semânticamente uma atribuição.
 * @param nome_variavel Nome da variável
 * @param valor Valor sendo atribuído
 * @param tipo_valor Tipo do token do valor
 * @param linha Linha atual
 */
void analisar_semantica_atribuicao(const char* nome_variavel, const char* valor, TipoToken tipo_valor, int linha);

/**
 * @brief Analisa semânticamente uma comparação.
 * @param operando1 Primeiro operando
 * @param tipo1 Tipo do primeiro operando
 * @param operando2 Segundo operando
 * @param tipo2 Tipo do segundo operando
 * @param operador Operador de comparação
 * @param linha Linha atual
 */
void analisar_semantica_comparacao(const char* operando1, TipoToken tipo1,
                                   const char* operando2, TipoToken tipo2,
                                   const char* operador, int linha);

/**
 * @brief Verifica funções não utilizadas e exibe relatório.
 */
void exibir_relatorio_semantico();

#endif