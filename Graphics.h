#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <windows.h>

int spawnWindow(HINSTANCE instance, int cmdShow, int width, int height, int fullscreen, const char* title, void (*onframe)(), void (*onclick)(int x, int y));
void resizeWindow(int width, int height);
unsigned char* pixels;
int size;

#endif //GRAPHICS_H
