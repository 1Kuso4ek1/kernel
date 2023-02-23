#pragma once
#include <font.hpp>

void* framebufferPtr;

class Framebuffer
{
public:
	Framebuffer(unsigned int width, unsigned int height, unsigned int* buffer)
	{
		this->width = width;
		this->height = height;
		this->buffer = buffer;
	}

	void DrawPixel(unsigned int x, unsigned int y, unsigned int color)
	{
		buffer[y * width + x] = color;
	}

	void DrawChar(char ch, unsigned int x, unsigned int y, unsigned int color)
	{
		unsigned int pixel = 0;
	    unsigned long long ch1 = font[ch];
	    
	    int i = width * (y - 1) + x + 8;
	    int inc = width - 8;
	    for(int yy = 0; yy < 8; yy++)
	    {
	        i += inc;
	        for(int xx = 0; xx < 8; xx++)
	        {
	            if ((ch1 >> pixel++) & 1)
	            {
	                buffer[i] = color;
	            }
	            i++;
	        }
	    }
	}

	void DrawRectangle(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color)
	{
		unsigned int i = width * (y - 1);
		i += x + w;
	    for(int yy = 0; yy < h; yy++)
	    {
	        i += width - w;
	        for(int xx = 0; xx < w; xx++)
	        {
	        	buffer[i++] = color;
	        }
	    }
	}

	void DrawString(char* str, unsigned int x, unsigned int y, unsigned int color)
	{
		int i = 0;
		while(str[i] != '\0')
		{
			DrawChar(str[i], x + i * 8, y, color);
			i++;
		}
	}

	void DrawString(char* str, unsigned int x, unsigned int y, int& cursorX, int& cursorY, unsigned int color)
	{
		int i = 0;
		while(str[i] != '\0')
		{
			if(str[i] == '\n')
			{
				cursorX = 0; cursorY += 10;
			}
			else
			{
				DrawChar(str[i], x + cursorX, y + cursorY, color);
				cursorX += 8;
				if(cursorX >= width)
				{
					cursorX = 0; cursorY += 10;
				}
			}
			i++;
		}
	}

	void Display()
	{
		for(int i = 0; i < width * height; i++)
		{
			//unsigned int* pixel = (unsigned int*)(framebufferPtr + mbInfo->framebufferPitch * i + 4 * i);
			((unsigned int*)(framebufferPtr))[i] = buffer[i];
		}
	}
	
	void Display(Framebuffer* fb, unsigned int dx = 0, unsigned int dy = 0)
	{
		for(int i = 0; i < width; i++)
			for(int j = 0; j < height; j++)
				fb->DrawPixel(i + dx, j + dy, buffer[j * width + i]);
	}
	
private:
	unsigned int width, height;
	unsigned int* buffer;
};
