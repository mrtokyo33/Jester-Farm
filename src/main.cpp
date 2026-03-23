#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <nlohmann/json.hpp>
#include <iostream>

 int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {   // se der erro ao inicializar o audio e video
        std::cout << "Error Initializing SDL2: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::cout << "SDL2 Initialized successfully!" << std::endl;

    SDL_version version;        // a variavel version é uma SDL_version
    SDL_GetVersion(&version);   // passa o endereço de version para a função preencher com os dados da versão

    std::cout << "SDL2 version: " << int(version.major) << "." << int(version.minor) << "." << int(version.patch) << std::endl;

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

    nlohmann::json test;    // criando um teste do nlohmann_json
    test["game"] = "Jester File";
    test["version"] = 1;

    std::cout << "nlohmann_json working! Test: " << test.dump(2) << std::endl;

    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    std::cout << "\nAll systems working! Environment ready." << std::endl;

    return 0;
 }