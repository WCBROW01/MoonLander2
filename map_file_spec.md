# ML2 Map File Spec

Revision 1

## Overview

This is the spec for the ML2 map file, a file format created for Moon Lander 2 project by Will Brown. The goal is to be a simple, fast format for storing tile-based maps. This might see use in other projects that use a similar or identical tile-based renderer. It is supposed to be easy to use with the C programming language, and easy to unpack into a C-style struct, so it is a custom binary format that makes no use of any existing standards or any currently popular container formats. Most of these would be too slow, too difficult to implement to be worth the effort, or too space-inefficient. Many existing formats are much more complicated than is needed for a simple tile-based renderer like this one.

## Naming

ML2 map files should usually use the .ml2 file extension, although this is not strictly required.

## Header

The header is 16 bytes long, and intentionally structured such that you can easily unpack the file into a padded struct.

The first 4 bytes of the file contain a null-terminated string with the ASCII text "ML2", or the bytes `{0x4D, 0x4C, 0x32, 0x00}`. If this is not present, you will immediately know that this is not a valid level file.

The next 4 bytes are a little-endian unsigned 32-bit integer that contains the revision of the spec being used, so that if your program depends on a specific revision or wants to account for differences in revisions of the spec, it can do so safely regardless of the version of the spec.

The first 8 bytes of the header will always contain this information and no changes will ever be made, so any program that uses the spec will always be able to detect the version properly. Everything else can change freely.

The final 8 bytes of the header contain two little-endian unsigned 32-bit integers denoting the width and height of the map by tile.

## Map Data

Map data is currently dead simple. Tiles are stored as a row-major array, with each tile being 1 byte long. This is subject to change, as compression may be added or tile length may be extended.

The most significant bit of the tile denotes whether it is horizontally flipped, and the second most significant bit denotes whether it is vertically flipped, leaving 6 bits to denote what type of tile it is.
This means you are currently able to address up to 64 individual types of tiles, each containing a parameter for direction flipped.

## Visualization of file structure as a struct

If you were to use this file in a program, you could very easily copy the file into memory and use it as-is with a struct similar to this:

```c
struct map {
	char sig[4];
	uint32_t rev;
	uint32_t width, height;
	uint8_t data[];
};
```

All you would need to do is allocate enough memory for the file and copy it, or memory map it.