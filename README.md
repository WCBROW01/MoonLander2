# Moon Lander 2

The sequel to [Moon Lander](https://github.com/WCBROW01/MoonLander_WCB/), my final project for my second semester computer science class.

Rather than being constrained to Java and Swing, as determined by the course curriculum, I can use whatever I want, so this is written in C using SDL 2. If I can avoid having any other dependencies, I will.

Why did I decide to make this? I have no idea. The first project is just a meme among some of my friends. However, I do have a few goals for this project:

- [ ] It should feel like an actual game.
- [x] It will have 2D motion.
- [x] It will have a larger more detailed board, with real terrain. (maybe procedurally generated?)
- [x] The camera will either change size as you get higher (akin to the Atari game), or scroll.
- [ ] Just generally act more like the Atari lunar lander game.
- [ ] Run on a wide variety of platforms.
- [ ] Include a map editor. (either internal or external)

Regardless, this is mostly an excuse for me to do some graphics programming, so don’t actually expect any of this to be here.

Special thanks to [Jerrin Redmon](https://github.com/CircuitBread0111) for the excellent sprites. You have contributed greatly to society with this.

## External assets

Currently the only external asset is the in-game font, which is distributed with the source code.

The font currently in use is [Public Pixel Font](https://ggbot.itch.io/public-pixel-font) version 1.0, created by [GGBotNet](https://www.ggbot.net/fonts/). It has been modified to be used as a bitmap font, and only the ASCII characterset is included.

## Building

To build, you only need to install SDL 2 and run `make`

On Linux or macOS, you can get this from your package manager. Windows builds using MSYS2 and MinGW-w64 have been successfully created and require no modifications to the source code.

Example for Debian/Ubuntu:

```sh
sudo apt install libsdl2-dev
```

