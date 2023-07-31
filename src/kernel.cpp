#include <multiboot.hpp>
#include <idt.hpp>
#include <keyboard.hpp>
#include <utils.hpp>
#include <window.hpp>
#include <mouse.hpp>

int cursorX = 0, cursorY = 10;
unsigned int commandBufferPos = 0;
char commandBuffer[256];
bool shift = false;
const unsigned int width = 1600;
const unsigned int height = 900;
unsigned int buffers[3][width * height];
Framebuffer* mainFb;

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
        	mainFb->DrawRectangle(cursorX, cursorY, 8, 8, 0x202020);

        	commandBuffer[--commandBufferPos] = ' ';
        }
        else if(keycode == 42 || keycode == 127 + 43)
       		shift = !shift;
        else
        {
        	char ch;
        	if(shift) ch = kbKeysShift[keycode];
        	else ch = kbKeys[keycode];
        	mainFb->DrawChar(ch, cursorX, cursorY, 0xFFFFFF);
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

void Wallpaper(Framebuffer* fb)
{
	for(int i = 0; i < width; i++)
    	for(int j = 0; j < height; j++)
    		fb->DrawPixel(i, j, (i + j) / 10);
}

extern "C" void kmain(unsigned int sp, unsigned long magic, unsigned long addr)
{
	mbInfo = (MultibootInfo*)addr;
    framebufferPtr = (void*)(unsigned long)mbInfo->framebufferAddr;

    Framebuffer buf(width, height, buffers[0]);
    Wallpaper(&buf);
    Window w(50, 50, 400, 300, "Terminal", buffers[1]);
    //Window w1(100, 100, 200, 150, "Window", buffers[2]);
    
    mainFb = w.GetFramebuffer();
    mainFb->DrawRectangle(0, 0, 400, 300, 0x202020);
    mainFb->DrawString("Welcome to the SusOS", 100, 0, cursorX, cursorY, 0xFFFFFF);
    cursorX = 0; cursorY = 20;
    //DrawString("> ", 0, 0, 0xFFFFFF);

	InitPIC();
	InitIDT(0x21, (unsigned long)call_kb_handle);
	write_port(0x21, 0xFD); // keyboard IRQ1

	//Mouse::Init();

	while(1)
	{
		/*int x = Mouse::GetX();
		int y = Mouse::GetY();*/

		//mainFb->DrawRectangle(x, y, 10, 10, 0xFFFFFF);

		if(commandBuffer[commandBufferPos] == '\0')
		{
			if(CompareCommandBuffer("clear"))
			{
				mainFb->DrawRectangle(0, 0, 640, 480, 0x202020);
				cursorX = 0; cursorY = 10;
			}
			if(CompareCommandBuffer("sus"))
			{
				mainFb->DrawString("AMOGUS!\n", 0, 0, cursorX, cursorY, 0xFFFFFF);
			}
			if(CompareCommandBuffer("help"))
			{
				mainFb->DrawString("Available commands:\nsus -- system will yell 'AMOGUS!'\n", 0, 0, cursorX, cursorY, 0xFFFFFF);
				mainFb->DrawString("clear -- clear the screen\nhelp -- this message\n", 0, 0, cursorX, cursorY, 0xFFFFFF);
			}
			if(CompareCommandBuffer("close"))
			{
				w.SetIsOpen(false);
				Wallpaper(&buf);
			}
			commandBufferPos = 0;
			ClearCommandBuffer();
			mainFb->DrawString("> ", 0, 0, cursorX, cursorY, 0xFFFFFF);
		}
		w.Draw(&buf);
		//w1.Draw(&buf);
		buf.Display();
	}
}
