cmake_minimum_required(VERSION 3.20)    

project(JesterIsle LANGUAGES CXX)       # Definindo o nome do projeto e a linguagem utilizada (C++)

set(CMAKE_CXX_STANDARD 17)              # definindo a versão do C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # a versão se torna obrigatoria suportar C++17
set(CMAKE_CXX_EXTENSIONS OFF)           # ele controla p/ as extensões do compilador serem apenas padrão oficial do C++

find_package(SDL2 REQUIRED)  
find_package(SDL2_image REQUIRED)                       
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(JesterIsle 
    src/main.cpp
)

target_link_libraries(JesterIsle PRIVATE # private -> as dependências só são visíveis para o projeto internamente
    SDL2::SDL2                 
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    nlohmann_json::nlohmann_json
)