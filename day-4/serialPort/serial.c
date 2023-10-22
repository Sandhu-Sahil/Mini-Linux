#include "serial.h"
#include "../IO_in-out/io.h"
#include "../deviceTracker/device.h"

static struct device serial_dev =	{1, serial_write};

void serial_cfg_baudrate(unsigned short com, unsigned short divisor){
	/*
	 * Tell the serial controller that we are about to
	 * set the divisor of the baudrate. This is done
	 * by writing to the data register and interrupt
	 * enabling register. These two bytes will hold the
	 * divisor data WHILE the DLAB bit is set.
	 */
	outb(SERIAL_LINE_CMD_PORT(com), SERIAL_LINE_ENABLE_DLAB);
	outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00FF);
	outb(SERIAL_DATA_PORT(com), divisor & 0x00FF);
	outb(SERIAL_LINE_CMD_PORT(com), 0x0);
}

void serial_cfg_line(unsigned short com){
	/*
	 * Line control register layout:
	 *
	 *	| 7 | 6 | 5 4 3 | 2 | 1 0 |
	 *	| d | b | pty   | s | dl  |
	 *
	 * Where:
	 *	+ d => Enables the DLAB
	 *	+ b => Enables break control
	 *	+ prty => The number of parity bits to use
	 *	+ s => The number of stop bits to use
	 *		(s = 0 equals 1, s = 1 equals 1.5 or 2)
	 *	+ dl => Describes the length of the data
	 *
	 * The most commonly used value is 8 bits lenght, no
	 * parity bit, 1 stop bit and break control disabled,
	 * i.e. 0x03.
	 */
	outb(SERIAL_LINE_CMD_PORT(com), 0x03);		
}

void serial_cfg_buf(unsigned short com){
	/*
	 * Buffer config register layout:
	 *
	 *	| 7 6 | 5  | 4 | 3   | 2   | 1   | 0 |
	 *	| lvl | bs | r | dma | clt | clr | e |
	 *
	 * Where:
	 *	+ lvl => Size of the buffer in bytes
	 *	+ bs => Enables 64 byte FIFO
	 *	+ r => Reserved for future use
	 *	+ dma => DMA mode selection
	 *	+ clt => Clear the transmission FIFO
	 *	+ clr => Clear the reception FIFO
	 *	+ e => Enables or disables the FIFO buffer
	 *
	 * The default value by convention, for now, will be
	 * one which enables the FIFO, clears both buffers and
	 * uses 14 bytes of size, i.e. 0xC7.
	 */
	 outb(SERIAL_FIFO_CMD_PORT(com), 0xC7);
}

void serial_cfg_modem(unsigned short com) {
	/*
	 * Modem control register:
	 *
	 *	| 7 | 6 | 5  | 4  | 3   | 2   | 1   | 0   |
	 *	| r | r | af | lb | ao2 | ao1 | rts | dtr |
	 *
	 * Where:
	 *	+ r => Reserved
	 *	+ af => Enables autoflow control (not used)
	 *	+ lb => Enables loopback mode. In loopback mode
	 *		the controller disconnects the receiver
	 *		serial input and redirects it to the
	 *		transmitter. Used for debugging
	 *	+ ao2 => Auxiliary output 2, used for receiving
	 *		interrupts
	 *	+ ao1 => Auxiliary output 1
	 *	+ rts => Ready to transmit (RTS) bit
	 *	+ dtr => Data terminal ready (DTR) bit
	 *
	 * The default value to use will be 0x03
	 */
	 outb(SERIAL_MODEM_CMD_PORT(com), 0x03);
}

void serial_cfg_port(const struct com_port *p) {
	serial_cfg_baudrate(p->com, p->divisor);
	serial_cfg_line(p->com);
	serial_cfg_buf(p->com);
	serial_cfg_modem(p->com);
}

unsigned char serial_is_tx_fifo_empty(unsigned short com){
	/* bit 5 of line status register indicates if queue is empty */
	return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_init(void){
	struct com_port com1 = { SERIAL_COM1_BASE, BAUD_RATE_DIVISOR }; 

	register_device(serial_dev);
	serial_cfg_port(&com1);
}

int serial_data_available() {
    return inb(SERIAL_COM1_BASE + 5) & 0x01;
}

// Function to read a character from the serial port
char serial_read_char() {
    while (!serial_data_available());  // Wait for data to be available
    return inb(SERIAL_COM1_BASE);
}

void serial_write(unsigned char *b, unsigned short len){
	int i = 0;

	/*
	 * TODO: Since there is no wait-queue or event dispatching
	 * mechanism yet, we will spin until data is available on
	 * a given COM port.
	 */
	while (!serial_is_tx_fifo_empty(SERIAL_COM1_BASE));

	while (i < len)
	{
		outb(SERIAL_COM1_BASE, b[i++]);
	}
	// unsigned char received_char = serial_read_char();
	// unsigned char a1 = serial_read_char();
	// unsigned char a2 = serial_read_char();
	// unsigned char a3 = serial_read_char();
	// unsigned char a4 = serial_read_char();
	// unsigned char a5 = serial_read_char();
	// dev_write(0, &received_char, sizeof(received_char));
	// dev_write(0, &a1, sizeof(a1));
	// dev_write(0, &a2, sizeof(a2));
	// dev_write(0, &a3, sizeof(a3));
	// dev_write(0, &a4, sizeof(a4));
	// dev_write(0, &a5, sizeof(a5));
}