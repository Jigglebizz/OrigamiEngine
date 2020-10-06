Build requires SDL2-2.0.12 and stb to be placed in directory `C:\sdk` as follows:

```
C:\
  sdk\
    SDL2-2.0.12\
    stb\
```

I'm linking SDL2 statically, which took a little 2 steps to get to build on Windows:

1. Add the following lines to src\corewindows\SDL_windows.h


```
#if defined STATIC_LIB
  # pragma comment(lib, "winmm.lib")
  # pragma comment(lib, "Version.lib")
  # pragma comment(lib, "Imm32.lib")
  # pragma comment(lib, "Setupapi.lib")
#endif
```

2. Change the configuration type to Static Library in the SDL2 Project's Configuration Properties

From here just hit build on the SDL2 projects for Debug and Release and you're good to go.
