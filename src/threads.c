#include <stdio.h>
#include "threads.h"
#include "thread_manager.h"

void threadUART(void) {
  iprintf("Hello world from thread UART\r\n");
  return;
}

void threadOLED(void) {
  return;
}

void threadLED(void) {
  return;
}
