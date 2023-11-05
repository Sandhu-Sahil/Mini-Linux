#include <frame_buffer.h>
#include <serial_port.h>

/* The C function */
int sum_of_three(int a, int b, int c) {
  char buffer[27] = "SANDHU, You are awesome!!!\n";
  fb_write(buffer, 28);
  serial_configure(SERIAL_COM1_BASE, Baud_115200);
  serial_write(SERIAL_COM1_BASE, buffer, 26);
  return a + b + c;
}
