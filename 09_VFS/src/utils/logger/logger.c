#include "logger.h"
#include <frame_buffer.h>
#include <helpers.h>
#include <serial_port.h>
#include <timer.h>
// #include <string.h>

char message[] = "\
                                                                                \
 /$$      /$$ /$$           /$$      /$$       /$$                              \
| $$$    /$$$|__/          |__/     | $$      |__/                              \
| $$$$  /$$$$ /$$ /$$$$$$$  /$$     | $$       /$$ /$$$$$$$  /$$   /$$ /$$   /$$\
| $$ $$/$$ $$| $$| $$__  $$| $$     | $$      | $$| $$__  $$| $$  | $$|  $$ /$$/\
| $$  $$$| $$| $$| $$  | $$| $$     | $$      | $$| $$  \\ $$| $$  | $$ \\  $$$$/ \
| $$\\  $ | $$| $$| $$  | $$| $$     | $$      | $$| $$  | $$| $$  | $$  >$$  $$ \
| $$ \\/  | $$| $$| $$  | $$| $$     | $$$$$$$$| $$| $$  | $$|  $$$$$$/ /$$/\\  $$\
|__/     |__/|__/|__/  |__/|__/     |________/|__/|__/  |__/ \\______/ |__/  \\__/\
                                                                                \
                                                                                \
";


char message2[] = "\
                                                                                \
        oooooooo8                                oooo oooo                      \
       888           ooooooo   oo oooooo    ooooo888   888ooooo oooo  oooo      \
        888oooooo    ooooo888   888   888 888    888   888   888 888   888      \
               888 888    888   888   888 888    888   888   888 888   888      \
       o88oooo888   88ooo88 8o o888o o888o  88ooo888o o888o o888o 888o88 8o     \
                                                                                \
                  oooooooo8              oooo        o88   o888                 \
                 888           ooooooo    888ooooo   oooo   888                 \
                  888oooooo    ooooo888   888   888   888   888                 \
                         888 888    888   888   888   888   888                 \
                 o88oooo888   88ooo88 8o o888o o888o o888o o888o                \
                                                                                \
                                                                                \
";

void print_screen(s8int buffer[]) { fb_write(buffer, custom_strlen(buffer)); }

void print_serial(s8int buffer[]) {
  serial_write(SERIAL_COM1_BASE, buffer, custom_strlen(buffer));
}

void print_screen_ch(s8int ch) { fb_write(&ch, 1); }

void print_serial_ch(s8int ch) { serial_write(SERIAL_COM1_BASE, &ch, 1); }

void clear_screen() { fb_clear_all(); }

/** fb_splash:
 *  Reset screen and display splash screen
 */
void init_display() {
  fb_clear_all();
  fb_set_color(FB_BLACK, FB_WHITE);
  fb_write(message, 881);
  fb_set_color(FB_BLACK, FB_LIGHT_GREEN);
  fb_write(message2, 1121);
  
  /* Sleep for 5 seconds (500 centiSeconds) */
  sleep(1500);
  fb_set_color(FB_BLACK, FB_WHITE);
}
