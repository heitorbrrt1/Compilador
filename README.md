# Compilador (Analisador Léxico e Sintático)

Este projeto implementa um analisador léxico e sintático em C para uma linguagem de programação simples, incluindo um subsistema de controle de memória dinâmico.

## 📝 Descrição Geral

O compilador realiza a análise de um código-fonte em duas fases principais:

1.  **Análise Léxica:** Lê um arquivo-fonte (`codigo_fonte.txt`) e segmenta o texto em *tokens* (palavras reservadas, identificadores, operadores, etc.).
2.  **Análise Sintática:** Consome os tokens gerados pela análise léxica para verificar se a estrutura do programa obedece às regras gramaticais da linguagem.

### Funcionalidades do Analisador Léxico

- Identifica literais numéricos (inteiros e decimais) e literais de texto.
- Reconhece operadores matemáticos (`+`, `-`, `*`, `/`, `^`), relacionais (`==`, `<>`, `<`, `<=`, `>`, `>=`), lógicos (`&&`, `||`) e atribuição (`=`).
- Detecta pontuação: parênteses `()`, chaves `{}`, colchetes `[]`, ponto e vírgula e vírgula.
- Reconhece palavras reservadas: `principal`, `funcao`, `retorno`, `leia`, `escreva`, `se`, `senao`, `para`, `inteiro`, `texto`, `decimal`.
- Identifica *identificadores* de variáveis (prefixo `!`) e funções (prefixo `__`).
- Gera mensagens de erro léxico com número da linha em casos de lexemas malformados ou caracteres não reconhecidos.

### Funcionalidades do Analisador Sintático

- [cite\_start]Valida a estrutura geral do programa, verificando a existência de uma função `principal` obrigatória[cite: 2].
- Analisa a sintaxe de declarações de funções (`funcao __nome(...)`) e de variáveis (`tipo !nome;`), incluindo múltiplos declaradores na mesma linha e limitadores de tamanho para `texto` e `decimal`.
- Verifica a correta formação de comandos como `leia`, `escreva`, `se`/`senao` e `para`.
- [cite\_start]Realiza o **balanceamento de delimitadores** (`()`, `{}`, `[]`) para garantir que todos sejam abertos e fechados corretamente[cite: 2].
- [cite\_start]Constrói e exibe uma **Tabela de Símbolos** com todas as variáveis declaradas, seus tipos e escopos[cite: 2].
- [cite\_start]Gera mensagens de erro sintático com o número da linha e o tipo de token esperado quando uma regra gramatical é violada[cite: 2].

## 💾 Controle de Memória

- [cite\_start]Aloca memória dinamicamente via `alocar_memoria(size_t)` e libera com `liberar_memoria(ptr, size)`[cite: 1, 2].
- [cite\_start]Monitora o uso atual e o pico de memória utilizada durante a execução[cite: 1].
- [cite\_start]Limite configurável em **2048 KB** (via `#define MEMORIA_MAXIMA_KB`)[cite: 1, 2].
- [cite\_start]Emite um **alerta** quando o uso de memória ultrapassa 90% da capacidade[cite: 1].
- [cite\_start]Interrompe a execução com erro fatal caso a alocação exceda o limite[cite: 1].
- [cite\_start]Ao final, exibe um relatório de consumo: total disponível, pico utilizado e memória restante[cite: 1].

## ⚙️ Estrutura dos Arquivos

- `compilador.c`: Implementação do **analisador léxico**.
- `parser.c`: Implementação do **analisador sintático**, da tabela de símbolos e da pilha de balanceamento.
- `compilador.h`: Declaração de todas as funções, tipos de token e estruturas de dados do projeto.
- `main.c`: Programa principal que inicializa e chama o analisador sintático para executar a análise completa.
- `codigo_fonte.txt`: Arquivo de entrada com o código da linguagem a ser analisado.

## 📥 Como Compilar

No Linux (gcc) ou Windows (Dev-C++ / Code::Blocks), inclua o novo arquivo `parser.c` no comando:

```bash
gcc -o compilador main.c compilador.c parser.c
```

## ▶️ Como Executar

1.  Coloque o código-fonte a ser analisado no arquivo `codigo_fonte.txt`.
2.  Execute o programa compilado:
    ```bash
    ./compilador
    ```
3.  O programa exibirá o resultado da análise sintática. Se não houver erros, mostrará a tabela de símbolos e, ao final, o relatório de memória.

## 📄 Licença

Distribuído sob a licença GNU GENERAL PUBLIC LICENSE.