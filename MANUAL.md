<!-- Multilanguage MANUAL.md for Sliding Window LZ77 Visualizer -->

<a name="inicio"></a><a name="sumario"></a><a name="top"></a>

<p align="center">
  <b>Selecione o idioma / Select language:</b><br>
  <a href="#ptbr">Português (BR)</a> |
  <a href="#enus">English (US)</a>
</p>

---

## <a id="ptbr"></a>Português (BR)

<details open>
<summary>Clique para expandir / recolher o Manual do Usuário em português</summary>

# Manual do Usuário - Sliding Window LZ77 & LZMA Visualizer

Guia completo e passo a passo para utilização do **Sliding Window LZ77 & LZMA Visualizer**.

---

## 📑 Sumário
1. [Introdução](#capitulo-1-pt)
2. [Formatos de Arquivo Suportados](#capitulo-2-pt)
3. [Interface Gráfica e Componentes](#capitulo-3-pt)
4. [Guia Passo a Passo de Extração e Análise](#capitulo-4-pt)
5. [Interpretação dos Dados e Métricas](#capitulo-5-pt)
6. [Troca de Idioma e Internacionalização](#capitulo-6-pt)
7. [Sistema de Logs, Configuração e Diagnóstico](#capitulo-7-pt)
8. [Solução de Problemas Comuns (Troubleshooting)](#capitulo-8-pt)

---

<a name="capitulo-1-pt"></a><a name="1-introdução"></a><a name="1-introducao"></a>
## [1. Introdução](#ptbr)

O **Sliding Window LZ77 & LZMA Visualizer** é uma ferramenta gráfica projetada para descompactar arquivos comprimidos e inspecionar visualmente como os algoritmos LZ77 e LZMA identificam redundâncias nos dados, substituindo-as por referências curtas `(distância, comprimento)`.

---

<a name="capitulo-2-pt"></a><a name="2-formatos-de-arquivo-suportados"></a>
## [2. Formatos de Arquivo Suportados](#ptbr)

O aplicativo suporta toda a gama de arquivos manipulados pelo projeto [7-Zip (ip7z/7zip)](https://github.com/ip7z/7zip):

| Categoria | Extensões Suportadas |
|---|---|
| **Arquivos Nativos de Alta Compressão** | `.7z`, `.xz`, `.lzma`, `.zst` |
| **Arquivos Populares e Empacotadores** | `.zip`, `.rar`, `.tar`, `.tgz`, `.tbz2`, `.txz`, `.gz`, `.bz2`, `.cab` |
| **Imagens de Disco e Contêineres** | `.wim`, `.iso`, `.dmg`, `.vhd`, `.vhdx`, `.vdi`, `.vmdk`, `.fat`, `.ntfs` |
| **Pacotes de Distribuição** | `.msi`, `.deb`, `.rpm`, `.cpio`, `.arj`, `.lzh`, `.chm` |

---

<a name="capitulo-3-pt"></a><a name="3-interface-gráfica-e-componentes"></a><a name="3-interface-grafica-e-componentes"></a>
## [3. Interface Gráfica e Componentes](#ptbr)

Ao iniciar a aplicação, a janela principal apresenta os seguintes blocos principais:

1. **Barra de Menus:**
   - **Arquivo:** Permite abrir qualquer arquivo compactado suportado ou sair do aplicativo.
   - **Configurações > Idioma:** Permite alternar instantaneamente entre **Português (Brasil)** e **English (US)**.
   - **Sobre:** Acesso direto a **Sobre** (`ABOUT.md`), **Leia-me** (`README.md`), **Manual** (`MANUAL.md`), **Nota de Lançamento** e **Documentação** (diálogo com abas para consulta de todas as licenças, permissões e avisos de componentes de terceiros).
2. **Painel de Seleção de Arquivo:**
   - Campo de texto exibindo o caminho absoluto do arquivo compactado selecionado.
   - Botão **"Procurar..."**: Abre o diálogo nativo do sistema com filtro para todos os tipos de arquivos suportados.
   - Botão de Ação Primária **"Iniciar Extração com Hook"**: Dispara a rotina assíncrona de descompressão e captura de repetições.
   - Botão **"Cancelar"**: Interrompe de forma segura a descompressão em andamento.
3. **Cartões de Métricas em Tempo Real:**
   - **Total de Repetições (Matches):** Quantidade de sequências redundantes encontradas na janela deslizante.
   - **Total de Literais:** Quantidade de bytes isolados emitidos sem repetição prévia.
   - **Tamanho Descomprimido:** Volume total acumulado de bytes decodificados.
   - **Tempo de Execução:** Duração do processamento em milissegundos.
4. **Tabela de Fluxo de Repetições:**
   - Exibe em tempo real o fluxo decodificado contendo:
     - **Offset de Saída:** Posição do byte no fluxo descompactado.
     - **Distância ($d$):** Distância para trás no histórico da janela deslizante.
     - **Comprimento ($l$):** Quantidade de bytes repetidos a partir daquela distância.
     - **Referência Curta:** Representação compacta `(d, l)`.
5. **Barra de Status e Progresso:**
   - Informa o estado atual (*Pronto*, *Extraindo e analisando fluxo...*, *Concluído*).
   - Barra de progresso visual exibindo a evolução percentual da descompressão.

---

<a name="capitulo-4-pt"></a><a name="4-guia-passo-a-passo-de-extração-e-análise"></a><a name="4-guia-passo-a-passo-de-extracao-e-analise"></a>
## [4. Guia Passo a Passo de Extração e Análise](#ptbr)

### Passo 1: Selecionar o Arquivo Compactado
- Clique no botão **"Procurar..."** ou acesse o menu **Arquivo > Abrir Arquivo Compactado...**.
- Localize e selecione um arquivo válido (ex.: `.7z`, `.rar`, `.zip`, `.tar`, `.wim`, `.xz`, etc.).

### Passo 2: Iniciar a Análise
- Clique no botão **"Iniciar Extração com Hook"**.
- O botão se desabilitará temporariamente, habilitando o botão **"Cancelar"**.

### Passo 3: Acompanhar o Processamento
- A tabela de repetições será preenchida progressivamente em lotes assíncronos.
- Os cartões de métricas serão atualizados continuamente conforme os blocos de dados são decodificados.

### Passo 4: Conclusão
- Ao término, uma caixa de diálogo informará o resumo completo da descompressão e as métricas finais consolidadas.

---

<a name="capitulo-5-pt"></a><a name="5-interpretação-dos-dados-e-métricas"></a><a name="5-interpretacao-dos-dados-e-metricas"></a>
## [5. Interpretação dos Dados e Métricas](#ptbr)

- **Referência Curta `(d, l)`:** Quanto maior o comprimento $l$, maior foi a sequência reutilizada. Quanto menor a distância $d$, mais recente era a repetição na janela.
- **Taxa de Compressão Teórica:** Se o número de repetições for alto e o número de literais for baixo, o arquivo original possuía alta redundância e alcançou grande taxa de compressão.

---

<a name="capitulo-6-pt"></a><a name="6-troca-de-idioma-e-internacionalização"></a><a name="6-troca-de-idioma-e-internacionalizacao"></a>
## [6. Troca de Idioma e Internacionalização](#ptbr)

1. Acesse a barra de menus em **Configurações > Idioma**.
2. Selecione **Português-BR** ou **English-US**.
3. Todos os textos da interface gráfica, botões, títulos de tabela e caixas de diálogo de documentação são traduzidos instantaneamente em tempo de execução, sem necessidade de reiniciar.

---

<a name="capitulo-7-pt"></a><a name="7-sistema-de-logs-configuração-e-diagnóstico"></a><a name="7-sistema-de-logs-configuracao-e-diagnostico"></a>
## [7. Sistema de Logs, Configuração e Diagnóstico](#ptbr)

Todos os registros de execução e preferências de configuração são gravados na pasta de dados locais do usuário:

- **Diretório Base:** `C:\Users\<usuario>\AppData\Local\LZ77_SlidingWindow\`
- **Logs:** `%LOCALAPPDATA%\LZ77_SlidingWindow\logs\slidingwindowlz77_YYYY-MM-DD.log`
- **Configuração de Idioma:** `%LOCALAPPDATA%\LZ77_SlidingWindow\config\language.json`

---

<a name="capitulo-8-pt"></a><a name="8-solução-de-problemas-comuns-troubleshooting"></a><a name="8-solucao-de-problemas-comuns-troubleshooting"></a>
## [8. Solução de Problemas Comuns (Troubleshooting)](#ptbr)

1. **"O arquivo informado não existe"**: Verifique se o caminho do arquivo selecionado é acessível.
2. **Arquivos `.rar` ou `.wim` não iniciam extração streaming**: Certifique-se de que o 7-Zip está instalado no diretório padrão (`C:\Program Files\7-Zip\7z.exe`) ou configurado no `PATH` do sistema. O aplicativo utiliza automaticamente o fallback para análise direta caso o executável externo não esteja presente.
3. **Cancelamento da Operação**: Caso o arquivo seja muito volumoso, você pode interromper o processo a qualquer instante clicando em **"Cancelar"**.

---

Versão: v2026.11.22.0<br>
Data técnica desta revisão: 22 de novembro de 2026<br>
Autor: Fernando Nillsson Cidade<br>

</details>

---

## <a id="enus"></a>English (US)

<details open>
<summary>Click to expand / collapse the User Manual in English</summary>

# User Manual - Sliding Window LZ77 & LZMA Visualizer

Comprehensive, step-by-step user guide for the **Sliding Window LZ77 & LZMA Visualizer**.

---

## 📑 Table of Contents
1. [Introduction](#chapter-1-en)
2. [Supported Archive Formats](#chapter-2-en)
3. [Graphical Interface & Components](#chapter-3-en)
4. [Step-by-Step Extraction & Analysis Guide](#chapter-4-en)
5. [Data & Metrics Interpretation](#chapter-5-en)
6. [Language Switching & Internationalization](#chapter-6-en)
7. [Logging, Configuration & Diagnostics](#chapter-7-en)
8. [Troubleshooting](#chapter-8-en)

---

<a name="chapter-1-en"></a><a name="1-introduction"></a>
## [1. Introduction](#enus)

The **Sliding Window LZ77 & LZMA Visualizer** is a graphical tool designed to decompress archives and visually inspect how the LZ77 and LZMA algorithms identify redundancies in data streams, replacing repetitive sequences with short `(distance, length)` reference pointers.

---

<a name="chapter-2-en"></a><a name="2-supported-archive-formats"></a>
## [2. Supported Archive Formats](#enus)

The application supports the full range of archive containers handled by the [7-Zip (ip7z/7zip)](https://github.com/ip7z/7zip) project:

| Category | Supported Extensions |
|---|---|
| **High-Ratio Compression Archives** | `.7z`, `.xz`, `.lzma`, `.zst` |
| **Popular & Standard Archives** | `.zip`, `.rar`, `.tar`, `.tgz`, `.tbz2`, `.txz`, `.gz`, `.bz2`, `.cab` |
| **Disk Images & Containers** | `.wim`, `.iso`, `.dmg`, `.vhd`, `.vhdx`, `.vdi`, `.vmdk`, `.fat`, `.ntfs` |
| **Distribution Packages** | `.msi`, `.deb`, `.rpm`, `.cpio`, `.arj`, `.lzh`, `.chm` |

---

<a name="chapter-3-en"></a><a name="3-graphical-interface--components"></a>
## [3. Graphical Interface & Components](#enus)

Upon launching the application, the main window provides the following key sections:

1. **Menu Bar:**
   - **File:** Open any supported archive format or exit the application.
   - **Settings > Language:** Instant switching between **Português (Brasil)** and **English (US)**.
   - **About:** Direct access to **About** (`ABOUT.md`), **Readme** (`README.md`), **Manual** (`MANUAL.md`), **Release Notes**, and **Documentation** (tabbed dialog to inspect all licenses, permissions, and third-party component notices).
2. **Archive Selection Panel:**
   - File path line edit displaying the selected archive's absolute location.
   - **"Browse..."** Button: Opens the native OS file picker with filters for all supported file formats.
   - Primary Action Button **"Start Hook Extraction"**: Triggers asynchronous background decompression and repetition capturing.
   - **"Cancel"** Button: Safely aborts ongoing extraction.
3. **Real-Time Metrics Cards:**
   - **Total Matches:** Count of redundant sequences identified in the sliding dictionary.
   - **Total Literals:** Count of single bytes emitted without previous matching history.
   - **Uncompressed Size:** Accumulated uncompressed byte size.
   - **Execution Time:** Decompression duration in milliseconds.
4. **Match Stream Table:**
   - Real-time tabular visualization containing:
     - **Output Offset:** Byte position in the uncompressed output stream.
     - **Distance ($d$):** Lookback offset in the sliding window buffer.
     - **Length ($l$):** Length in bytes of the repeated sequence.
     - **Short Reference:** Compact representation `(d, l)`.
5. **Status and Progress Bar:**
   - Displays ongoing status (*Ready*, *Extracting and analyzing stream...*, *Completed*).
   - Visual progress bar showing percentage completion.

---

<a name="chapter-4-en"></a><a name="4-step-by-step-extraction--analysis-guide"></a>
## [4. Step-by-Step Extraction & Analysis Guide](#enus)

### Step 1: Select Compressed Archive
- Click **"Browse..."** or navigate to **File > Open Archive File...**.
- Select a valid compressed archive (e.g., `.7z`, `.rar`, `.zip`, `.tar`, `.wim`, `.xz`, etc.).

### Step 2: Start Analysis
- Click **"Start Hook Extraction"**.
- The start button is temporarily disabled, and the **"Cancel"** button becomes active.

### Step 3: Monitor Live Stream
- The match table will populate in progressive asynchronous batches.
- Compression metric cards update continuously as data blocks decode.

### Step 4: Completion
- Upon completion, a summary dialog reports final totals and elapsed time.

---

<a name="chapter-5-en"></a><a name="5-data--metrics-interpretation"></a>
## [5. Data & Metrics Interpretation](#enus)

- **Short Pointer `(d, l)`:** Larger length $l$ indicates larger matched repetition. Smaller distance $d$ indicates recent appearance in the sliding window.
- **Theoretical Compression Ratio:** Higher match count with lower literal count indicates high redundancy and superior compression density.

---

<a name="chapter-6-en"></a><a name="6-language-switching--internationalization"></a>
## [6. Language Switching & Internationalization](#enus)

1. Open the menu at **Settings > Language**.
2. Select **Português-BR** or **English-US**.
3. All interface texts, tables, buttons, and document dialogs translate immediately at runtime without application restart.

---

<a name="chapter-7-en"></a><a name="7-logging-configuration--diagnostics"></a>
## [7. Logging, Configuration & Diagnostics](#enus)

All application logs and user preferences persist in standard OS local storage:

- **Base Directory:** `C:\Users\<user>\AppData\Local\LZ77_SlidingWindow\`
- **Logs:** `%LOCALAPPDATA%\LZ77_SlidingWindow\logs\slidingwindowlz77_YYYY-MM-DD.log`
- **Language Config:** `%LOCALAPPDATA%\LZ77_SlidingWindow\config\language.json`

---

<a name="chapter-8-en"></a><a name="8-troubleshooting"></a>
## [8. Troubleshooting](#enus)

1. **"The specified file does not exist"**: Check that the archive path is accessible and not locked.
2. **`.rar` or `.wim` archives do not stream extraction**: Ensure 7-Zip is installed in standard path (`C:\Program Files\7-Zip\7z.exe`) or available in system `PATH`. The visualizer automatically falls back to direct analysis if the external tool is absent.
3. **Aborting Large Tasks**: For very large files, click **"Cancel"** at any moment to stop decompression cleanly.

---

Version: v2026.11.22.0<br>
Technical revision date: November 22, 2026<br>
Author: Fernando Nillsson Cidade<br>

</details>
