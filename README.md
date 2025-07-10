# Analisador Léxico com Controle de Memória

Este projeto implementa um analisador léxico em C para uma linguagem simples. Além da análise léxica, inclui um subsistema de controle de memória dinâmico.

## 📝 Descrição

- Realiza a leitura de um arquivo-fonte (`codigo_fonte.txt`) e segmenta o texto em *tokens*.
- Identifica literais numéricos (inteiros e decimais) e literais de texto.
- Reconhece operadores matemáticos (`+`, `-`, `*`, `/`, `^`), relacionais (`==`, `<>`, `<`, `<=`, `>`, `>=`), lógicos (`&&`, `||`) e atribuição (`=`).
- Detecta pontuação: parênteses `()`, chaves `{}`, colchetes `[]`, ponto final, vírgula e ponto-e-vírgula.
- Reconhece palavras reservadas: `principal`, `funcao`, `retorno`, `leia`, `escreva`, `se`, `senao`, `para`, `inteiro`, `texto`, `decimal`.
- Identifica *identificadores* de variáveis (marcados com `!`) e funções (prefixo `__`).
- Gera mensagens de erro léxico com número da linha em casos de lexemas mal-formados ou caracteres não reconhecidos.

## 💾 Controle de Memória

- Aloca memória dinamicamente via `alocar_memoria(size_t)` e libera com `liberar_memoria(ptr, size)`.
- Monitora uso atual e pico de memória.
- Limite configurável em **2048 KB** (via define `MEMORIA_MAXIMA_KB`).
- Emite **alerta** quando o uso ultrapassa 90% da capacidade.
- Interrompe a execução com erro fatal caso a alocação exceda o limite.
- Ao final, exibe relatório de consumo: total disponível, pico utilizado e restante.

## ⚙️ Estrutura dos Arquivos

- `compilador.c`  – implementação do analisador léxico e controle de memória.
- `compilador.h`  – declaração de funções, tipos de token e estruturas.
- `main.c`        – programa principal: abre arquivo, chama o lexer e exibe resultados.
- `codigo_fonte.txt` – arquivo de entrada com código da linguagem alvo.

## 📥 Como Compilar

No Linux (gcc) ou Windows (Dev-C++ / Code::Blocks):

```bash
gcc -o compilador main.c compilador.c
```

## ▶️ Como Executar

1. Coloque o código-fonte a analisar em `codigo_fonte.txt`.
2. Execute:
   ```bash
   ./compilador
   ```
3. O programa exibirá a lista de tokens e, ao final, o relatório de memória.

## 📄 Licença

Distribuído sob a licença MIT.
