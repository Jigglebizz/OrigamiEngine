# Origami Engine
Hello and welcome to the Origami Engine. This is a 2D game engine that seeks to support large teams working with 
many assets. The key feature of this engine is that it can load assets from two different locations, depending 
on configuration:
- From a generated binary file (also known as a pkg)
- From a web server

The advantage of this kind of system is that we can package our game normally, as we would in any other engine.
But in web server mode, we afford the flexibility to change assets while the game is running. A simple reload of 
a level, or changing the asset before it is encountered will result in your new changes being loaded on-the-fly.

Other advantages include running a central "asset server" that multiple team members can connect to simultaneously.
This can be helpful for setting up automated tests, or working with different versions of assets among team members.

## How it works
All assets are stored as JSON in source. A human-readable format is helpful for analyzing assets, especially as projects 
grow larger.

While developing your game, the built application "Kami.exe" should be running. This is the asset server that scans 
the filesystem and converts the assets into an optimized, binary format. Kami does this by loading and executing *Builders* 
for individual asset types. These are also known as Asset Serializers in other game engines. When a file is updated, 
Kami makes note of it. Depending on Kami's settings it may build it immediately, or wait until a game requests it to start 
building.

It is possible to modify the provided builders to meet the needs of your project - they are provided in the .sln under the 
"Builders" filter.

## Build setup

Build requires SDL2-2.0.12, stb, and rapidjson to be placed in directory `C:\sdk` as follows:

```
C:\
  sdk\
    SDL2-2.0.12\
    stb\
    rapidjson\
```

They are available at the following locations:  
SDL:   https://www.libsdl.org/release/  
STB:   https://github.com/nothings/stb.git  
RJSON: https://github.com/Tencent/rapidjson.git  


I'm linking SDL2 statically, which took a little 3 steps to get to build on Windows:

1. Add the following lines to src\core\windows\SDL_windows.h


```
#if defined STATIC_LIB
  # pragma comment(lib, "winmm.lib")
  # pragma comment(lib, "Version.lib")
  # pragma comment(lib, "Imm32.lib")
  # pragma comment(lib, "Setupapi.lib")
#endif
```

2. Change the configuration type to Static Library in the SDL2 Project's Configuration Properties

3. Define STATIC_LIB in the preprocessor definitions for SDL2

From here just hit build on the SDL2 projects for Debug and Release and you're good to go.

## When creating your own game
Rename the Solution Name and solution folder to the same name. For example MyCoolGame.sln would be inside the a 
folder named MyCoolGame. It is totally fine to leave both as OrigamiEngine, however these must match or the tools 
will break.