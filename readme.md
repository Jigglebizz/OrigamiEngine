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

# When creating your own game
Rename the Solution Name and solution folder to the same name. For example MyCoolGame.sln would be inside the a folder named MyCoolGame. It is totally fine to leave both as OrigamiEngine, however these must match or the tools will break.