#ifndef MONITOR_H
#define MONITOR_H

#include "common.h"

void monitor_put(char c);
void monitor_clear();
void monitor_write(char *c);
void monitor_put_with_colors(char c, u8int backColour, u8int foreColour);
void monitor_write_with_colors(char *c, u8int backColour, u8int foreColour);
void monitor_welcome_message();
void monitor_command_prompt();
void monitor_color_write(char *c, u8int backColour, u8int foreColour);
void monitor_write_dec_with_colors(u32int n, u8int backColor, u8int foreColor);
void monitor_setTempCursorLocation(u8int x,u8int y);
void monitor_getBacktoTemp();

#endif
