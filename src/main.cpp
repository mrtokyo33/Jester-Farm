#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <nlohmann/json.hpp>
#include <iostream>

// window settings & frame settings
constexpr int WINDOW_WIDTH      = 1280;
constexpr int WINDOW_HEIGHT     = 720; 
constexpr int FPS_TARGET        = 60;
constexpr int FRAME_DURATION_MS = 1000 / FPS_TARGET;

// plot settings
constexpr int TILE_WIDTH    = 64;
constexpr int TILE_HEIGHT   = 32;

constexpr int GRID_COL      = 7;
constexpr int GRID_LIN      = 5;

constexpr int OFFSET_X      = WINDOW_WIDTH / 2;
constexpr int OFFSET_Y      = 200;

constexpr int INITIAL_PLOTS = 6;

// posição isométrica para posição X da tela
int isoToScreenX(int col, int lin) {
    return (col - lin) * (TILE_WIDTH / 2) + OFFSET_X;
}

// posição isométrica para posição Y da tela
int isoToScreenY(int col, int lin) {
    return (col + lin) * (TILE_HEIGHT / 2) + OFFSET_Y;
}

// a partir do mouse pegar qual coluna estou selecionando
float screenToGridCol(int mouseX, int mouseY) {
    float relX = (float)(mouseX - OFFSET_X);
    float relY = (float)(mouseY - OFFSET_Y);

    return (relX / (TILE_WIDTH / 2.0f) + relY / (TILE_HEIGHT / 2.0f)) / 2.0f;
}

// a partir do mouse pegar qual linha estou selecionando
float screenToGridLin(int mouseX, int mouseY) {
    float relX = (float)(mouseX - OFFSET_X);
    float relY = (float)(mouseY - OFFSET_Y);

    return (relY / (TILE_HEIGHT / 2.0f) - relX / (TILE_WIDTH / 2.0f)) / 2.0f;
}

// estados que um plot pode ter
enum PlotState {
    Blocked     = 0,
    Empty       = 1,
    Planted     = 2,
    Growing     = 3,
    Mature      = 4, 
    Harvested   = 5
};

// modelo do plot
struct Plot {
    int col; 
    int lin;
    PlotState state;
};

// desenha a partir de um centro (x, y) e altura e largura um retangulo, preenchido
void drawFilledDiamond(SDL_Renderer *renderer, int centerX, int centerY, int r, int g, int b, int width = TILE_WIDTH, int height = TILE_HEIGHT) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    // iteração para pintar todo
    for (int dy = -height / 2; dy <= height / 2; dy ++) {
        int halfWidth = (height / 2 - abs(dy)) * width / height;

        SDL_RenderDrawLine(renderer, centerX - halfWidth, centerY + dy, centerX + halfWidth, centerY + dy);
    }
}

// desenhar apenas o contorno do plot
void drawDiamondOutline(SDL_Renderer *renderer, int centerX, int centerY, int r, int g, int b, int width = TILE_WIDTH, int height = TILE_HEIGHT) {
    // definindo as posições do plot
    int topX = centerX;
    int topY = centerY - height / 2;

    int rightX = centerX + width / 2;
    int rightY = centerY;

    int leftX = centerX - width / 2;
    int leftY = centerY;

    int downX = centerX;
    int downY = centerY + height / 2; 

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    SDL_RenderDrawLine(renderer, topX, topY, rightX, rightY);
    SDL_RenderDrawLine(renderer, rightX, rightY, downX, downY);
    SDL_RenderDrawLine(renderer, downX, downY, leftX, leftY);
    SDL_RenderDrawLine(renderer, leftX, leftY, topX, topY);
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path) {
    // fluxo: arquivo PNG -> SDL_Surface (memória RAM) -> SDL_Texture (GPU) -> desenhar na tela

    SDL_Surface* surface = IMG_Load(path); // carrega a imagem na RAM (CPU Memory)

    if (!surface) {
        std::cerr << "Error Loading Image from: " << path << ":" << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface); // converte a imagem da RAM em GPU para rendereização

    SDL_FreeSurface(surface);   // libera a memória RAM

    if (!texture) {
        std::cerr << "Error Creating Texture from: " << path << ":" << SDL_GetError() << std::endl;
        return nullptr;
    }

    return texture;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {   // se der erro ao inicializar o audio e video
        std::cout << "Error Initializing SDL2: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::cout << "SDL2 Initialized successfully!" << std::endl;

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {    // se o suporte a PNG NÃO foi inicializado corretamente
        std::cout << "Error Initializing SDL2: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::cout << "SDL2_Image (PNG) Initialized successfully!" << std::endl;

    if (TTF_Init() != 0) {  // Se houve erro ao inicializar o SDL_ttf
        std::cout << "Error initializing SDL2_ttf: " << TTF_GetError() << std::endl; 
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    std::cout << "SDL2_ttf Initialized successfully!" << std::endl;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {   // Se houver erro ao inicializar o SDL_mixer
        std::cout << "Error initializing SDL2_mixer: " << Mix_GetError() << std::endl;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    std::cout << "SDL2_mixer Initialized successfully!" << std::endl;

    // criando a janela do jogo
    SDL_Window*  window = SDL_CreateWindow("Jester Isle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) { 
        std::cout << "Error creating the window: " << SDL_GetError() << std::endl;
        Mix_CloseAudio();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // criando o renderer utilizando a GPU para otimizar e sincronizando o monitor para evitar problemas
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); 

    if (!renderer) { 
        std::cout << "Error creating the renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // criando um array para cada plot
    std::array<Plot, GRID_COL * GRID_LIN> plots;

    int plots_unlocked = 0;

    // iteração de matriz para a definição dos plots
    for (int i = 0; i < GRID_LIN; i++) {
        for (int j=0; j < GRID_COL; j++) {
            int index = i * GRID_COL + j;

            plots[index].col = j;
            plots[index].lin = i;

            plots[index].state = (plots_unlocked < INITIAL_PLOTS) ? Empty : Blocked;
            if (plots_unlocked < INITIAL_PLOTS) {
                plots_unlocked++;
            }
        }
    }

    std::cout << "Farm: " << GRID_COL << "x" << GRID_LIN << " (" << INITIAL_PLOTS << " unlockeds)" << std::endl;

    bool running = true; 

    // O SDL_Event é uma struct que guarda os eventos: tudo que acontece durante a execução do programa
    SDL_Event event;

    Uint32 previous_tick = SDL_GetTicks();
    float deltaTime = 0.0f;

    int plotHover   = -1;
    int mouseX      = 0;
    int mouseY      = 0;

    while(running) {
        Uint32 current_tick = SDL_GetTicks();
        deltaTime = (current_tick - previous_tick) / 1000.0f;      
        previous_tick = current_tick;

        while(SDL_PollEvent(&event)) {  // verifica os eventos
            if (event.type == SDL_QUIT) {  
                running = false;
            }

            if (event.type == SDL_MOUSEMOTION) {    // toda vez que movermos o mouse
                mouseX = event.motion.x;
                mouseY = event.motion.y;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {  // controlando o click esquerdo
                if (plotHover >= 0) {
                    Plot& p = plots[plotHover];

                    std::cout << "Clicked (" << p.col << ", " << p.lin << "): ";
                    switch (p.state) {
                        case Blocked:   std::cout   << "BLOCKED"    << std::endl; break;
                        case Empty:     std::cout   << "EMPTY"      << std::endl; break;
                        case Planted:   std::cout   << "PLANTED"    << std::endl; break;
                        case Growing:   std::cout   << "GROWING"    << std::endl; break;
                        case Mature:    std::cout   << "MATURE"     << std::endl; break;
                        case Harvested: std::cout   << "HARVESTED"  << std::endl; break;
                    }
                }
            }

            if (event.type == SDL_KEYDOWN) {    
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }

        {
            float colF = screenToGridCol(mouseX, mouseY);
            float linF = screenToGridLin(mouseX, mouseY);

            int col = static_cast<int>(floor(colF));
            int lin = static_cast<int>(floor(linF));

            if (col >= 0 && col < GRID_COL && lin >= 0 && lin < GRID_LIN) {
                plotHover = lin * GRID_COL + col;
            } else {
                plotHover = -1;
            }
        }

        // array que guarda o state de todas as teclas pressionadas
        const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

        SDL_SetRenderDrawColor(renderer, 87, 250, 40, 255);
        SDL_RenderClear(renderer);

        // desenhando todos os plots com canteiros isométricos
        for (int i = 0; i < GRID_LIN; i ++) {
            for (int j = 0; j < GRID_COL; j++) {
                int screenX = isoToScreenX(j, i);
                int screenY = isoToScreenY(j, i);

                int index = i * GRID_COL + j;
                Plot& p = plots[index];

                switch(p.state) {
                    case Blocked:
                        drawFilledDiamond(renderer, screenX, screenY, 90, 90, 90);
                        SDL_SetRenderDrawColor(renderer, 60, 60, 50, 255);
                        SDL_RenderDrawLine(renderer, screenX - 8, screenY - 4, screenX + 8, screenY + 4);
                        SDL_RenderDrawLine(renderer, screenX + 8, screenY - 4, screenX - 8, screenY + 4);
                        break;

                    case Empty:
                        drawFilledDiamond(renderer, screenX, screenY, 139, 100, 60);
                        break;

                    case Planted:
                        drawFilledDiamond(renderer, screenX, screenY, 120, 80, 45);
                        SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
                        SDL_RenderDrawLine(renderer, screenX - 8, screenY - 4, screenX + 8, screenY + 4);
                        break;

                    case Mature: {
                        drawFilledDiamond(renderer, screenX, screenY, 120, 80, 45);
                        SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
                        SDL_Rect fruit = {screenX - 4, screenY - 6, 8, 8};
                        SDL_RenderFillRect(renderer, &fruit);
                        break;
                    }

                    case Harvested:
                        drawFilledDiamond(renderer, screenX, screenY, 110, 95, 70);
                        SDL_SetRenderDrawColor(renderer, 80, 70, 50, 255);
                        SDL_RenderDrawLine(renderer, screenX - 6, screenY, screenX + 6, screenY);
                        break;
                }

                if (p.state != Blocked) {
                    drawDiamondOutline(renderer, screenX, screenY, 100, 70, 40);
                }

                if (index == plotHover) {
                    drawDiamondOutline(renderer, screenX, screenY, 255, 255, 0);
                    drawDiamondOutline(renderer, screenX, screenY, 255, 255, 100, TILE_WIDTH-4, TILE_HEIGHT - 2);
                }
            }
        }

        SDL_RenderPresent(renderer);

        // lógica para deixar fluído de acordo com o FRAME_DURATION_MS, para um frame nao ser muito curto e sobrecarregar a CPU
        Uint32 frame_time = SDL_GetTicks() - current_tick;

        if (frame_time < FRAME_DURATION_MS) {
            SDL_Delay(FRAME_DURATION_MS - frame_time);
        }
    }

    // limpeza já que o jogo não está mais rodando

    SDL_DestroyRenderer(renderer);  
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    std::cout << "\nGame Closed with successfull" << std::endl;

    return 0;
 }