# ML2 Map File Spec

Revision 2

Note: The revision will be reset to 1 and all compatibility code will be removed from the loader once the code goes public.

## Overview

This is the spec for the ML2 map file, a file format created for Moon Lander 2 project by Will Brown. The goal is to be a simple, fast format for storing tile-based maps. This might see use in other projects that use a similar or identical tile-based renderer. It is supposed to be easy to use with the C programming language, and easy to unpack into a C-style struct, so it is a custom binary format that makes no use of any existing standards or any currently popular container formats. Most of these would be too slow, too difficult to implement to be worth the effort, or too space-inefficient. Many existing formats are much more complicated than is needed for a simple tile-based renderer like this one.

## Naming

ML2 map files should usually use the .ml2 file extension, although this is not strictly required.

## Header

The header is 16 bytes long, and intentionally structured such that you can easily unpack the file into a padded struct.

Bytes 0-3 of the file contain a null-terminated string with the ASCII text "ML2", or the bytes `{0x4D, 0x4C, 0x32, 0x00}`. If this is not present, you will immediately know that this is not a valid level file.

Bytes 4-7 are a little-endian unsigned 32-bit integer that contains the revision of the spec being used, so that if your program depends on a specific revision or wants to account for differences in revisions of the spec, it can do so safely regardless of the version of the spec.

The first 8 bytes of the header will always contain this information and no changes will ever be made, so any program that uses the spec will always be able to detect the version properly. Everything else can change freely.

Bytes 8-15 of the header contain two little-endian unsigned 32-bit integers denoting the width and height of the map by tile.

Bytes 16-23 contain the coordinates starting position of the player on the map as two little-endian unsigned 32-bit integers.

Bytes 24-27 of the header contain the amount of fuel the player will start with as a little-endian unsigned 32-bit integer.

Bytes 28-32 of the header contain a background color for the map in the form of an RGBA color.

Byte 33 of the header will determine the tilesheet that the map uses. If this byte is 0, the tilesheet is pulled from within the map file. Otherwise, one of the built in tilesheets is used.

## Custom tilesheets

Custom tilesheets are stored as a standard Windows bitmap (of any pixel format) directly after the end of the header data, such that it may be loaded in directly after the header.
However, before the bitmap data begins, you must provide the width and height of a single tile, so two unsigned little-endian 32-bit integers denoting the width and height of a tile (in that order) must be present in bytes 33-40.

## Map Data

Map data is currently dead simple. Tiles are stored as a row-major array, with each tile being 1 byte long. This is subject to change, as compression may be added or tile length may be extended.

The most significant bit of the tile denotes whether it is vertically flipped, and the second most significant bit denotes whether it is horizontally flipped, leaving 6 bits to denote what type of tile it is.
If shifted, these rotation bits can be used as an SDL_RendererFlip value.
This means you are currently able to address up to 64 individual types of tiles, each containing a parameter for direction flipped.

The coordinate (0, 0) can be found at the bottom left of the map, matching the coordinate system for Moon Lander 2, so expanding a map can be done with a trivial for loop, possibly using memcpy to speed up the process. This also makes the format easier to deal with for other types of 2D games, like platformers.

## Reference-implementation of the spec

The Moon Lander 2 program uses a struct similar to this:

```c
typedef struct {
	char sig[4];
	Uint32 rev;
	Uint32 width, height;
	Uint32 start_x, start_y, start_fuel;
	SDL_Color bgcolor;
	TileSheet *tiles;
	Uint8 data[];
} ML2_Map;
```

All header data unchanged from revision 1 is dumped directly into the struct.
Start position, color, and the tilesheet are selectively loaded from the file, depending on whether a revision 1 map or a revision 2 map is loaded.
If a revision 1 map is loaded, all default values from before revision 2 was finalized are used in place of the new values.
