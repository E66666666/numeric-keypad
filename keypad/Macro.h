#pragma once

struct Stroke {
	Stroke(uint8_t key, unsigned long keyDown, unsigned long keyUp): 
		   Key(key), KeyDown(keyDown), KeyUp(keyUp) {}

	uint8_t Key;
	unsigned long KeyDown;
	unsigned long KeyUp;
	bool Active = false;
};

struct Macro {
	Macro(uint8_t numStrokes, Stroke *strokes):
		  NumStrokes(numStrokes), Strokes(strokes){}

	uint8_t NumStrokes;
	Stroke *Strokes;
};
