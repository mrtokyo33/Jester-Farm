# **Jester Farm**

JesterFarm é um jogo escrito em C++ usando SDL2
O Jogo ainda está em desenvolvimento

## **Ideia**

Jester Farm é um jogo de fazenda isométrico com elementos de fantasia, onde você assume o papel de um bobo da corte exilado que decide recomeçar a vida cultivando sua própria terra
Parece uma fazenda comum: você planta, colhe, cria animais e expande sua propriedade. Porém, o mundo ao seu redor é imprevisível. De tempos em tempos, eventos mágicos e estranhos acontecem, transformando completamente a rotina da fazenda. Esses fenômenos podem ser tanto benéficos quanto perigosos

## **Tecnologias**

- C++17
- SDL2
- SDL2_image
- SDL2_ttf
- SDL2_mixer
- nlohmann_json
- Cmake

## **Game Loop**
um loop infinito que:
- captura input
- atualizar a lógica
- desenhar na tela

## **Como funciona tile isométrico**

cada canteiro será uma "matriz" com posição (x, y)
cada Tile será: 64x43px (2:1) tendo um achatamento = profundidade

o cálculo matemático é:

- x = (col - lin) * (tile_w/2) + offsetX
- y = (col + lin) * (tile_h/2) + offsetY

estrutura de Dados:
```cpp
enum PlotState {
    Blocked = 0,
    Empty,
    Planted,
    Growing,
    Mature, 
    Harvested
};

struct Plot {
    int col; 
    int lin;
    PlotState state;
}
```

Para Desenhar irá ter:
- Preencher com a cor do state
- Borda do canteiro
- Borda se tiver selecionado

## **Build**

na pasta root
```bash
cmake -S .  -B build -G Ninja
cmake --build build
```

## **Run**
```bash
./build/JesterFarm
```