#ifndef __BAR_H__
#define __BAR_H__

struct Color;

struct Bar
{
	Bar()
	{
		bar = { 0,0,0,0 };
	}

	SDL_Rect	bar;
	Color		colour;
};

#endif // !__BAR_H__