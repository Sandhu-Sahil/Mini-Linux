#ifndef _IDTASM_H
#define _IDTASM_H

#include "interrupt.h"

void enable_interrupts(void);
void disable_interrupts(void);
void switch_to_kernel_stack(void (*continuation)(uint32_t), uint32_t data);

#endif /* _IDTASM_H */