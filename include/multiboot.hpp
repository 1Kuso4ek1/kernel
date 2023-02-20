#pragma once

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

MultibootInfo* mbInfo = nullptr;
