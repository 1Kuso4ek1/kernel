#include <mouse.hpp>

Status status;

int posx = 0, posy = 0;

void Wait(int type)
{
    int time = 100000;
    while(time--)
    {
        if((type == 0 && (read_port(PS2_CMD_PORT) & 1) == 1) ||
            (type == 1 && (read_port(PS2_CMD_PORT) & 2) == 0))
            return;
    }
    return;
}

void UpdateStatus(char byte)
{
    if (byte & 0x01)
        status.left = 1;
    if (byte & 0x02)
        status.right = 1;
    if (byte & 0x04)
        status.middle = 1;
    if (byte & 0x08)
        status.always1 = 1;
    if (byte & 0x10)
        status.xSign = 1;
    if (byte & 0x20)
        status.ySign = 1;
    if (byte & 0x40)
        status.xOverflow = 1;
    if (byte & 0x80)
        status.yOverflow = 1;
}

unsigned char Read()
{
    Wait(0);
    return read_port(MOUSE_DATA_PORT);
}

void Write(unsigned char data)
{
    Wait(1);
    write_port(PS2_CMD_PORT, 0xD4);
    Wait(1);
    write_port(MOUSE_DATA_PORT, data);
}

void Update()
{
    static unsigned char cycle = 0;
    static char byte[3];

    switch (cycle)
    {
    case 0:
        byte[0] = Read();
        UpdateStatus(byte[0]);
        cycle++;
        break;
    case 1:
        byte[1] = Read();
        cycle++;
        break;
    case 2:
        byte[2] = Read();
        posx = posx + byte[1];
        posy = posy - byte[2];

        if (posx < 0) posx = 0;
        if (posy < 0) posy = 0;

        if (posx > 1600) posx = 1600 - 1;
        if (posy > 900) posy = 900 - 1;

        cycle = 0;
        break;
    }

    write_port(0xA0, 0x20 + 12);
}

void Init()
{
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
}

int GetX()
{
    return posx;
}

int GetY()
{
    return posy;
}
