# Moon Lander 2

The sequel to [Moon Lander](https://github.com/WCBROW01/MoonLander_WCB/), my final project for my second semester computer science class.

Rather than being constrained to Java and Swing, as determined by the course curriculum, I can use whatever I want, so this is written in C using SDL 2. If I can avoid having any other dependencies, I will.

Why did I decide to make this? I have no idea. The first project is just a meme among some of my friends. However, I do have a few goals for this project:

- It should feel like an actual game.
- It will have 2D motion.
- It will have a larger more detailed board, with real terrain. (maybe procedurally generated?)
- The camera will either change size as you get higher (akin to the Atari game), or scroll.
- Just generally act more like the Atari lunar lander game.

Regardless, this is mostly an excuse for me to do some graphics programming, so don’t actually expect any of this to be here.

Special thanks to [Jerrin Redmon](https://github.com/CircuitBread0111) for the excellent sprites. You have contributed greatly to society with this.

## Building

To build, you only need to install SDL 2 and run `make`

On Linux or macOS, you can get this from your package manager. If you’re on Windows, you’re on your own.

Example for Debian/Ubuntu:

```sh
sudo apt install libsdl2-dev
```

