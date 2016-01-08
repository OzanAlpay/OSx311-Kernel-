#include "monitor.h"

u16int *video_memory = (u16int *)0xB8000;

u8int cursor_x = 0;
u8int cursor_y = 0;
u8int cursor_x_temp = 0;
u8int cursor_y_temp = 0;


static void move_cursor()
{
    u16int cursorLocation = cursor_y * 80 + cursor_x;
    outb(0x3D4, 14);
    outb(0x3D5, cursorLocation >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, cursorLocation);
}


static void scroll()
{
    u8int attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
    u16int blank = 0x20 /* space */ | (attributeByte << 8);

    if(cursor_y >= 25)
    {
        int i;
        for (i = 0*80; i < 24*80; i++)
        {
            video_memory[i] = video_memory[i+80];
        }

        for (i = 24*80; i < 25*80; i++)
        {
            video_memory[i] = blank;
        }

        cursor_y = 24;
    }
}
void monitor_put_with_colors(char c, u8int backColour, u8int foreColour) {
	u8int attributeByte = (backColour << 4) | (foreColour & 0x0F);
	u16int attribute = attributeByte << 8;
	u16int *location;

    if (c == 0x08 && cursor_x)
    {
        cursor_x--;
    }

    else if (c == 0x09)
    {
        cursor_x = (cursor_x+8) & ~(8-1);
    }

    else if (c == '\r')
    {
        cursor_x = 0;
    }

    else if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;
        monitor_command_prompt();
    }

    else if(c >= ' ')
    {
        location = video_memory + (cursor_y*80 + cursor_x);
        *location = c | attribute;
        cursor_x++;
    }

    if (cursor_x >= 80)
    {
        cursor_x = 0;
        cursor_y ++;
    }

    scroll();

    move_cursor();

}



void monitor_put(char c)
{
    u8int backColour = 0;
    u8int foreColour = 0x0F;

    u8int  attributeByte = (backColour << 4) | (foreColour & 0x0F);
    u16int attribute = attributeByte << 8;
    u16int *location;

    if (c == 0x08 && cursor_x)
    {

        cursor_x--;
        monitor_put(32);
        cursor_x--;
    }

    else if (c == 0x09)
    {
        cursor_x = (cursor_x+8) & ~(8-1);
    }

    else if (c == '\r')
    {
        cursor_x = 0;
    }

    else if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;
        monitor_command_prompt();
    }

    else if(c >= ' ')
    {
        location = video_memory + (cursor_y*80 + cursor_x);
        *location = c | attribute;
        cursor_x++;
    }

    if (cursor_x >= 80)
    {
        cursor_x = 0;
        cursor_y ++;
    }

    scroll();

    move_cursor();

}

void monitor_clear()
{
    u8int attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
    u16int blank = 0x20 /* space */ | (attributeByte << 8);

    int i;
    for (i = 0; i < 80*25; i++)
    {
        video_memory[i] = blank;
    }

    cursor_x = 0;
    cursor_y = 0;
    move_cursor();
}
void monitor_write_with_colors(char *c, u8int backColour, u8int foreColour)
{

	int i = 0;
	while(c[i])
	{
		monitor_put_with_colors(c[i++],backColour,foreColour);
	}
}
void monitor_welcome_message(){
  monitor_write_with_colors("Welcome to OSx311.\n",0,4);
}
void monitor_command_prompt(){
  monitor_write_with_colors("OSx311>",0,2);
}
void monitor_color_write(char *c, u8int backColour, u8int foreColour) {

  monitor_write_with_colors(c,backColour,foreColour);
  monitor_write("\n");
}
//bu zaten normalde olması gerekene eşitlendi rahatça kullanılabilir.
void monitor_write(char *c)
{

    int i = 0;
    while (c[i])
    {
        monitor_put(c[i++]);
    }
}

void monitor_write_hex(u32int n)
{
    s32int tmp;

    monitor_write("0x");

    char noZeroes = 1;

    int i;
    for (i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && noZeroes != 0)
        {
            continue;
        }

        if (tmp >= 0xA)
        {
            noZeroes = 0;
            monitor_put (tmp-0xA+'a' );
        }
        else
        {
            noZeroes = 0;
            monitor_put( tmp+'0' );
        }
    }

    tmp = n & 0xF;
    if (tmp >= 0xA)
    {
        monitor_put (tmp-0xA+'a');
    }
    else
    {
        monitor_put (tmp+'0');
    }

}
void monitor_write_dec_with_colors(u32int n, u8int backColor, u8int foreColor) {
    if (n == 0)
    {
        monitor_put_with_colors('0',backColor,foreColor);
        return;
    }

    s32int acc = n;
    char c[32];
    int i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc%10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;
    int j = 0;
    while(i >= 0)
    {
        c2[i--] = c[j++];
    }
    monitor_write_with_colors(c2,backColor,foreColor);

}

void monitor_write_dec(u32int n)
{

    if (n == 0)
    {
        monitor_put('0');
        return;
    }

    s32int acc = n;
    char c[32];
    int i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc%10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;
    int j = 0;
    while(i >= 0)
    {
        c2[i--] = c[j++];
    }
    monitor_write(c2);
}
void monitor_setTempCursorLocation(u8int x,u8int y)
{
	cursor_x_temp=cursor_x;
	cursor_y_temp=cursor_y;
	cursor_x = x;
	cursor_y = y;
}
void monitor_getBacktoTemp()
{
  cursor_x = cursor_x_temp;
  cursor_y = cursor_y_temp;
}
