#define UART0_BASE_ADDR 0x10000000

#define REG(base, offset) ((*((volatile unsigned char *)(base + offset))))

#define UART0_DR    REG(UART0_BASE_ADDR, 0x00)
#define UART0_FCR   REG(UART0_BASE_ADDR, 0x02)
#define UART0_LSR   REG(UART0_BASE_ADDR, 0x05)
																						
#define UARTFCR_FFENA 0x01 // FIFO Control Register enable bit
#define UARTLSR_THRE 0x20 // Line Status Register Transmit Hold Register Empty bit
#define UART0_FF_THR_EMPTY (UART0_LSR & UARTLSR_THRE)

void uart_putc(char c) {
  while (!UART0_FF_THR_EMPTY) {
  }
  
  UART0_DR = c;                           
}

static void uart_puts(const char *str) {
  while (*str) {                        
    uart_putc(*str++);                    
  }
}

int main() {
  UART0_FCR = UARTFCR_FFENA;

  uart_puts("Hello, world!");
 
  return 0;
}