# Sliding Window LZ77 & LZMA Visualizer

Um aplicativo gráfico moderno em C++23 e Qt 6 desenvolvido para descompressão em tempo real e visualização interativa do algoritmo de compressão de janela deslizante (Sliding Window LZ77) e cadeias de Markov LZMA em toda a gama de arquivos suportados pelo projeto [7-Zip (ip7z/7zip)](https://github.com/ip7z/7zip).

---

## 🌟 Visão Geral

O **Sliding Window LZ77 & LZMA Visualizer** permite abrir, inspecionar e descompactar arquivos comprimidos de diversos formatos — abrangendo `.7z`, `.zip`, `.rar`, `.tar`, `.wim`, `.xz`, `.gz`, `.bz2`, `.cab`, `.iso`, `.lzma`, `.zst`, `.dmg`, `.vhd`, `.vhdx`, `.vdi`, `.vmdk`, `.fat`, `.ntfs`, `.ext` e outros contêineres suportados pelo ecossistema 7-Zip — interceptando e reconstruindo em tempo real o fluxo de referências de repetição e literais.

O projeto combina duas abordagens de desconstrução:
1. **Núcleo Nativo em C com Ganchos (*Hooks*) para `.7z`:** Decodificação direta no código oficial do **LZMA SDK (Igor Pavlov)**, interceptando o Range Coder e probabilidades Markovianas no laço mais interno de descompressão.
2. **Motor de Análise Deslizante Multi-formato:** Extração contínua e análise de janela deslizante LZ77 em tempo real para os demais contêineres de arquivo através da integração com o backend 7-Zip.

Durante o processamento, são capturados e exibidos:
- **Ponteiros de Repetição (Matches):** Distância no histórico da janela deslizante ($d$) e comprimento da repetição ($l$), representados pela referência curta `(d, l)`.
- **Símbolos Literais (Literals):** Bytes emitidos diretamente sem histórico de repetição na janela.
- **Métricas Globais:** Total de repetições, total de literais, tamanho descomprimido acumulado e tempo de execução.

---

## 🚀 Principais Funcionalidades

- **Suporte Amplo a Arquivos Compactados:** Compatibilidade com toda a gama de contêineres suportados pelo 7-Zip (`.7z`, `.zip`, `.rar`, `.tar`, `.wim`, `.xz`, `.gz`, `.bz2`, `.cab`, `.iso`, `.lzma`, `.zst`, etc.).
- **Visualização em Tempo Real de Ponteiros:** Tabela interativa com offset de saída, distância ($d$), comprimento ($l$) e par de referência curta `(d, l)`.
- **Persistência de Dados no Diretório do Usuário:** Armazenamento automático de logs e configurações em `%LOCALAPPDATA%\LZ77_SlidingWindow\` (`logs/` e `config/language.json`).
- **Internacionalização Dinâmica (i18n):** Alternância instantânea de idioma entre **Português (Brasil)** e **English (US)** pela barra de menus (`Configurações > Idioma`), sem reiniciar.
- **Visualizador Integrado de Documentação:** Leitura direta de `Sobre`, `Leia-me`, `Manual` e `Nota de Lançamento` pela barra de menus (`Sobre > ...`).
- **Execução Nativa GUI sem Janela de Terminal:** Subsistema Windows 10/11 x64 GUI puro com suporte completo para compiladores **MinGW-w64 x64** e **MSVC x64**.
- **Arquitetura Modular em C++23:** Separação estrita em módulos `source/config`, `source/interface`, `source/locale`, `source/logs`, `source/lzma_sdk` e `source/assets/RELEASES`.

---

## 🛠️ Tecnologias Utilizadas

- **Linguagens:** C++23 (Frontend e Lógica Qt) e C11 (Núcleo LZMA SDK)
- **Framework de GUI:** Qt 6.12.0 (Módulos: `Core`, `Gui`, `Widgets`, `LinguistTools`)
- **Compiladores e Plataformas Homologados (Windows 10/11 x64):**
  - **MinGW-w64 x64:** GCC/G++ `15.1.0` (`x86_64-posix-seh`, Qt `6.12.0/mingw_64`)
  - **MSVC x64:** Microsoft Visual C++ (Visual Studio 18 / toolset `14.52.37025` / `v145`, Windows SDK `10.0.28000.0`, Qt `6.12.0/msvc2022_64`)
- **Sistema de Build:** CMake 3.28+ e Ninja (`1.12+` / `1.13+`)
- **Ambiente de Desenvolvimento:** Visual Studio Code com CMake Tools e C/C++ Extensions

---

## 📂 Estrutura de Diretórios do Projeto

```text
SlidingWindowLZ77_cpp/
├── .vscode/                     # Configurações do VS Code (tasks.json, c_cpp_properties.json, etc.)
├── cmake/                       # Scripts auxiliares do CMake (deploy mínimo Qt)
├── mocks/                       # Ferramentas auxiliares isoladas (compilador de traduções, version editor)
├── source/
│   ├── assets/                  # Ícones, licenças e notas de lançamento (RELEASES)
│   ├── config/                  # Worker assíncrono de extração e hooks (LzmaExtractor)
│   ├── docs/                    # Documentação bilíngue integrada (pt_BR e en_US)
│   ├── interface/               # Janela principal e diálogos da interface gráfica (MainWindow, DocumentViewerDialog)
│   ├── locale/                  # Gerenciador de traduções dinâmicas e arquivos .ts / .qm
│   ├── logs/                    # Gerenciador de logs thread-safe (LogManager)
│   └── lzma_sdk/                # Código-fonte oficial do LZMA SDK (C puro) com injeção de hooks
├── CMakeLists.txt               # Script de compilação CMake da aplicação principal
├── CMakePresets.json            # Presets de configuração e build para MinGW x64 e MSVC x64
├── main.cpp                     # Ponto de entrada da aplicação
├── README.md                    # Readme multilíngue unificado
├── ABOUT.md                     # Detalhes teóricos e arquiteturais multilíngue unificado
└── MANUAL.md                    # Manual de utilização multilíngue unificado
```

> [!NOTE]
> Os diretórios de execução e persistência de dados (`logs/` e `config/`) são gerados no caminho de dados do usuário:  
> `C:\Users\<usuario>\AppData\Local\LZ77_SlidingWindow\`

---

## ⚙️ Compilação e Execução

### Pré-requisitos
- Windows 10/11 (64 bits).
- **Para MinGW x64:** Qt 6.12.0 (`mingw_64`) em `C:/Qt/6.12.0/mingw_64`, MinGW-w64 GCC `15.1.0` em `C:/Qt/Tools/mingw1510_64` e Ninja em `C:/Qt/Tools/Ninja`.
- **Para MSVC x64:** Visual Studio 18 (Community ou Build Tools) com toolset MSVC x64 `14.52.37025` (`v145`), Windows SDK `10.0.28000.0`, Ninja `1.13+` e Qt 6.12.0 (`msvc2022_64`) em `C:/Qt/6.12.0/msvc2022_64`.
- CMake 3.28 ou superior.

### 1. Compilação via VS Code (Tarefas Rápidas)
Pressione `Ctrl+Shift+P` > `Tasks: Run Task` e escolha:
- **`Qt6: Compilar e Executar LZ77Visualizer (MinGW)`** / **`(MSVC)`** para compilar o aplicativo em Release e executar.
- **`Qt6: Compilar LZ77Visualizer Debug (MinGW)`** / **`(MSVC)`** para compilar a configuração Debug.
- **`Qt6: Compilar e Executar Mocks (MinGW)`** / **`(MSVC)`** para compilar e executar os utilitários gráficos de apoio.
- **`Qt6: Compilar Tudo - App e Mocks (MinGW)`** / **`(MSVC)`** para compilar a aplicação principal e todas as ferramentas auxiliares do respectivo toolchain.
- **`Qt6: Compilar Tudo - MinGW e MSVC (App e Mocks)`** para reconstruir ambos os toolchains completos.

### 2. Compilação via Linha de Comando (PowerShell)

#### Compilação com MinGW 15.1.0 x64 (Release / Debug / Mocks):
```powershell
cmake --preset host-qt6-mingw-release
cmake --build --preset host-qt6-mingw-release

cmake --preset host-qt6-mingw-debug
cmake --build --preset host-qt6-mingw-debug

cmake --preset mocks-qt6-mingw-release
cmake --build --preset mocks-qt6-mingw-release
```
Saídas MinGW: `build/build_LZ77Visualizer_mingw/LZ77Visualizer.exe`, `build/build_LZ77Visualizer_mingw_debug/LZ77Visualizer.exe` e `build/build_mocks_mingw/`.

#### Compilação com MSVC x64 (Release / Debug / Mocks):
```powershell
cmake --preset host-qt6-msvc-release
cmake --build --preset host-qt6-msvc-release

cmake --preset host-qt6-msvc-debug
cmake --build --preset host-qt6-msvc-debug

cmake --preset mocks-qt6-msvc-release
cmake --build --preset mocks-qt6-msvc-release
```
Saídas MSVC: `build/build_LZ77Visualizer_msvc_ninja/Release/LZ77Visualizer.exe`, `build/build_LZ77Visualizer_msvc_ninja_debug/Debug/LZ77Visualizer.exe` e `build/build_mocks_msvc_ninja/Release/`.

---

## 📄 Licença e Créditos

- **LZMA SDK:** Domínio público / LGPL (Igor Pavlov).
- **Interface e Visualizador:** Desenvolvido em C++23 e Qt 6 sob licença MIT.

---

Versão: v2026.11.22.0<br>
Data técnica desta revisão: 22 de novembro de 2026<br>
Autor: Fernando Nillsson Cidade<br>
