#define UART0_BASE 0x10000000

#include "s29gl256.h"

// Use a datasheet for a 16550 UART
// For example: https://www.ti.com/lit/ds/symlink/tl16c550d.pdf
#define REG(base, offset) ((*((volatile unsigned char *)(base + offset))))
#define UART0_DR    REG(UART0_BASE, 0x00)
#define UART0_FCR   REG(UART0_BASE, 0x02)
#define UART0_LSR   REG(UART0_BASE, 0x05)
																						
#define UARTFCR_FFENA 0x01                // UART FIFO Control Register enable bit
#define UARTLSR_THRE 0x20                 // UART Line Status Register Transmit Hold Register Empty bit
#define UART0_FF_THR_EMPTY (UART0_LSR & UARTLSR_THRE)

void uart_putc(char c) {
  while (!UART0_FF_THR_EMPTY);            // Wait until the FIFO holding register is empty
  UART0_DR = c;                           // Write character to transmitter register
}

void uart_puts(const char *str) {
  while (*str) {                          // Loop until value at string pointer is zero
    uart_putc(*str++);                    // Write the character and increment pointer
  }
}

void main() {
  UART0_FCR = UARTFCR_FFENA;              // Set the FIFO for polled operation

  unsigned char byte = S29GL256_read_byte(0x60000000);

  if (byte == 0xff) {
      uart_puts("cool");  
  } else {
    uart_puts("ploho");  
  }

//  byte = S29GL256_read_manufacturer_id();

//   if (byte == 0x01) {
//       uart_puts("cool");  
//   } else {
//     uart_puts("ploho");  
//   }

//   byte = S29GL256_read_chip_id();

//   if (byte == 0x22) {
//       uart_puts("cool");  
//   } else {
//     uart_puts("ploho");  
//   }

//   byte = S29GL256_read_byte(0x60000000);

//   if (byte == 0xff) {
//       uart_puts("cool");  
//   } else {
//     uart_puts("ploho");  
//   }

  unsigned char in = 0xaa;
  S29GL256_status_t status = S29GL256_write(&in, 0x60000000, 1, 0);

  byte = S29GL256_read_byte(0x60000000);

  if (byte == 0xff) {
      uart_puts("cool");  
  } else {
    uart_puts("ploho");  
  }

  uart_puts(&byte);
}