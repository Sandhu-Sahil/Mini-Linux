#include "fb.h"
#include "../IO_in-out/io.h"
#include "../deviceTracker/device.h"

static struct framebuffer *fb =		{0};
static unsigned char curr_color =	FB_BLACK | FB_WHITE;
static unsigned short cursor_idx =	0;
struct device fbdev =			{0, fb_write};

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg){
	fb[i].c = c;
	fb[i].colors = (((fg & 0x0F) << 4) | (bg & 0x0F)); 
}

void fb_move_cursor(unsigned short pos){
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_CMD);
	outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF)); // pos >> 8 = right shift and gives higher 8 bits, then and operaton with 0000000011111111 gives lower 8 bits conformation make other bits 0
	outb(FB_COMMAND_PORT, FB_LOW_BYTE_CMD);
	outb(FB_DATA_PORT, pos & 0x00FF); // same as above but no need to shift
}

void fb_init(void){
	register_device(fbdev);
	fb = (struct framebuffer *) 0x000B8000;
}

void fb_change_color(unsigned char fg, unsigned char bg){
	curr_color &= (fg | bg);
}

void scroll(){
	unsigned int i;
	for (i = 0; i < FB_NUM_CELLS - FB_TERM_WIDTH; i++){
		fb[i].c = fb[i + FB_TERM_WIDTH].c;
		fb[i].colors = fb[i + FB_TERM_WIDTH].colors;
	}
	for (i = FB_NUM_CELLS - FB_TERM_WIDTH + 1; i <= FB_NUM_CELLS; i++){
		fb_write_cell(i, ' ', FB_BLACK, FB_WHITE);
	}
}

void fb_write(unsigned char *buf, unsigned short len){
	unsigned int i;
	unsigned char fg = curr_color & 0xF0;
	unsigned char bg = curr_color & 0x0F;
	for (i = 0; i < len; ++i)
	{
		fb_write_cell(cursor_idx, buf[i], fg, bg);
		// fb_write_cell(cursor_idx, 'a', fg, bg);
		if (cursor_idx == FB_NUM_CELLS - 1)
		{
			scroll();
			cursor_idx = cursor_idx - FB_TERM_WIDTH + 1;
		}
		else{
			cursor_idx++;
		}
	}
	fb_move_cursor(cursor_idx);
}