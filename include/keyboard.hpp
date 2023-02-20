#pragma once

// http://www.osdever.net/bkerndev/Docs/keyboard.htm
char kbKeys[128] =
{
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
  	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, // Ctrl
  	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, // LShift
 	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, // RShift
  	'*', 0,	// Alt
  	' ', 0,	// Caps lock
    0,	// F1
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	// F10
    0,	// Num lock
    0,	// Scroll Lock 
    0,	// Home
    0,	// Up Arrow
    0,	// Page Up
  	'-',
    0,	// Left Arrow
    0,
    0,	// Right Arrow
  	'+',
    0,	// End
    0,	// Down Arrow
    0,	// Page Down
    0,	// Insert
    0,	// Delete
    0,   0,   0,
    0,	// F11
    0,	// F12
    0,	// Undefined
};

char kbKeysShift[128] =
{
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
  	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, // Ctrl
  	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, // LShift
 	'|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, // RShift
  	'*', 0,	// Alt
  	' ', 0,	// Caps lock
    0,	// F1
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	// F10
    0,	// Num lock
    0,	// Scroll Lock 
    0,	// Home
    0,	// Up Arrow
    0,	// Page Up
  	'-',
    0,	// Left Arrow
    0,
    0,	// Right Arrow
  	'+',
    0,	// End
    0,	// Down Arrow
    0,	// Page Down
    0,	// Insert
    0,	// Delete
    0,   0,   0,
    0,	// F11
    0,	// F12
    0,	// Undefined
};
