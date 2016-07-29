#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

void init_thread_table(void);
void init_thread(void (*callback)(void));
void remove_current_thread(void);
void schedule(void);
void yield(void);

#endif // THREAD_MANAGER_H
