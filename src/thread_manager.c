#include <stdio.h>
#include <stdlib.h>
#include "thread_manager.h"

#define STACK_SIZE 4096

typedef struct thread {
  struct thread *next;
  void (*callback)(void);
  char *stack;
  unsigned reg[40];
} thread_t;

typedef struct thread_table {
  struct thread *current;
  struct thread *previous;
} thread_table_t;

// global thread table
thread_table_t table;

// pointer to a thread that needs to be deleted
// (set to NULL if nothing needs to be deleted)
volatile thread_t *delete = NULL;

volatile int flag = 0;

/*
 * Private function to run and clean up a thread
 */
void thread_runner(void) {
  iprintf("  HALLELUJAH!\r\n");
  // call the current thread's entry point
  (*(table.current->callback))();
  // clean up a thread after it exits
  remove_current_thread();
  // yield to the schedule
  yield();
}

/*
 * Initialize a thread table for main to use
 */
void init_thread_table(void) {
  table.current = NULL;
  table.previous = NULL;
}

/*
 * Allow main to add a thread to the thread table
 */
void init_thread(void (*callback)(void)) {
  thread_t *T = malloc(sizeof(thread_t));
  T->callback = callback;
  T->stack = malloc(STACK_SIZE) + STACK_SIZE;

  if (NULL == table.current) {
    table.current = T;
    table.previous = T;
  } else {
    table.previous->next = T;
    table.previous = T;
  }
  table.previous->next = table.current;

  asm volatile(
    "ldr    r1, [%0]\n"             // read in temp stack pointer

    "movw   r2, 0x0000\n"
    "movt   r2, 0x0100\n"
    "stmdb  r1!, {r2}\n"            // initialize xpsr

    "ldr    r2, =thread_runner\n"
    "stmdb  r1!, {r2}\n"            // store address to return to (A2R2)

    "subs   r1, #24\n"              // decrement stack poitner by 24
    "str    r1, [%0]\n"

    "stm    %1, {r1}"               // initialize registers
    : : "r" (&(T->stack)), "r" (T->reg)
  );
}

/*
 * Remove a thread from the table/schedule ring when it's done
 */
void remove_current_thread(void) {
  table.previous->next = table.current->next;
  // mark thing to be deleted
  delete = table.current;
  table.current = table.previous;
}

/*
 * SVC handler/scheduler routine
 * Rotate to the next thread, and clean up dead ones.
 */
void schedule(void) {
  iprintf(" A\r\n");
  // perform janitorial duties
  if (delete) {
    free((thread_t *)delete->stack);
    free((thread_t *)delete);
    if (delete == table.current) {
      while(1);
      // this should return to main somehow?
    }
    delete = NULL;
  }

  if (flag) {
    //save stack
    asm volatile (
      "mrs    r1, psp\n"
      "stm    %0, {r1,r4-r12}\n"
      : : "r" (table.current->reg)
    );
  }
  flag = 1;

  iprintf(" B\r\n");

  //rotate schedule
  table.previous = table.current;
  table.current = table.current->next;

  iprintf(" C\r\n");

  //restore stack
  asm volatile (
    "ldm    %0, {r1,r4-r12}\n"
    "msr    psp, r1\n"

    "movw   lr, 0xFFFD\n"
    "movt   lr, 0xFFFF\n" // return with unprivileged access
    "bx lr"
    : : "r" (table.current->reg)
  );
}

/*
 * Call the scheduler, and give control to the next thread
 */
void yield(void) {
  asm volatile("svc #0");
}
