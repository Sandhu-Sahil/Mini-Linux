#include "frameBuffer/fb.h"
#include "deviceTracker/device.h"
#include "serialPort/serial.h"
#include "gdt/gdt.h"

void init(void) {
	init_gdt();
	fb_init();
	serial_init();
}

void clear(void){
	int i;
	unsigned char s[] = " ";
	for (i = 0; i < FB_NUM_CELLS; ++i){
		dev_write(0, s, 1);	
    }
}

void kmain(void){
	unsigned char s[] = "This is MINI Sandhu, mini version of myself Sahilsher Singh Sandhu.";
	//I am a 3rd year Computer Science student at University of Waterloo. I am a passionate programmer and love to code. I am a quick learner and love to learn new things. I am a team player and love to work in a team. I am a hard worker and love to work hard. I am a good listener and love to listen to people. I am a good speaker and love to speak to people. I am a good leader and love to lead people. I am a good follower and love to follow people. I am a good friend and love to be friends with people. I am a good person and love to be a person. I am a good human and love to be a human. I am a good human being and love to be a human being. I am a good being and love to be a being. I am a good son and love to be a son. I am a good brother and love to be a brother. I am a good friend and love to be a friend. I am a good student and love to be a student. I am a good programmer and love to be a programmer.";

	init();

	clear();

	// fb_change_color(FB_LIGHT_CYAN, FB_RED);

	dev_write(0, s, sizeof(s));

	dev_write(1, s, sizeof(s));
}