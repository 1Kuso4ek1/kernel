#include <multiboot.hpp>
#include <idt.hpp>
#include <keyboard.hpp>
#include <utils.hpp>
#include <font.hpp>

void* framebufferPtr;
int cursorX = 0, cursorY = 10;
unsigned int commandBufferPos = 0;
char commandBuffer[256];
bool shift = false;

void DrawChar(char, unsigned int, unsigned int, unsigned int);
void DrawPixel(unsigned int, unsigned int, unsigned int);
void DrawRectangle(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

extern "C" void kb_handle()
{
	unsigned char status = read_port(0x64);

	if(status & 0x01)
	{
        unsigned char keycode = read_port(0x60);

        if(keycode > 127 && keycode != 127 + 43)
        {
        	write_port(0x20, 0x20); // End of interrupt
            return;
        }
        if(keycode == 0x1C) // Enter
        {
        	cursorX = 0; cursorY += 10;
        	commandBuffer[commandBufferPos] = '\0';
    		write_port(0x20, 0x20); // End of interrupt
        	return;
        }
        else if(keycode == 0x0E) // Backspace
        {
        	cursorX -= 8;
        	if(cursorX < 0)
			{
				cursorX = 640 - 8; cursorY -= 10;
			}
        	DrawRectangle(cursorX, cursorY, 8, 8, 0x202020);

        	commandBuffer[--commandBufferPos] = ' ';
        }
        else if(keycode == 42 || keycode == 127 + 43)
       		shift = !shift;
        else
        {
        	char ch;
        	if(shift) ch = kbKeysShift[keycode];
        	else ch = kbKeys[keycode];
        	DrawChar(ch, cursorX, cursorY, 0xFFFFFF);
        	cursorX += 8;
        	if(cursorX >= 640)
			{
				cursorX = 0; cursorY += 10;
			}

        	commandBuffer[commandBufferPos] = ch;
        	commandBufferPos++;
        }
    }
    
    write_port(0x20, 0x20); // End of interrupt
}

int CompareCommandBuffer(const char* command)
{
	int i = 0;
	while(commandBuffer[i] != '\0' || command[i] != '\0')
	{
		if(commandBuffer[i] != command[i])
			return 0;
		i++;
	}
	return 1;
}

void ClearCommandBuffer()
{
	for(int i = 0; i < 256; i++)
		commandBuffer[i] = ' ';
}

void DrawPixel(unsigned int x, unsigned int y, unsigned int color)
{
	unsigned int* pixel = (unsigned int*)(framebufferPtr + (mbInfo->framebufferPitch * x) + (4 * x) + (4 * y * 640));
	*pixel = color;
}

void DrawChar(char ch, unsigned int x, unsigned int y, unsigned int color)
{
	unsigned int pixel = 0;
    unsigned long long ch1 = font[ch];
    
    int i = 640 * (y - 1) + x + 8;
    int inc = 640 - 8;
    for(int yy = 0; yy < 8; yy++)
    {
        i += inc;
        for(int xx = 0; xx < 8; xx++)
        {
            if ((ch1 >> pixel++) & 1)
            {
                //buffer[i] = color;
                unsigned int* pixel = (unsigned int*)(framebufferPtr + mbInfo->framebufferPitch * i + 4 * i);
                *pixel = color;
            }
            i++;
        }
    }
}

void DrawRectangle(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color)
{
	unsigned int i = 640 * (y - 1);
	i += x + w;
    for(int yy = 0; yy < h; yy++)
    {
        i += 640 - w;
        for(int xx = 0; xx < w; xx++)
        {
        	unsigned int* pixel = (unsigned int*)(framebufferPtr + mbInfo->framebufferPitch * (i++) + 4 * i);
        	*pixel = color;
        }
    }
}

void DrawString(char* str, unsigned int x, unsigned int y, unsigned int color)
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
			if(cursorX >= 640)
			{
				cursorX = 0; cursorY += 10;
			}
		}
		i++;
	}
}

extern "C" void kmain(unsigned int sp, unsigned long magic, unsigned long addr)
{
	mbInfo = (MultibootInfo*)addr;
    framebufferPtr = (void*)(unsigned long)mbInfo->framebufferAddr;
    DrawRectangle(0, 0, 640, 480, 0x202020);
    DrawString("Welcome to the SusOS", 240, 0, 0xFFFFFF);
    cursorX = 0; cursorY = 20;
    //DrawString("> ", 0, 0, 0xFFFFFF);

	InitPIC();
	InitIDT(0x21, (unsigned long)call_kb_handle);
	write_port(0x21, 0xFD); // keyboard IRQ1

	while(1)
	{
		if(commandBuffer[commandBufferPos] == '\0')
		{
			if(CompareCommandBuffer("clear"))
			{
				DrawRectangle(0, 0, 640, 480, 0x202020);
				cursorX = 0; cursorY = 10;
			}
			if(CompareCommandBuffer("sus"))
			{
				DrawString("AMOGUS!\n", 0, 0, 0xFFFFFF);
			}
			if(CompareCommandBuffer("help"))
			{
				DrawString("Available commands:\nsus -- system will yell 'AMOGUS!'\n", 0, 0, 0xFFFFFF);
				DrawString("clear -- clear the screen\nhelp -- this message\n", 0, 0, 0xFFFFFF);
			}
			commandBufferPos = 0;
			ClearCommandBuffer();
			DrawString("> ", 0, 0, 0xFFFFFF);
		}
	}
}
