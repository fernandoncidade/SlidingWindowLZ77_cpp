NOTAS DE LANÇAMENTO: Versão 2026.11.22.0<br>
Data: 22 de novembro de 2026<br>

# Sliding Window LZ77 Visualizer & 7z/LZMA Extractor

Esta atualização moderniza integralmente a base do aplicativo e de suas ferramentas auxiliares para oferecer uma experiência mais estável, rápida e confiável no Windows 10 e no Windows 11 (64 bits).

## Principais Melhorias e Atualizações

### 1. Experiência Unificada e Exclusiva para Windows 10 e Windows 11 (64 bits)
- O aplicativo foi consolidado em uma única edição nativa de 64 bits dedicada ao Windows 10 e ao Windows 11, encerrando a manutenção da variante alternativa legada.
- Essa unificação elimina diferenças entre edições e assegura que todos os usuários recebam o mesmo padrão de qualidade, compatibilidade e desempenho.

### 2. Base Gráfica e de Sistema Modernizada
- Os componentes visuais e a estrutura interna do aplicativo foram atualizados para uma geração mais recente, proporcionando maior estabilidade geral, melhor integração com recursos atuais do Windows e preparação para futuras evoluções do programa.
- Os arquivos essenciais que acompanham o aplicativo agora são organizados de forma mais consistente e limpa, evitando conflitos de ambiente e garantindo a execução imediata sem dependências ausentes.

### 3. Mais Confiabilidade nas Ferramentas de Apoio
- As ferramentas complementares de tradução e gerenciamento de versão foram alinhadas à mesma base atualizada do aplicativo principal.
- Foi corrigida uma situação que podia gerar conflitos de arquivos durante a preparação simultânea dessas ferramentas, tornando o processo mais seguro e previsível.

### 4. Distribuição, Instalação e Manutenção Aprimoradas
- O empacotamento e o instalador foram simplificados e atualizados para trabalhar exclusivamente com a edição nativa de 64 bits para Windows 10 e Windows 11, removendo itens obsoletos e assegurando a inclusão automática dos componentes necessários para o funcionamento do programa.
- As rotinas de limpeza de arquivos temporários e preparação do ambiente foram refinadas para manter o projeto organizado e livre de resquícios de versões anteriores.

### 5. Documentação Atualizada
- Os documentos informativos do projeto foram revisados em Português (Brasil) e Inglês (EUA) para refletir a nova base exclusiva para Windows 10 e Windows 11 (64 bits), mantendo total fidelidade entre os dois idiomas.

---

NOTAS DE LANÇAMENTO: Versão 2026.8.21.0<br>
Data: 21 de agosto de 2026<br>

# Sliding Window LZ77 Visualizer & 7z/LZMA Extractor

Bem-vindo às Notas de Lançamento oficiais do **Sliding Window LZ77 Visualizer & 7z/LZMA Extractor**. Esta versão consolida uma suíte completa de engenharia de software para visualização, descompressão em fluxo contínuo e análise profunda de algoritmos de compressão de dados baseados em dicionário deslizante (Sliding Window LZ77) e no algoritmo LZMA (Lempel-Ziv-Markov chain-Algorithm).

---

## 1. Visão Geral do Projeto

O **Sliding Window LZ77 Visualizer** é uma ferramenta profissional de alta performance desenvolvida em **C++17** com a biblioteca gráfica **Qt6** (versão 6.11.1). Seu propósito é fornecer tanto utilidade prática (extração real de arquivos compactados multiformato via LZMA SDK) quanto visibilidade analítica em tempo real de cada ponteiro de repetição (match) encontrado no fluxo de dados, exibindo:
- **Distância (Offset)**: deslocamento retroativo dentro do dicionário deslizante.
- **Comprimento (Length)**: quantidade de bytes idênticos referenciados no buffer anterior.
- **Literais**: bytes emitidos diretamente sem correspondência prévia no dicionário.
- **Tamanho Descomprimido e Taxa de Compressão**: métricas globais e temporais de processamento.

---

## 2. Principais Funcionalidades e Destaques desta Versão

### 2.1 Motor de Descompressão LZMA SDK Integrado
- **Decodificador LZMA/LZMA2 Completo**: Baseado na implementação de referência oficial do LZMA SDK do 7-Zip (`7zDec.c`, `LzmaDec.c`, `Lzma2Dec.c`, `Bra86.c`, `Ppmd7Dec.c`, `7zAlloc.c`, etc.).
- **Suporte Multiformato Amplo**: Compatibilidade nativa com arquivos compactados nos formatos `.7z`, `.zip`, `.rar`, `.tar`, `.wim`, `.xz`, `.gz`, `.bz2`, `.cab`, `.iso`, `.lzma`, `.zst`, `.tgz`, `.tbz2`, `.txz`.
- **Arquitetura Assíncrona Multithread**: Todo o processamento de descompressão e cálculo de repetições é executado em segundo plano através de uma `QThread` dedicada (`LzmaExtractor`), garantindo total responsividade da interface gráfica (GUI 60 FPS) mesmo durante o processamento de arquivos de múltiplos gigabytes.
- **Emissão e Amostragem de Lotes (Batch Sampling)**: Transmissão fluida de dados para a GUI em lotes controlados com amostragem inteligente, prevenindo saturação de memória e garantindo alta taxa de transferência.

### 2.2 Tabela Virtual de Repetições em Tempo Real (Virtual Table)
- **Modelo de Dados Virtualizado (`LzmaMatchesTableModel`)**: Tabela virtual baseada no paradigma `QAbstractTableModel`, capaz de renderizar milhões de repetições com consumo irrisório de memória e rolagem instantânea.
- **Colunas Detalhadas**:
  - `Índice`: Sequencial do registro de correspondência.
  - `Posição do Fluxo (Stream Offset)`: Offset exato no arquivo descomprimido.
  - `Distância (Distance)`: Deslocamento no dicionário deslizante.
  - `Comprimento (Length)`: Tamanho do bloco repetido.
  - `Janela / Dicionário (Dictionary Window)`: Indicação do alcance relativo no buffer.

### 2.3 Interface Gráfica Moderna e Widgets Nativos Qt6
- **Interface Nativa Pura**: Todos os diálogos, menus e componentes seguem rigorosamente os padrões de widgets nativos do Qt6 (`QDialog`, `QTextBrowser`, `QTableView`, `QProgressBar`, `QMenuBar`), integrando-se perfeitamente ao tema e à acessibilidade do sistema operacional.
- **Menu Sobre Completo**:
  - `Sobre`: Apresentação e arquitetura do software.
  - `Leia-me`: Instruções rápidas, requisitos e compilação.
  - `Manual de Instruções`: Guia passo a passo com sumário interativo e navegação por âncoras.
  - `Nota de Lançamento`: Registro histórico detalhado de versões e mudanças.
  - `Documentação e Licenças`: Visualizador de licenças e avisos de terceiros em abas dedicadas.

### 2.4 Internacionalização e Troca Dinâmica de Idiomas (i18n)
- **Suporte Bilingue Completo**: Português do Brasil (`pt-BR`) e Inglês (`en-US`).
- **Troca Dinâmica em Tempo Real**: Seleção instantânea através do menu `Configurações > Idioma`, recarregando todos os textos, menus, caixas de diálogo, tabelas e manuais em tempo de execução sem necessidade de reiniciar o aplicativo.

### 2.5 Sistema de Logging e Rastreamento Estruturado
- **Gerenciador Centralizado de Logs (`LogManager`)**: Criação automática de arquivos de log datados em `%LOCALAPPDATA%\SlidingWindowLZ77\logs` (ou pasta local de build).
- **Rastreamento de Eventos**: Registro detalhado com níveis de severidade (INFO, WARNING, ERROR, DEBUG), rastreando a abertura de arquivos, estatísticas de descompressão e encerramento.

### 2.6 Suporte a Compilação Dupla (Dual Toolchain)
- **MinGW (GCC 13.1.0)**: Otimizações com `--release` e vinculação estática de bibliotecas C/C++.
- **MSVC (Visual Studio 2022 / v14.51)**: Compilação otimizada para arquitetura x64 com empacotamento completo de dependências CRT do Windows.
- **Presets Automatizados CMake (`CMakePresets.json`)**: Configuração, build e deploy padronizados com Ninja em uma única etapa.
- **Paridade de Distribuição**: Tamanho equilibrado entre as compilações (~64 MB MSVC e ~71 MB MinGW).

---

## 3. Estrutura e Ferramentas Auxiliares (Mocks)

O projeto inclui um conjunto de utilitários de suporte construídos e exportados conjuntamente:
1. **`CompileTranslationsGUI.exe`**: Interface gráfica para compilar arquivos de localização `.ts` em binários `.qm` usando a ferramenta `lrelease` do Qt6.
2. **`CompileTranslationsTerminal.exe`**: Ferramenta de linha de comando para automação de builds e compilação de traduções.
3. **`find_VersionEditor.exe`**: Painel visual de governança que varre e sincroniza em lote a versão, a data de revisão e o autor em todos os documentos Markdown (`.md`) e instaladores Inno Setup (`.iss`) do projeto.

---

## 4. Instaladores Oficiais (Inno Setup)

- Scripts parametrizados em [`repo/SlidingWindowLZ77.iss`](file:///c:/Users/ferna/APLICATIVOS/CPP/SlidingWindowLZ77_cpp/repo/SlidingWindowLZ77.iss), [`repo/SlidingWindowLZ77_MinGW.iss`](file:///c:/Users/ferna/APLICATIVOS/CPP/SlidingWindowLZ77_cpp/repo/SlidingWindowLZ77_MinGW.iss) e [`repo/SlidingWindowLZ77_MSVC.iss`](file:///c:/Users/ferna/APLICATIVOS/CPP/SlidingWindowLZ77_cpp/repo/SlidingWindowLZ77_MSVC.iss).
- Geração de instaladores modernos com descompactação sólida, ícones de alta resolução, criação de atalhos e desinstalador limpo.

---

Versão: v2026.8.21.0<br>
Data técnica desta revisão: 21 de agosto de 2026<br>
Autor: Fernando Nillsson Cidade<br>
