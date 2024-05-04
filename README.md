[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/kexGK_Es)
# project3-drone-simulator
##Installing  SDL and Compiling
https://wiki.libsdl.org/SDL2/Installation

###Alternative ways
- Linux (debian apt package manager)
``` sudo apt-get install libsdl2-dev```
to compile sdltest.c
```gcc sdltest.c -lSDL2```
to compile our program
```gcc list.c view.c model.c controller.c -lSDL2```

- macOS
Download SDL2 from https://github.com/libsdl-org/SDL/releases/tag/release-2.30.2
extension should be dmg. SDL2-2.30.2.dmg 
copy folder **SDL2.framework** into ``/Library/Frameworks``

to compile sdltest.c
```gcc sdltest.c -F/Library/Frameworks -framework SDL2```
to compile our program
```gcc list.c view.c model.c controller.c -F/Library/Frameworks -framework SDL2```

It may not allow to run ``a.out``, go to **Priviligies/Permissions** and allow to run ``a.out``.

- Ubuntu on WSL NOT TESTED! but maybe working with X11 https://learn.microsoft.com/en-us/windows/wsl/tutorials/gui-apps
