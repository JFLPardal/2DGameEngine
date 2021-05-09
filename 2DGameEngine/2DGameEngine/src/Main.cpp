#include <sol/sol.hpp>

#include "Game/Game.h"

int nativeCPPCubeFunction(int n)
{
   return n* n* n;
}

void TestLua()
{
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    // expose and bind c++ functions to lua
    // needs to be done before script_file
    lua["cube"] = nativeCPPCubeFunction;

    lua.script_file("./assets/scripts/myscript.lua");

    // get values from lua scripts
    sol::table config = lua["config"];

    const bool isFullscreen = config["isFullscreen"];
    const std::string windowTitle = config["title"];
    const size_t windowWidth = config["resolution"]["width"];
    const size_t windowHeight = config["resolution"]["height"];

    std::cout << 
        "window: " << windowTitle << 
        " is fullscreen: " << isFullscreen  <<  
        " with width: " << windowWidth << 
        " and height: " << windowHeight <<
        std::endl;

    // call functions from lua scripts
    sol::function factorialFunction = lua["factorial"];
    const int factorialResult = factorialFunction(6);

    std::cout << "factorial of 6 is: " << factorialResult << std::endl;;
}

int main(int argc, char* argv[]) 
{
    Game game;

    TestLua();
    game.Initialize();
    game.Run();
    game.Destroy();
    return 0;
} 