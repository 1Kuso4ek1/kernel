#pragma once

#include "external.hpp"
#include "idt.hpp"

#define MOUSE_CMD_RESET 0xFF
#define MOUSE_CMD_RESEND 0xFE
#define MOUSE_CMD_SET_DEFAULTS 0xF6
#define MOUSE_CMD_DISABLE_PACKET_STREAMING 0xF5
#define MOUSE_CMD_ENABLE_PACKET_STREAMING 0xF4
#define MOUSE_CMD_SAMPLE_RATE 0xF3
#define MOUSE_CMD_MOUSE_ID 0xF2
#define MOUSE_CMD_REQUEST_SINGLE_PACKET 0xEB
#define MOUSE_CMD_STATUS 0xE9
#define MOUSE_CMD_RESOLUTION 0xE8

#define MOUSE_ACKNOWLEDGE 0xFA

#define PS2_CMD_PORT 0x64
#define MOUSE_DATA_PORT 0x60

typedef struct
{
    unsigned char left: 1;
    unsigned char right: 1;
    unsigned char middle: 1;
    unsigned char always1: 1;
    unsigned char xSign: 1;
    unsigned char ySign: 1;
    unsigned char xOverflow: 1;
    unsigned char yOverflow: 1;
} Status;

void Init();
/*{
    unsigned char status;

    Wait(1);
    write_port(PS2_CMD_PORT, 0xA8);

    // print mouse id
    write_port(MOUSE_DATA_PORT, MOUSE_CMD_MOUSE_ID);
    status = Read();

    //set_mouse_rate(10);

    Wait(1);
    write_port(PS2_CMD_PORT, 0x20);
    Wait(0);
    
    status = (read_port(MOUSE_DATA_PORT) | 2);

    Wait(1);
    write_port(PS2_CMD_PORT, MOUSE_DATA_PORT);
    Wait(1);
    write_port(MOUSE_DATA_PORT, status);

    Write(MOUSE_CMD_SET_DEFAULTS);

    Write(MOUSE_CMD_ENABLE_PACKET_STREAMING);

    InitIDT(0x20 + 12, (unsigned long)Update);
}*/

int GetX();
/*{
    return posx;
}*/

int GetY();
/*{
    return posy;
}*/
