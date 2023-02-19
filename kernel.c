extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt);
extern void call_kb_handle();

const int screenSize = 80 * 25 * 2;
const int lines = 160;

char* videoMemory = (char*)0xB8000;
unsigned int* vgaMemory = (unsigned int*)0xA0000;
unsigned int cursor = 0, commandBufferPos = 0;
char commandBuffer[256];

struct MultibootSymbolTable
{
	unsigned int tabsize;
	unsigned int strsize;
	unsigned int addr;
	unsigned int reserved;
};

/* The section header table for ELF. */
struct MultibootELFSectionHeaderTable
{
	unsigned int num;
	unsigned int size;
	unsigned int addr;
	unsigned int shndx;
};

struct MultibootInfo
{
	/* Multiboot info version number */
	unsigned int flags;

	/* Available memory from BIOS */
	unsigned int memLower;
	unsigned int memUpper;

	/* "root" partition */
	unsigned int bootDevice;

	/* Kernel command line */
	unsigned int cmdline;

	/* Boot-Module list */
	unsigned int modsCount;
	unsigned int modsAddr;

	union
	{
		struct MultibootSymbolTable sym;
		struct MultibootELFSectionHeaderTable elf;
	} u;

	/* Memory Mapping buffer */
	unsigned int mmapLength;
	unsigned int mmapAddr;

	/* Drive Info buffer */
	unsigned int drivesLength;
	unsigned int drivesAddr;

	/* ROM configuration table */
	unsigned int configTable;

	/* Boot Loader Name */
	unsigned int bootloaderName;

	/* APM table */
	unsigned int apmTable;

	/* Video */
	unsigned int vbeControlInfo;
	unsigned int vbeModeInfo;
	unsigned short vbeMode;
	unsigned short vbeInterfaceSeg;
	unsigned short vbeInterfaceOff;
	unsigned short vbeInterfaceLen;

	unsigned long framebufferAddr;
	unsigned int framebufferPitch;
	unsigned int framebufferWidth;
	unsigned int framebufferHeight;
	unsigned char framebufferBpp;
	unsigned char framebufferType;
	
	union
	{
		struct
		{
			unsigned int framebufferPaletteAddr;
			unsigned short framebufferPaletteNumColors;
		};
		struct
		{
			unsigned char framebufferRedFieldPosition;
			unsigned char framebufferRedMaskSize;
			unsigned char framebufferGreenFieldPosition;
			unsigned char framebufferGreenMaskSize;
			unsigned char framebufferBlueFieldPosition;
			unsigned char framebufferBlueMaskSize;
		};
	};
};

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

void itoa (char *buf, int base, int d)
{
	char *p = buf;
	char *p1, *p2;
	unsigned long ud = d;
	int divisor = 10;

	/* If %d is specified and D is minus, put ‘-’ in the head. */
	if(base == 'd' && d < 0)
	{
		*p++ = '-';
		buf++;
		ud = -d;
	}
	else if(base == 'x')
		divisor = 16;

	/* Divide UD by DIVISOR until UD == 0. */
	do
	{
		int remainder = ud % divisor;

		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
	}
	while(ud /= divisor);

	/* Terminate BUF. */
	*p = 0;

	/* Reverse BUF. */
	p1 = buf;
	p2 = p - 1;
	while(p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

void kmain(unsigned long magic, unsigned long addr)
{
	struct MultibootInfo* mbInfo = (struct MultibootInfo*)addr;
	ClearCommandBuffer();
	ClearScreen(0xF0);
	Print("                             Welcome to the sus OS", 0xF0);
	Print("\n", 0x00);
	Print("Magic number: ", 0xF0);
	char buf[20];
	itoa(buf, 'x', magic);
	Print("0x", 0xF0);
	Print(buf, 0xF0);
	if(magic == 0x2BADB002)
		Print(" -- correct", 0xF0);
	else
		Print(" -- incorrect", 0xF0);
	Print("\n", 0x00);
	Print("Flags: ", 0xF0);
	char buf1[20];
	itoa(buf1, 'x', mbInfo->flags);
	Print("0x", 0xF0);
	Print(buf1, 0xF0);
	Print("\n", 0x00);
	Print("> ", 0xF0);

	InitIDT();
	InitKeyboard();

	if(((mbInfo->flags) & (1 << 12)))
	{
		unsigned int color = 0x10;
		void *fb = (void*)(unsigned long)mbInfo->framebufferAddr;
		for(unsigned int i = 0; i < mbInfo->framebufferWidth && i < mbInfo->framebufferHeight; i++)
		{
			unsigned int* pixel = fb + mbInfo->framebufferPitch * i + 4 * i;
			*pixel = color;
		}
	}

	while(1)
	{
		if(commandBuffer[commandBufferPos] == '\0')
		{
			if(CompareCommandBuffer("clear"))
				ClearScreen(0xF0);
			commandBufferPos = 0;
			ClearCommandBuffer();
			Print("> ", 0xF0);
		}
	}
}
