#pragma once

extern "C" char read_port(unsigned short port);
extern "C" void write_port(unsigned short port, unsigned char data);
extern "C" void load_idt(unsigned long *idt);
extern "C" void call_kb_handle();
