#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <nlohmann/json.hpp>
#include <iostream>

constexpr int WINDOW_WIDTH      = 1280;
constexpr int WINDOW_HEIGHT     = 720; 
constexpr int FPS_TARGET        = 60;
constexpr int FRAME_DURATION_MS = 1000 / FPS_TARGET;

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

    SDL_Texture* textureTest = loadTexture(renderer, "./assets/image/playerDown.png");

    if (!textureTest) {
        std::cerr << "WARNING! Test Image not founded!" << std::endl;
        std::cerr << "The Game will run without sprite for a while." << std::endl;
    }

    int spriteWidth  = 0;
    int spriteHeight = 0;
    
    if(textureTest) {
        SDL_QueryTexture(textureTest, NULL, NULL, &spriteWidth, &spriteHeight); // obtém a largura e altura do sprite em px
        std::cout << "Sprite Loaded: " << spriteWidth << "x" << spriteHeight << "px" << std::endl;
    }

    float spriteX = (WINDOW_WIDTH - spriteWidth) / 2.0f;
    float spriteY = (WINDOW_HEIGHT - spriteHeight) / 2.0f;

    float spriteSpd = 200.0f;

    bool running = true; 

    // O SDL_Event é uma struct que guarda os eventos: tudo que acontece durante a execução do programa
    SDL_Event event;

    Uint32 previous_tick = SDL_GetTicks();
    float deltaTime = 0.0f;

    while(running) {
        Uint32 current_tick = SDL_GetTicks();
        deltaTime = (current_tick - previous_tick) / 1000.0f;      
        previous_tick = current_tick;

        while(SDL_PollEvent(&event)) {  // verifica os eventos
            if (event.type == SDL_QUIT) {  
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {    
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }

        // array que guarda o state de todas as teclas pressionadas
        const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

        // lógica de movimentação usando deltaTime
        if (keyboardState[SDL_SCANCODE_UP]) {
            spriteY -= spriteSpd * deltaTime;
        }
        if (keyboardState[SDL_SCANCODE_DOWN]) {
            spriteY += spriteSpd * deltaTime;
        }
        if (keyboardState[SDL_SCANCODE_LEFT]) {
            spriteX -= spriteSpd * deltaTime;
        }
        if (keyboardState[SDL_SCANCODE_RIGHT]) {
            spriteX += spriteSpd * deltaTime;
        }

        SDL_SetRenderDrawColor(renderer, 12, 12, 12, 255);
        SDL_RenderClear(renderer);

        if (textureTest) {
            SDL_Rect dest;  // literalmente um retangulo, com susas posições (x,y) e tamanho (w,h)
            dest.x = static_cast<int>(spriteX);
            dest.y = static_cast<int>(spriteY);
            dest.w = spriteWidth;
            dest.h = spriteHeight;

            SDL_RenderCopy(renderer, textureTest, NULL, &dest);
        }

        SDL_RenderPresent(renderer);

        // lógica para deixar fluído de acordo com o FRAME_DURATION_MS, para um frame nao ser muito curto e sobrecarregar a CPU
        Uint32 frame_time = SDL_GetTicks() - current_tick;

        if (frame_time < FRAME_DURATION_MS) {
            SDL_Delay(FRAME_DURATION_MS - frame_time);
        }
    }

    // limpeza já que o jogo não está mais rodando

    if (textureTest) {
        SDL_DestroyTexture(textureTest);
    }

    SDL_DestroyRenderer(renderer);  
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    std::cout << "\nGame Closed with successfull" << std::endl;

    return 0;
 }