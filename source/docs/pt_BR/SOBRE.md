# Sobre o Sliding Window LZ77 & LZMA Visualizer

---

## 🔬 Fundamentos Teóricos e Algorítmicos

O **Sliding Window LZ77 & LZMA Visualizer** é uma ferramenta de engenharia reversa, análise de dados e visualização educacional que desmistifica o funcionamento interno da compressão sem perdas baseada em dicionário histórico e modelos estatísticos de Markov em toda a gama de contêineres e formatos suportados pelo ecossistema [7-Zip](https://github.com/ip7z/7zip).

### 1. O Algoritmo LZ77 e a Janela Deslizante (Sliding Window)
Proposto por Abraham Lempel e Jacob Ziv em 1977, o LZ77 substitui sequências repetidas de dados por ponteiros para ocorrências anteriores dentro de um buffer de histórico limitado (chamado *Janela Deslizante* ou *Dicionário*).

Cada repetição identificada é expressa pelo par:
$$\text{Referência Curta} = (d, l)$$
Onde:
- **$d$ (Distância / Distance):** A quantidade de bytes para trás no histórico onde a sequência idêntica teve início.
- **$l$ (Comprimento / Length):** O número de bytes consecutivos que se repetem.

Caso nenhuma repetição com tamanho vantajoso seja encontrada no histórico, o dado é emitido como um **Literal** (byte isolado).

```text
[ Histórico da Janela Deslizante (Buffer) ] <-- Distância d --
... A B C D E F G ... [ A B C ] X Y Z ...
                      ^^^^^^^
                      Comprimento l = 3
                  Emissão: (d=120, l=3)
```

---

### 2. A Evolução no LZMA (Lempel-Ziv-Markov chain Algorithm)
Criado por Igor Pavlov em 1998 para o formato 7-Zip, o LZMA aperfeiçoa o conceito do LZ77 ao combinar:

1. **Dicionários Gigantescos:** Janelas deslizantes que podem atingir de 64 KB até vários Gigabytes (ex.: $2^{27}$ bytes = 128 MB).
2. **Histórico de Repetições Recentes (4 Distâncias):** Mantém os últimos 4 offsets utilizados (`rep0`, `rep1`, `rep2`, `rep3`) para referências ultra-rápidas com custo quase nulo de bits.
3. **Cadeias de Markov e Contextos:** A probabilidade de cada bit subsequente depende do estado atual da máquina de estados (12 estados) e dos bits de contexto (`lc`, `lp`, `pb`).
4. **Codificador de Intervalo (Range Coder):** Um codificador entrópico aritmético de alta precisão que codifica símbolos fracionários de bits conforme probabilidades dinâmicas continuamente recalculadas.

---

### 3. Arquitetura Dual de Desconstrução Multi-formato

Para atender arquivos `.7z`, `.rar`, `.zip`, `.tar`, `.wim`, `.xz`, `.gz`, `.bz2`, `.cab`, `.iso`, `.lzma`, `.zst` e demais contêineres:

1. **Motor Nativo C SDK com Hooks para `.7z`:**
   - **Injeção em `LzmaDec.c`:**
     - *Gancho de Repetição:* Disparado logo após a decodificação da distância e comprimento pelo Range Coder, imediatamente antes da cópia circular no buffer de dicionário:
       ```c
       if (g_lzma_match_hook != NULL) {
           g_lzma_match_hook(dicPos, distance, len, g_lzma_hook_user_data);
       }
       ```
     - *Gancho de Literal:* Disparado quando um byte literal é emitido pela árvore de probabilidades de Markov:
       ```c
       if (g_lzma_literal_hook != NULL) {
           g_lzma_literal_hook(dicPos, (Byte)b, g_lzma_hook_user_data);
       }
       ```
   - Processamento de cabeçalhos decodificados, filtros Delta, BCJ/BCJ2 e blocos sólidos em memória.

2. **Motor Deslizante Multi-formato para Demais Contêineres:**
   - Para arquivos como `.zip`, `.rar`, `.tar`, `.wim`, `.xz`, `.gz`, `.bz2`, etc., a extração é conduzida pelo backend de streaming 7-Zip (`7z.exe` / decodificadores de fluxo).
   - O fluxo descompactado é submetido em tempo real ao algoritmo de janela deslizante LZ77 indexado por tabela de dispersão (*hash table*), detectando todas as repetições $(d, l)$ com comprimento $l \ge 3$ e emitindo eventos no mesmo formato estruturado.

3. **Propagação de Contexto Thread-Safe em Qt 6:**
   - Os eventos de repetição e literais são acumulados em lotes (*batches*) na thread de trabalho `LzmaExtractor` e despachados via sinais Qt (`Q_SIGNALS`) para a interface `MainWindow` sem travar a renderização gráfica.

---

## 🏛️ Diretrizes de Design e Persistência

- **Padrão Moderno de Persistência:** Todos os dados mutáveis (arquivos de log rotacionados e configurações de idioma) são gravados em `%LOCALAPPDATA%\LZ77_SlidingWindow\`, mantendo o diretório do projeto limpo e portátil.
- **Subsistema Windows GUI Puro:** O aplicativo opera sem consoles de terminal residuais.
- **Internacionalização Fluida:** Suporte completo e bidirecional a Português-BR e Inglês-US com atualização dinâmica de interface em tempo real.
- **Plataforma Nativa Moderna (Windows 10/11 x64):** Construído em C++23, C11 e Qt 6.12.0 com suporte completo aos compiladores **MinGW-w64 GCC 15.1.0 x64** (`mingw_64`) e **MSVC x64** (Visual Studio 18 / `v145`, Windows SDK `10.0.28000.0`) para máximo desempenho, estabilidade e integração nativa.

---

Versão: v2026.11.22.0<br>
Data técnica desta revisão: 22 de novembro de 2026<br>
Autor: Fernando Nillsson Cidade<br>
