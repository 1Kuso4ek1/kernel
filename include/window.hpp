#pragma once
#include <vga.hpp>

class Window
{
public:
	Window(unsigned int x, unsigned int y, unsigned int w, unsigned int h, char* title,
		   unsigned int* buffer)
		: x(x), y(y), w(w), h(h), title(title), contents(w, h, buffer)
	{}

	void Draw(Framebuffer* fb)
	{
		if(!isOpen) return;
		
		fb->DrawRectangle(x, y, w + 2, h + 12, 0xFFFFFF);
		fb->DrawRectangle(x + 1, y + 11, w, h, 0x000000);
		fb->DrawString(title, x + (w / 2 - w / 10), y + 2, 0x000000);
		contents.Display(fb, x + 1, y + 11);
	}

	void SetIsOpen(bool isOpen)
	{
		this->isOpen = isOpen;
	}

	bool IsOpen()
	{
		return isOpen;
	}

	Framebuffer* GetFramebuffer()
	{
		return &contents;
	}
	
private:
	unsigned int x, y, w, h;
	bool isOpen = true;
	char* title;
	Framebuffer contents;
};
