#include <stdio.h>
#include <stdlib.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "drivers/rit128x96x4.h"

#include "thread_manager.h"
#include "threads.h"

void uart_init(void) {
  // set GPIO A0 and A1 as UART pins.
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  // configure the UART for 115200, 8-N-1 operation.
  UARTConfigSetExpClk(
    UART0_BASE,
    SysCtlClockGet(),
    115200,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)
  );
}

void led_init(void) {
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
}

int main(void) {
  int c;
  int light=0;
  // set the clocking to run directly from the crystal.
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                 SYSCTL_XTAL_8MHZ);

  // initialize the OLED display and display "IR Sensor Demo" on the OLED screen.
  RIT128x96x4Init(1000000);
  RIT128x96x4StringDraw("potatOS", 38,  0, 15);

  // enable peripherals
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  // initialize peripherals.
  uart_init();
  led_init();

  // prepare thread table
  iprintf("1\r\n");
  init_thread_table();
  iprintf("2\r\n");
  init_thread(threadUART);
  iprintf("3\r\n");

  // init systick

  // register SVC handler
  IntRegister(FAULT_SVCALL, schedule);

  // Enable global Interrupts
  IntMasterEnable();
  iprintf("4\r\n");

  yield();
  iprintf("5\r\n");


  // Commands for user control mode printed to terminal using iprintf()
  iprintf("Hello world\r\n");

  while(1) {
    switch ((c = getchar())) {
      case EOF:
        clearerr(stdin);
        break;

      case 32:
        light ^= 1;
        iprintf("A key was pressed: %d\r\n", light);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, light);
        break;
    }
  }

  exit(0);
}
