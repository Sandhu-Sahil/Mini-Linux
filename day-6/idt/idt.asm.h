#ifndef _IDTASM_H
#define _IDTASM_H

#include "idt.h"

void idt_load_and_set(struct gdt g);

#endif /* _IDTASM_H */