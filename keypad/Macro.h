#pragma once
#include <StandardCplusplus.h>
#include <vector>

struct Stroke {
	Stroke(uint8_t key, unsigned long keyDown, unsigned long keyUp): 
		   Key(key), KeyDown(keyDown), KeyUp(keyUp) {}

	uint8_t Key;
	unsigned long KeyDown;
	unsigned long KeyUp;
	bool Active = false;
};

struct Macro {
	Macro(std::vector<Stroke> strokes):
		  Strokes(strokes){}

	std::vector<Stroke> Strokes;
};
