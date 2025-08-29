# Compilador (Analisador Léxico, Sintático e Semântico)

Este projeto implementa um analisador léxico, sintático e semântico em C para uma linguagem de programação simples, incluindo um subsistema de controle de memória dinâmico.

## 📝 Descrição Geral

O compilador realiza a análise de um código-fonte em três fases principais:

1.  **Análise Léxica:** Lê um arquivo-fonte (`codigo_fonte.txt`) e segmenta o texto em *tokens* (palavras reservadas, identificadores, operadores, etc.).
2.  **Análise Sintática:** Consome os tokens gerados pela análise léxica para verificar se a estrutura do programa obedece às regras gramaticais da linguagem.
3.  **Análise Semântica:** Verifica o "significado" do código sintaticamente correto, checando regras de tipo, escopo e uso de variáveis e funções.

### Funcionalidades do Analisador Léxico

  - Identifica literais numéricos (inteiros e decimais) e literais de texto.
  - Reconhece operadores matemáticos (`+`, `-`, `*`, `/`, `^`), relacionais (`==`, `<>`, `<`, `<=`, `>`, `>=`), lógicos (`&&`, `||`) e atribuição (`=`).
  - Detecta pontuação: parênteses `()`, chaves `{}`, colchetes `[]`, ponto e vírgula e vírgula.
  - Reconhece palavras reservadas: `principal`, `funcao`, `retorno`, `leia`, `escreva`, `se`, `senao`, `para`, `inteiro`, `texto`, `decimal`.
  - Identifica *identificadores* de variáveis (prefixo `!`) e funções (prefixo `__`).
  - Gera mensagens de erro léxico detalhadas com o número da linha em casos de lexemas malformados ou caracteres não reconhecidos.

### Funcionalidades do Analisador Sintático

  - Valida a estrutura geral do programa, verificando a existência de uma função `principal` obrigatória.
  - Analisa a sintaxe de declarações de funções (`funcao __nome(...)`) e de variáveis (`tipo !nome;`), incluindo múltiplos declaradores e limitadores de tamanho.
  - Verifica a correta formação de comandos como `leia`, `escreva`, `se`/`senao` e `para`.
  - Realiza o **balanceamento de delimitadores** (`()`, `{}`, `[]`, `""`) para garantir que todos sejam abertos e fechados corretamente.
  - Constrói e exibe uma **Tabela de Símbolos** com todas as variáveis declaradas, seus tipos e escopos.
  - Gera mensagens de erro sintático com o número da linha e o tipo de token esperado quando uma regra gramatical é violada.

### Funcionalidades do Analisador Semântico

  - Verifica se variáveis e funções foram declaradas antes de serem utilizadas.
  - Emite **alertas** para incompatibilidade de tipos em atribuições (ex: atribuir um `texto` a um `inteiro`).
  - Emite **alertas** para incompatibilidade de tipos em comparações (ex: usar `>` para comparar `texto` com `inteiro`).
  - Valida se os valores atribuídos a `texto` e `decimal` respeitam os limitadores de tamanho definidos.
  - Gera um relatório final indicando se foram encontrados erros ou alertas semânticos, incluindo funções declaradas que nunca foram utilizadas.

## 💾 Controle de Memória

  - Aloca memória dinamicamente via `alocar_memoria(size_t)` e libera com `liberar_memoria(ptr, size)`.
  - Monitora o uso atual e o pico de memória utilizada durante a execução.
  - Limite configurável em **2048 KB** (via `#define MEMORIA_MAXIMA_KB`).
  - Emite um **alerta** quando o uso de memória ultrapassa 90% da capacidade.
  - Interrompe a execução com erro fatal caso a alocação exceda o limite.
  - Ao final, exibe um relatório de consumo: total disponível, pico utilizado e memória restante.

## ⚙️ Estrutura dos Arquivos

  - `compilador.c`: Implementação do **analisador léxico**.
  - `parser.c`: Implementação do **analisador sintático**.
  - `semantico.c`: Implementação do **analisador semântico**.
  - `compilador.h`: Declaração de todas as funções, tipos de token e estruturas de dados do projeto.
  - `main.c`: Programa principal que inicializa e chama as fases de análise.
  - `codigo_fonte.txt`: Arquivo de entrada com o código da linguagem a ser analisado.

## 📥 Como Compilar

No Linux (gcc) ou Windows (Dev-C++ / Code::Blocks), inclua todos os arquivos `.c` no comando de compilação:

```bash
gcc -o compilador main.c compilador.c parser.c semantico.c
```

## ▶️ Como Executar

1.  Coloque o código-fonte a ser analisado no arquivo `codigo_fonte.txt`.
2.  Execute o programa compilado:
    ```bash
    ./compilador
    ```
3.  O programa exibirá o resultado das análises léxica, sintática e semântica. Se não houver erros fatais, mostrará a tabela de símbolos, o relatório semântico e, ao final, o relatório de memória.

## 📄 Licença

Distribuído sob a licença GNU GENERAL PUBLIC LICENSE.