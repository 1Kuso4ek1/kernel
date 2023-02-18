extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt);
extern void call_kb_handle();

const int screenSize = 80 * 25 * 2;
const int lines = 160;

char* videoMemory = (char*)0xb8000;
unsigned int cursor = 0, commandBufferPos = 0;
char commandBuffer[256];

struct IDTEntry
{
	unsigned short int offsetLowerBits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char typeAttrib;
	unsigned short int offsetHigherBits;
};

struct IDTEntry idtArr[256];

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

void Print(const char* msg, unsigned char color);
void UpdateCursor();

void kb_handle()
{
	write_port(0x20, 0x20); // End of interrupt

	unsigned char status = read_port(0x64);

	if(status != 0x0)
	{
        unsigned char keycode = read_port(0x60);

        if(keycode > 127 || kbKeys[keycode] == 0) return;

        if(keycode == 0x1C) // Enter
        {
        	Print("\n", 0x00);
        	commandBuffer[commandBufferPos] = '\0';
        	return;
        }
        else if(keycode == 0x0E) // Backspace
        {
        	videoMemory[--cursor] = 0xF0;
        	videoMemory[--cursor] = ' ';

        	commandBuffer[--commandBufferPos] = ' ';
        }
        else
        {
        	videoMemory[cursor++] = kbKeys[keycode];
        	videoMemory[cursor++] = 0xF0;

        	commandBuffer[commandBufferPos] = kbKeys[keycode];
        	commandBufferPos++;
        }
        
        UpdateCursor();
    }
}

void InitIDT()
{
	unsigned long kbAddr = (unsigned long)call_kb_handle;
    unsigned long idt[2];
    
    idtArr[0x21].offsetLowerBits = kbAddr & 0xffff;
    idtArr[0x21].selector = 0x08; // 0x08 - segment offset
    idtArr[0x21].zero = 0;
    idtArr[0x21].typeAttrib = 0x8e; // int gate
    idtArr[0x21].offsetHigherBits = (kbAddr & 0xffff0000) >> 16;

    write_port(0x20, 0x11);
    write_port(0xA0, 0x11);

    write_port(0x21, 0x20);
    write_port(0xA1, 0x28);

    write_port(0x21, 0x00);  
    write_port(0xA1, 0x00);  

    write_port(0x21, 0x01);
    write_port(0xA1, 0x01);

    write_port(0x21, 0xff);
    write_port(0xA1, 0xff);

    unsigned long idtAddr = (unsigned long)idtArr;
    
    idt[0] = (sizeof(struct IDTEntry) * 256) + ((idtAddr & 0xffff) << 16);
    idt[1] = idtAddr >> 16;

    load_idt(idt);
}

void InitKeyboard()
{
    write_port(0x21, 0xFD); // IRQ1
}

void EnableCursor(unsigned char start, unsigned char end)
{
	write_port(0x3D4, 0x0A);
	write_port(0x3D5, (read_port(0x3D5) & 0xC0) | start);
 
	write_port(0x3D4, 0x0B);
	write_port(0x3D5, (read_port(0x3D5) & 0xE0) | end);
}

void DisableCursor()
{
	write_port(0x3D4, 0x0A);
	write_port(0x3D5, 0x20);
}

void UpdateCursor()
{
	write_port(0x3D4, 0x0F);
	write_port(0x3D5, (unsigned char)((cursor / 2) & 0xFF));
	write_port(0x3D4, 0x0E);
	write_port(0x3D5, (unsigned char)(((cursor / 2) >> 8) & 0xFF));
}

void MoveCursor(int x, int y)
{
	cursor = (y * 80 + x) * 2;
	UpdateCursor();
}

unsigned short GetCursorPosition()
{
	unsigned short pos = 0;
    write_port(0x3D4, 0x0F);
    pos |= read_port(0x3D5);
    write_port(0x3D4, 0x0E);
    pos |= ((unsigned short)read_port(0x3D5)) << 8;
    return pos;
}

void ClearScreen(unsigned char color)
{
	for(int i = 0; i < screenSize; i += 2)
	{
		videoMemory[i] = ' ';
		videoMemory[i + 1] = color;
	}
	MoveCursor(0, 0);
}

void Print(const char* msg, unsigned char color)
{
	if(msg[0] == '\n')
	{
		cursor += lines - cursor % lines;
		UpdateCursor();
		return;
	}
	
	for(int i = 0; msg[i] != '\0'; i++)
	{
		videoMemory[cursor++] = msg[i];
		videoMemory[cursor++] = color;
	}

	UpdateCursor();
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

void kmain()
{
	ClearCommandBuffer();
	ClearScreen(0xF0);
	Print("                             Welcome to the kernel", 0xF0);
	Print("\n", 0x00);
	Print("> ", 0xF0);

	InitIDT();
	InitKeyboard();

	while(1)
	{
		if(commandBuffer[commandBufferPos] == '\0')
		{
			if(CompareCommandBuffer("clear"))
				ClearScreen(0xF0);
			if(CompareCommandBuffer("restart"))
				return;
			commandBufferPos = 0;
			ClearCommandBuffer();
			Print("> ", 0xF0);
		}
	}
}
