#ifndef game_hpp
#define game_hpp

#include <raylib.h>
#include "components/components.hpp"
#include "orbit_controls.hpp"
#include "./planet/create_grid.hpp"
#include "rlgl.h"
#include "raymath.h"
#include "utils.hpp"

class Game
{
public:
    Game();
    ~Game();
     
    void init();

    void handleEvents();

    void update();

    void render();

    void clean();
};

#endif