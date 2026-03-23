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

        SDL_SetRenderDrawColor(renderer, 12, 12, 12, 255);

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

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