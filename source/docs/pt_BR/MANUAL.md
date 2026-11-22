<a name="inicio"></a><a name="sumario"></a>
# Manual do Usuário - Sliding Window LZ77 & LZMA Visualizer

Guia completo e passo a passo para utilização do **Sliding Window LZ77 & LZMA Visualizer**.

---

## 📑 Sumário
1. [Introdução](#capitulo-1)
2. [Formatos de Arquivo Suportados](#capitulo-2)
3. [Interface Gráfica e Componentes](#capitulo-3)
4. [Guia Passo a Passo de Extração e Análise](#capitulo-4)
5. [Interpretação dos Dados e Métricas](#capitulo-5)
6. [Troca de Idioma e Internacionalização](#capitulo-6)
7. [Sistema de Logs, Configuração e Diagnóstico](#capitulo-7)
8. [Solução de Problemas Comuns (Troubleshooting)](#capitulo-8)

---

<a name="capitulo-1"></a><a name="1-introdução"></a><a name="1-introducao"></a>
## [1. Introdução](#inicio)

O **Sliding Window LZ77 & LZMA Visualizer** é uma ferramenta gráfica projetada para descompactar arquivos comprimidos e inspecionar visualmente como os algoritmos LZ77 e LZMA identificam redundâncias nos dados, substituindo-as por referências curtas `(distância, comprimento)`.

---

<a name="capitulo-2"></a><a name="2-formatos-de-arquivo-suportados"></a>
## [2. Formatos de Arquivo Suportados](#inicio)

O aplicativo suporta toda a gama de arquivos manipulados pelo projeto [7-Zip (ip7z/7zip)](https://github.com/ip7z/7zip):

| Categoria | Extensões Suportadas |
|---|---|
| **Arquivos Nativos de Alta Compressão** | `.7z`, `.xz`, `.lzma`, `.zst` |
| **Arquivos Populares e Empacotadores** | `.zip`, `.rar`, `.tar`, `.tgz`, `.tbz2`, `.txz`, `.gz`, `.bz2`, `.cab` |
| **Imagens de Disco e Contêineres** | `.wim`, `.iso`, `.dmg`, `.vhd`, `.vhdx`, `.vdi`, `.vmdk`, `.fat`, `.ntfs` |
| **Pacotes de Distribuição** | `.msi`, `.deb`, `.rpm`, `.cpio`, `.arj`, `.lzh`, `.chm` |

---

<a name="capitulo-3"></a><a name="3-interface-gráfica-e-componentes"></a><a name="3-interface-grafica-e-componentes"></a>
## [3. Interface Gráfica e Componentes](#inicio)

Ao iniciar a aplicação, a janela principal apresenta os seguintes blocos principais:

1. **Barra de Menus:**
   - **Arquivo:** Permite abrir qualquer arquivo compactado suportado ou sair do aplicativo.
   - **Configurações > Idioma:** Permite alternar instantaneamente entre **Português (Brasil)** e **English (US)**.
   - **Sobre:** Acesso direto a **Sobre** (`SOBRE.md` / `ABOUT.md`), **Leia-me** (`LEIAME.md` / `README.md`), **Manual** (`MANUAL.md`) e **Documentação** (diálogo com abas para consulta de todas as licenças, permissões e avisos de componentes de terceiros).
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

<a name="capitulo-4"></a><a name="4-guia-passo-a-passo-de-extração-e-análise"></a><a name="4-guia-passo-a-passo-de-extracao-e-analise"></a>
## [4. Guia Passo a Passo de Extração e Análise](#inicio)

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

<a name="capitulo-5"></a><a name="5-interpretação-dos-dados-e-métricas"></a><a name="5-interpretacao-dos-dados-e-metricas"></a>
## [5. Interpretação dos Dados e Métricas](#inicio)

- **Referência Curta `(d, l)`:** Quanto maior o comprimento $l$, maior foi a sequência reutilizada. Quanto menor a distância $d$, mais recente era a repetição na janela.
- **Taxa de Compressão Teórica:** Se o número de repetições for alto e o número de literais for baixo, o arquivo original possuía alta redundância e alcançou grande taxa de compressão.

---

<a name="capitulo-6"></a><a name="6-troca-de-idioma-e-internacionalização"></a><a name="6-troca-de-idioma-e-internacionalizacao"></a>
## [6. Troca de Idioma e Internacionalização](#inicio)

1. Acesse a barra de menus em **Configurações > Idioma**.
2. Selecione **Português-BR** ou **English-US**.
3. Todos os textos da interface gráfica, botões, títulos de tabela e caixas de diálogo de documentação são traduzidos instantaneamente em tempo de execução, sem necessidade de reiniciar.

---

<a name="capitulo-7"></a><a name="7-sistema-de-logs-configuração-e-diagnóstico"></a><a name="7-sistema-de-logs-configuracao-e-diagnostico"></a>
## [7. Sistema de Logs, Configuração e Diagnóstico](#inicio)

Todos os registros de execução e preferências de configuração são gravados na pasta de dados locais do usuário:

- **Diretório Base:** `C:\Users\<usuario>\AppData\Local\LZ77_SlidingWindow\`
- **Logs:** `%LOCALAPPDATA%\LZ77_SlidingWindow\logs\slidingwindowlz77_YYYY-MM-DD.log`
- **Configuração de Idioma:** `%LOCALAPPDATA%\LZ77_SlidingWindow\config\language.json`

---

<a name="capitulo-8"></a><a name="8-solução-de-problemas-comuns-troubleshooting"></a><a name="8-solucao-de-problemas-comuns-troubleshooting"></a>
## [8. Solução de Problemas Comuns (Troubleshooting)](#inicio)

1. **"O arquivo informado não existe"**: Verifique se o caminho do arquivo selecionado é acessível.
2. **Arquivos `.rar` ou `.wim` não iniciam extração streaming**: Certifique-se de que o 7-Zip está instalado no diretório padrão (`C:\Program Files\7-Zip\7z.exe`) ou configurado no `PATH` do sistema. O aplicativo utiliza automaticamente o fallback para análise direta caso o executável externo não esteja presente.
3. **Cancelamento da Operação**: Caso o arquivo seja muito volumoso, você pode interromper o processo a qualquer instante clicando em **"Cancelar"**.

---

Versão: v2026.11.22.0<br>
Data técnica desta revisão: 22 de novembro de 2026<br>
Autor: Fernando Nillsson Cidade<br>
