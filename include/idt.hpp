#pragma once
#include <external.hpp>

struct IDTEntry
{
	unsigned short offsetLowerBits;
	unsigned short selector;
	unsigned char zero;
	unsigned char typeAttrib;
	unsigned short offsetHigherBits;
};

static IDTEntry idtArr[256];

static void InitPIC()
{
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
}

static void InitIDT(char interrupt, unsigned long addr)
{
    unsigned long idt[2];
    
    idtArr[interrupt].offsetLowerBits = addr & 0xffff;
    idtArr[interrupt].selector = 0x08; // 0x08 - segment offset
    idtArr[interrupt].zero = 0;
    idtArr[interrupt].typeAttrib = 0x8e; // int gate
    idtArr[interrupt].offsetHigherBits = (addr & 0xffff0000) >> 16;

    unsigned long idtAddr = (unsigned long)idtArr;
    
    idt[0] = (sizeof(IDTEntry) * 256) + ((idtAddr & 0xffff) << 16);
    idt[1] = idtAddr >> 16;

    load_idt(idt);
}
