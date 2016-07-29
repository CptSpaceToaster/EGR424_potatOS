#include <stdio.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "drivers/rit128x96x4.h"
#include "threads.h"
#include "thread_manager.h"

void threadUART(void) {
  unsigned iter = 0;
  volatile unsigned i;
  while(1) {
    iprintf("Hello from thread UART: Pass %d\r\n", iter++);
    for(i=0; i<200000; i++);
  }
}

void threadOLED(void) {
  volatile unsigned i;
  RIT128x96x4StringDraw("potatOS", 40, 40, 15);
  for(i=0; i<1000000; i++);
  RIT128x96x4Clear();
}

void threadLED(void) {
  unsigned light = 0;
  volatile unsigned i;
  while(1) {
    light ^= 1;
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, light);
    for(i=0; i<200000; i++);
  }
}
