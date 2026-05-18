# Bar Game

Um jogo 2D de coleta desenvolvido em C++ utilizando OpenGL (FreeGLUT). O objetivo é controlar uma cesta utilizando reflexos rápidos para coletar frutas que caem (peras, bananas e maçãs) enquanto desvia ativamente de bombas para maximizar a sua pontuação.

## Pré-requisitos

Para compilar e executar o jogo no **Windows**, é necessário configurar um ambiente de desenvolvimento C++ com as bibliotecas gráficas apropriadas:

1. **MinGW-w64 (via MSYS2)**: Necessário para obter o compilador `g++`.
2. **FreeGLUT**: Biblioteca utilizada para manipulação de janelas e contextos de renderização em OpenGL.

> **Nota**: A biblioteca `stb_image.h` (utilizada para renderizar as texturas PNG com transparência alpha) está embutida diretamente no código-fonte e não requer instalação adicional.

### Configuração do Ambiente (Windows)

O método mais direto para configurar o ambiente é através do terminal do MSYS2. Execute o seguinte comando:
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-freeglut
```
*Certifique-se de que o diretório `bin` do MinGW (ex: `C:\msys64\mingw64\bin`) esteja corretamente adicionado à variável de ambiente `PATH` do seu Windows.*

## Compilação e Execução

Após instalar o compilador e as dependências, abra o seu terminal (Prompt de Comando ou PowerShell) e navegue até o diretório raiz do projeto. 

Você pode compilar o projeto de duas formas:

### Opção 1: Compilação manual via g++ (Recomendado se não possuir o Make)
Copie e cole o comando abaixo no terminal para compilar todos os arquivos fonte e linkar as bibliotecas necessárias em um único passo:
```bash
g++ -Wall -std=c++11 main.cpp src/game.cpp src/render.cpp -o bar_game.exe -lfreeglut -lglu32 -lopengl32
```

### Opção 2: Compilação via Makefile (Se possuir o Make instalado)
Se você instalou a ferramenta `make` (`pacman -S mingw-w64-x86_64-make`), basta executar:
```bash
make
```

### Executando o Jogo
Independente do método de compilação escolhido, execute o binário gerado com:
```bash
.\bar_game.exe
```

## Controles

* **Setas (Esquerda `<-` / Direita `->`)**: Movem a cesta horizontalmente.
* **Shift (Segurar)**: Ativa o *Dash* para mover a cesta com velocidade aumentada.
* **ESC**: Abandona a sessão atual e retorna ao Menu Principal (ou encerra a aplicação se já estiver no menu).

## Estrutura do Projeto

* `main.cpp`: Ponto de entrada, loop principal da aplicação e tratamento de eventos da janela.
* `src/`: Lógica central de física, gerenciamento de estado do jogo e motor de renderização.
* `img/`: Recursos de textura do jogo (.png).
* `Makefile`: Script automatizado para compilação.