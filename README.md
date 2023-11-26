# Understanding OS Development: A Deep Dive into the Basics
## Introduction
Embarking on the journey of developing an operating system (OS) is a challenging but rewarding endeavor. This blog post serves as a comprehensive README guide, demonstrating a thorough understanding of the key concepts presented in a paper titled "First Steps." The paper outlines the initial steps in OS development, offering insights into the tools, programming languages, and the booting process.

## Setting Up the Development Environment
### Tools
The authors recommend using Ubuntu as the preferred operating system for OS development. Both physical and virtual setups are supported, with VirtualBox as the recommended virtualization tool. A quick setup involves installing essential packages via `apt-get`:

```
sudo apt-get install build-essential nasm genisoimage bochs bochs-sdl
```

### Programming Languages
The OS is developed primarily using the C programming language and GCC. The precise control over generated code and direct access to memory make C the language of choice. The use of the `__attribute__((packed))` attribute in GCC ensures a specific memory layout for structs.

For assembly code, NASM is chosen over GNU Assembler for its preferred syntax. Bash is the scripting language throughout the development process.

### Host Operating System
The code examples assume compilation on a UNIX-like operating system, specifically tested on Ubuntu versions 11.04 and 11.10.

### Build System
Make is the chosen build system, as demonstrated in the provided Makefile examples.

### Virtual Machine
Bochs is recommended as the emulator for the x86 (IA-32) platform due to its debugging features. While other options like QEMU and VirtualBox exist, this guide exclusively employs Bochs.

## Booting Process
Understanding the booting process is crucial for OS development. The process involves a sequence of programs, each more powerful than the last, culminating in the OS itself.

### BIOS
The Basic Input Output System (BIOS) initiates the boot process. Modern OSes bypass BIOS functions, relying on direct hardware interaction. The BIOS performs early diagnostics and transfers control to the bootloader.

### The Bootloader
The bootloader, often split into two parts, facilitates the transition from BIOS to the operating system. The GNU GRand Unified Bootloader (GRUB) is the chosen bootloader, simplifying the development process. GRUB loads the OS as an ELF executable, adhering to the multiboot specification.

### The Operating System
GRUB hands over control to the OS, ensuring it meets the multiboot specification's requirements. The OS gains full control of the computer, marking the culmination of the booting process.

## Implementing Hello Cafebabe: A Comprehensive Guide to the Smallest OS
### Introduction
This blog post serves as a comprehensive README guide to showcase a deep understanding of the paper's section titled "2.3 Hello Cafebabe." This section delves into the implementation of the smallest possible operating system, interacting with the GRUB bootloader. The OS, although minimalistic, performs a significant task — writing the hex value 0xCAFEBABE to the `eax` register.

### Compiling the Operating System
The OS is written in assembly code since C requires a stack, which is not available at this stage. The code is saved in a file named `loader.s`. The assembly code initializes essential constants and loops indefinitely, writing 0xCAFEBABE to the `eax` register.

The following command compiles the assembly code into a 32-bit ELF object file:

```
nasm -f elf32 loader.s
```

### Linking the Kernel
To produce an executable file, the code must be linked with some considerations. The linker script, `link.ld`, ensures that GRUB loads the kernel at an address equal to or larger than 0x00100000 (1 megabyte). The following command links the object file to create the final executable, `kernel.elf`:

```
ld -T link.ld -melf_i386 loader.o -o kernel.elf
```

### Obtaining GRUB
The chosen GRUB version is GRUB Legacy. To use the GRUB Legacy stage2_eltorito bootloader, download it from the source (ftp://alpha.gnu.org/gnu/grub/grub-0.97.tar.gz). Alternatively, for ease, the binary file can be obtained from 
https://github.com/Sandhu-Sahil/Mini-Linux/blob/master/day-1/iso/boot/grub/stage2_eltorito.

Copy the `stage2_eltorito` file to the folder containing `loader.s` and `link.ld`.

### Building an ISO Image
The OS needs to be loaded onto a media that can be used by a virtual or physical machine. In this guide, an ISO image is used. The following steps create the kernel ISO image using the `genisoimage` program:

```
mkdir -p iso/boot/grub
cp stage2_eltorito iso/boot/grub/
cp kernel.elf iso/boot/
```

Create a configuration file, `menu.lst`, in the `iso/boot/grub/` folder. The ISO image is then generated with the following command:

```
genisoimage -R \
            -b boot/grub/stage2_eltorito \
            -no-emul-boot \
            -boot-load-size 4 \
            -A os \
            -input-charset utf8 \
            -quiet \
            -boot-info-table \
            -o os.iso \
            iso
```

### Running Bochs
To test the OS, the Bochs emulator is used with a simple configuration file. Create a configuration file, for example, `bochsrc.txt`, and run Bochs with the following command:

```
bochs -f bochsrc.txt -q
```

The Bochs log file (`bochslog.txt`) can be examined to ensure successful execution.

## Mastering C for OS Development: A Comprehensive Guide
### Introduction
This blog post serves as a comprehensive README guide, demonstrating a deep understanding of the paper's content from the section titled "Getting to C." The section covers the transition from assembly code to the C programming language for OS development. By delving into topics such as setting up a stack, calling C code from assembly, and compiling C code without a standard library, the developer showcases a robust grasp of the intricacies involved in OS development.

### Setting Up a Stack in C
Transitioning to C necessitates the establishment of a stack, a crucial element for non-trivial C programs. The process involves reserving a piece of uninitialized memory in the `.bss` section of the ELF file. The reserved space, labeled as `kernel_stack`, is aligned at 4 bytes, ensuring optimal performance. The stack pointer (`esp`) is then initialized to point to the end of the `kernel_stack` memory.

```
KERNEL_STACK_SIZE equ 4096                  ; size of stack in bytes

section .bss
align 4                                     ; align at 4 bytes
kernel_stack:                               ; label points to beginning of memory
    resb KERNEL_STACK_SIZE                  ; reserve stack for the kernel

mov esp, kernel_stack + KERNEL_STACK_SIZE   ; point esp to the start of the stack (end of memory area)
```

### Calling C Code From Assembly
The next step involves calling a C function from assembly code, utilizing the cdecl calling convention. This convention dictates that function arguments be passed via the stack, with the rightmost argument pushed first. The return value is stored in the `eax` register.

```
/* The C function */
int sum_of_three(int arg1, int arg2, int arg3)
{
    return arg1 + arg2 + arg3;
}

; The assembly code
external sum_of_three   ; the function sum_of_three is defined elsewhere

push dword 3            ; arg3
push dword 2            ; arg2
push dword 1            ; arg1
call sum_of_three       ; call the function, the result will be in eax
```

### Packing Structs in C
To handle configuration bytes efficiently, packed structures are introduced. These structures ensure specific bit orders without any padding. The `__attribute__((packed))` is employed to instruct GCC not to add any padding.

```
struct example {
    unsigned char config;   /* bit 0 - 7   */
    unsigned short address; /* bit 8 - 23  */
    unsigned char index;    /* bit 24 - 31 */
} __attribute__((packed));
```

### Compiling C Code
Compiling C code for the OS demands specific flags to GCC, considering the absence of a standard library. Flags such as `-m32`, `-nostdlib`, and `-fno-builtin` are used to ensure the code doesn't assume the presence of a standard library.

```
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c kmain.c -o kmain.o
```

### Build Tools
Setting up build tools, such as a Makefile, streamlines the compilation and testing process. The Makefile includes rules for compiling C and assembly code, creating an ISO image, and running the OS in the Bochs emulator.

```
make run
```
This command compiles the kernel and boots it up in Bochs, as defined in the Makefile.

## Understanding and Implementing OS Development - Chapter 4
### Introduction
This document summarizes and demonstrates a comprehensive understanding of Chapter 4, "Output," from the paper on OS development. The chapter focuses on displaying text on the console, creating a framebuffer driver, and implementing a serial port driver.

### Interacting with the Hardware
The chapter begins by discussing two ways to interact with hardware: memory-mapped I/O and I/O ports. Memory-mapped I/O involves writing to specific memory addresses, while I/O ports require using assembly code instructions `out` and `in`. The section emphasizes the importance of understanding these concepts for hardware communication.

### The Framebuffer
#### Writing Text
The framebuffer, a hardware device capable of displaying memory on the screen, is introduced. Writing text to the console is explained through memory-mapped I/O. Each cell in the framebuffer represents 16 bits, with ASCII values, foreground, and background colors. An example assembly code snippet is provided to illustrate writing the character 'A' with specific colors to the framebuffer.

#### Moving the Cursor
Moving the cursor is achieved via two I/O ports (0x3D4 and 0x3D5). Assembly code instructions `out` are used to set the cursor's position. A function `fb_move_cursor` is introduced in C to encapsulate this functionality. It also discusses the necessity of wrapping `out` in assembly code to make it accessible from C.

#### The Driver
The concept of a framebuffer driver is introduced, emphasizing the need for a clear interface. A suggested `write` function is proposed, which automatically advances the cursor and scrolls the screen if necessary.

### The Serial Ports
#### Configuring the Serial Port
The serial port, a communication interface for hardware devices, is discussed. Configuration data, including baud rate, error checking, and data length, needs to be sent to the serial port.

#### Configuring the Line
Configuring the line for serial communication involves setting parameters like data length, parity, and stop bits. A function `serial_configure_line` is introduced to simplify this configuration.

#### Configuring the Buffers
Buffer configuration for the serial port is explained. A FIFO queue configuration byte is discussed, specifying buffer size, enable/disable FIFO, and clearing options.

#### Configuring the Modem
The modem control register is introduced for hardware flow control. The chapter explains setting the Ready To Transmit (RTS) and Data Terminal Ready (DTR) pins and provides a configuration byte example.

#### Writing Data to the Serial Port
The process of writing data to the serial port is explained, highlighting the need to check if the transmit FIFO queue is empty before writing.

#### Configuring Bochs
Configuration details for Bochs to save serial port output to a file are discussed. The `com1` configuration in the Bochs file (`bochsrc.txt`) is provided.

#### The Driver
Recommendations for implementing a write function for the serial port are given. A suggestion is made to create a printf-like function and a way to distinguish log message severity.

## Understanding and Implementing OS Development - Chapter 5
### Introduction
This document serves as a comprehensive summary of Chapter 5, "Segmentation," from the OS development paper. Segmentation in x86 architecture involves accessing memory through segments, each defined by a base address and a limit. The chapter explores the details of segmentation, its implementation through descriptor tables, and its relevance in OS development.

### Understanding Segmentation
Segmentation in x86 requires addressing memory through segments specified by a base address and a limit. A 48-bit logical address is used, comprising 16 bits for the segment and 32 bits for the offset within that segment. The resulting linear address is checked against the segment's limit to obtain access to memory. The chapter emphasizes that segmentation is rarely used extensively, especially in comparison to paging.

### Accessing Memory
Memory access is mostly implicit, with six 16-bit segment registers: cs, ss, ds, es, gs, and fs. The chapter provides code examples demonstrating implicit and explicit use of segment registers when accessing memory. It highlights that although you can interchange segment selectors, for the implicit style, segment selectors must be stored in their intended registers.

### The Global Descriptor Table (GDT)
The GDT is introduced as an array of 8-byte segment descriptors. The GDT includes a null descriptor and at least two segment descriptors for code execution (cs) and data access (ds). The Type and Descriptor Privilege Level (DPL) fields play a crucial role in defining segments for code and data.

The relevant segments needed in a minimal setup are described, including the null descriptor, kernel code segment, and kernel data segment. The chapter emphasizes that in this minimal setup, segmentation is primarily used for obtaining privilege levels.

### Loading the GDT
Loading the GDT into the processor involves using the lgdt assembly code instruction. The GDT is represented by a struct containing the address and size, and this information is loaded using the lgdt instruction. The document suggests making this instruction available from C for ease of use.

After loading the GDT, segment registers need to be loaded with their corresponding segment selectors. The structure of segment selectors, including the Requested Privilege Level (RPL), Table Indicator (ti), and offset, is explained. The chapter provides examples of loading the segment selector registers, with a particular emphasis on a "far jump" to load the code segment register (cs).

## Understanding and Implementing OS Development - Chapter 6
### Introduction
This document serves as an in-depth exploration and summary of Chapter 6, "Interrupts and Input," from the OS development paper. Understanding interrupts is crucial for an operating system to handle input effectively, and this chapter provides insights into interrupt handling, the Interrupt Descriptor Table (IDT), and the role of the Programmable Interrupt Controller (PIC).

### Understanding Interrupts
Interrupts are signals that notify the CPU of state changes in hardware devices or due to program errors. They play a crucial role in allowing the operating system to read information from various sources, such as the keyboard, serial port, or timer. The chapter distinguishes between hardware, software, and CPU-generated interrupts.

### Interrupt Handlers and the IDT
Interrupts are managed through the Interrupt Descriptor Table (IDT). The IDT contains handlers for each interrupt, with three types: Task handlers, Interrupt handlers, and Trap handlers. The document focuses on Trap handlers, explaining that they don't disable interrupts, making them suitable for the book's approach.

### Creating an Entry in the IDT
An entry in the IDT for an interrupt handler is a 64-bit structure. The chapter provides a detailed breakdown of the structure, including the offset, privilege level, and size of the gate. It emphasizes the importance of using packed structures for better code readability.

### Handling an Interrupt
When an interrupt occurs, the CPU pushes information onto the stack, including the error code if applicable. The chapter clarifies which interrupts generate error codes and how the interrupt handler retrieves relevant information. It introduces the iret instruction for returning from the interrupt handler.

### Creating a Generic Interrupt Handler
To simplify the creation of interrupt handlers, the document introduces macros using NASM. It demonstrates the use of macros for handling interrupts with and without error codes, emphasizing the common structure shared by all handlers.

### Loading the IDT
Loading the IDT is achieved through the lidt assembly code instruction. The chapter provides a function, `load_idt`, that wraps this instruction and can be easily used from C.

### Programmable Interrupt Controller (PIC)
Configuring the PIC is essential for handling hardware interrupts effectively. The chapter explains the need for remapping interrupts, selecting interrupts to receive, and setting up the correct mode for the PIC. Acknowledging interrupts from the PIC is demonstrated using the byte 0x20.

### Reading Input from the Keyboard
The keyboard generates scan codes rather than ASCII characters. The document introduces a function, `read_scan_code`, to read scan codes from the keyboard's data I/O port. It emphasizes the need to acknowledge interrupts from the keyboard using the `pic_acknowledge` function.

## Implementing User Mode in Your Operating System
### Chapter 7: The Road to User Mode
Congratulations! Your kernel now boots, prints to the screen, and reads from the keyboard. What's next? Typically, a kernel is not meant to handle application logic itself. Instead, it creates abstractions for memory, files, and devices, facilitates system calls on behalf of applications, and schedules processes. This chapter delves into transitioning from kernel mode to user mode, creating a foundation for running user programs.

### Loading an External Program
To execute a program, you need to load its code into memory. In more complex operating systems, drivers and file systems handle this, but in this book, GRUB's module feature simplifies the process. GRUB modules are arbitrary files loaded into memory. To load a module, edit `iso/boot/grub/menu.lst` and add the line:

```
module /modules/program
```

Create the `iso/modules` folder, where the application program will reside.

### GRUB Modules
Modify the "multiboot header" to inform GRUB about modules and align them on page boundaries. Update the `loader.s` file:

```
MAGIC_NUMBER    equ 0x1BADB002      ; magic number constant
ALIGN_MODULES   equ 0x00000001      ; align modules instruction
CHECKSUM        equ -(MAGIC_NUMBER + ALIGN_MODULES)

section .text:
align 4
    dd MAGIC_NUMBER
    dd ALIGN_MODULES
    dd CHECKSUM
```

### Executing a Program
#### A Very Simple Program
A simple program that writes a value to a register serves as a test. Halt Bochs, check the register value in the Bochs log, and verify the program's execution.

```
mov eax, 0xDEADBEEF
jmp $
```

#### Compiling
Compile the program into a flat binary:

```
nasm -f bin program.s -o program
```

Move the `program` file to `iso/modules`.

#### Finding the Program in Memory
Assuming `ebx` holds the multiboot structure pointer, retrieve the address of the first module:

```
int kmain(/* additional arguments */ unsigned int ebx)
{
    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
    unsigned int address_of_module = mbinfo->mods_addr;
}
```

Ensure the module is loaded correctly by checking the `flags` and `mods_count` fields of the `multiboot_info_t` structure.

#### Jumping to the Code
Jump to the loaded code by calling it from C:

```
typedef void (*call_module_t)(void);

call_module_t start_program = (call_module_t) address_of_module;
start_program();
```

### The Beginning of User Mode
The program now runs at the same privilege level as the kernel. However, to enable applications to execute at a different privilege level, you'll need to implement paging and page frame allocation in the upcoming chapters.

## Understanding Virtual Memory: A Brief Overview
### Chapter 8: Unveiling the World of Virtual Memory
Welcome to the realm of virtual memory, a crucial abstraction in operating systems that simplifies application development and enhances system security. The primary goal is to allow processes to address more memory than physically present in the machine while preventing applications from tampering with the kernel or each other's memory. In the x86 architecture, two main techniques handle virtual memory: segmentation and paging. However, paging stands out as the most common and versatile method, which we'll delve into in the upcoming chapter.

### Virtual Memory Through Segmentation?
While paging takes center stage in x86 virtual memory management, segmentation plays a supporting role, especially for allowing code execution under different privilege levels. The idea of using segmentation alone is feasible but comes with challenges. Assigning each user mode process its own segment with specific base addresses and limits ensures memory isolation. However, this method requires contiguous physical memory for a process, posing problems when predicting memory requirements or dealing with dynamic memory growth. Paging, our focus in the next chapter, efficiently addresses these issues, making memory management more flexible and robust.

It's worth noting that x86_64, the 64-bit version of the x86 architecture, has nearly phased out segmentation, highlighting the dominance of paging in modern virtual memory systems.

## Understanding Paging: Unveiling the Magic of Virtual Memory
### Chapter 9: Navigating the World of Paging
Welcome to the intricate world of paging, where logical addresses find their way to physical memory, and access rights are determined with finesse. In this chapter, we will explore the crucial role that paging plays in enabling virtual memory on the x86 architecture.

### Why Paging?
Paging, a cornerstone of x86 virtual memory, offers processes the illusion of a vast memory range (0x00000000 - 0xFFFFFFFF) despite limited physical memory. Through paging, each process uses virtual (linear) addresses, seamlessly translated to physical addresses by the Memory Management Unit (MMU) and the page table. If a virtual address lacks a mapped physical address, a page fault interrupt is raised. While optional, paging is essential for managing memory access levels across different privilege levels, a key feature in modern operating systems.

### Paging in x86
In x86 architecture, paging involves a page directory (PDT), page tables (PT), and page frames (PF). A virtual address's highest 10 bits specify the offset of a Page Directory Entry (PDE) in the current PDT, followed by 10 bits for the Page Table Entry (PTE) offset. The remaining 12 bits denote the offset within the page frame. With the ability to use 4 MB pages or 4 KB pages, paging provides flexibility and efficiency in virtual-to-physical address translation.

#### Identity Paging
The simplest form of paging, identity paging, maps each virtual address to the same physical address. This can be achieved at compile time or runtime, ensuring a straightforward mapping for the first 4 MB of virtual memory.

#### Enabling Paging
Paging is enabled by loading the address of a page directory into cr3 and setting the "paging-enable" bit in cr0. To use 4 MB pages, the Page Size Extensions (PSE) bit in cr4 is set. Sample assembly code illustrates the process.

### Paging and the Kernel
Paging significantly influences the OS kernel's placement and addressing. Placing the kernel at a high virtual memory address, such as 0xC0000000 (3 GB), avoids conflicts with user mode processes. Techniques like identity mapping and higher-half kernels are explored, addressing linker script challenges and runtime considerations.

### Virtual Memory Through Paging
Paging brings two key advantages: fine-grained access control and the illusion of contiguous memory. With marked pages and the appearance of continuity, user mode processes and the kernel can efficiently manage memory, offering a seamless experience.

## Mastering Page Frame Allocation: Unveiling the Heart of Virtual Memory
### Chapter 10: Navigating the World of Page Frame Allocation
Welcome to the crucial realm of page frame allocation, where the operating system manages the delicate balance between available and in-use memory. In this chapter, we will unravel the intricacies of the page frame allocator and explore how it plays a pivotal role in effective memory management.

### Managing Available Memory
#### How Much Memory is There?
Before delving into page frame allocation, we need a comprehensive understanding of available memory. The multiboot structure provided by GRUB is a treasure trove of information about memory regions. Care must be taken not to mark the kernel-used memory as free, and labels exported from the linker script help delineate the kernel's memory boundaries.
```
; Exporting kernel memory labels
kernel_virtual_start = .;
kernel_physical_start = . - 0xC0000000;

; ... (kernel sections)

kernel_virtual_end = .;
kernel_physical_end = . - 0xC0000000;
```

These labels, accessible in assembly or C, serve as crucial pointers for memory management.

#### Managing Available Memory
To keep track of available memory, various strategies can be employed, including bitmaps, linked lists, trees, or the Buddy System. This chapter focuses on the implementation simplicity of bitmaps, where each bit corresponds to a page frame, aiding efficient tracking of free and occupied frames.

### How Can We Access a Page Frame?
When the page frame allocator returns the physical start address of a frame, it's not automatically mapped. To access and manipulate data within the frame, we need to map it into virtual memory. However, the challenge arises when available page tables are full, creating a circular dependency. One solution is to reserve a portion of the first page table, dedicating the last entry for temporary mappings. This allows us to break the circular dependency and efficiently manage page frames.

### A Kernel Heap
With the page frame allocator in place, we can elevate our memory capabilities beyond fixed-size data. Implementation of `malloc` and `free` becomes possible, enabling dynamic memory allocation within the kernel. Inspiration can be drawn from Kernighan and Ritchie's example implementation, with the addition of integrating the page frame allocator for memory allocation and deallocation.

## Understanding User Mode: A Deep Dive into Implementation
### Chapter 11: Navigating the Complexity of User Mode
Congratulations! You've reached the chapter on User Mode, a pivotal milestone in the journey of mastering operating system development. As we delve into the intricacies presented in this chapter, we'll explore the critical steps required to enable and seamlessly operate in User Mode.

### Segments for User Mode
The first crucial step involves enhancing the Global Descriptor Table (GDT) to accommodate User Mode segments. Two additional segments, namely the user code segment and user data segment, are introduced. These segments, with a Descriptor Privilege Level (DPL) of 3, enable the execution of code in User Mode without compromising kernel protection.

```
; User Mode Segment Descriptors
Index   Offset   Name                Address Range               Type    DPL
3       0x18     user code segment   0x00000000 - 0xFFFFFFFF    RX      PL3
4       0x20     user data segment   0x00000000 - 0xFFFFFFFF    RW      PL3
```

### Setting Up For User Mode
Preparing for User Mode involves several essential steps:

- Allocating page frames for code, data, and stack.
- Copying the binary from the GRUB module to the allocated page frames.
- Creating a page directory and page tables to map page frames into memory.

Structuring this information in a process-specific struct, dynamically allocated using the kernel's malloc function, facilitates efficient organization.

### Entering User Mode
To enter User Mode, we utilize the `iret` or `lret` instruction. Setting up the stack is crucial, mimicking the structure as if the processor had raised an inter-privilege level interrupt. Adjusting the stack involves manipulating values like stack segment selector, user mode stack pointer, control flags, code segment selector, and the instruction pointer. Once appropriately configured, executing `iret` transitions the system into User Mode.

### Using C for User Mode Programs
When C becomes the language of choice for user mode programs, considerations shift towards the compilation structure. ELF serves as a viable file format for kernel executables due to GRUB's inherent support. However, for user mode programs, simplicity is introduced by allowing programs to be written in C but compiled into flat binaries instead of ELF binaries.

An assembly code snippet, often placed at offset 0 in the binary, ensures proper execution of the C `main` function. This approach streamlines the development process for user mode programs, enabling seamless integration with the kernel.

#### A C Library
As the complexity of user mode programs grows, thoughts on crafting a small "standard library" arise. While some functionalities necessitate system calls, others, such as those found in `string.h`, do not. This sets the stage for building a foundation for user mode program development.

## Understanding File Systems: A Comprehensive Guide
### Chapter 12: Navigating the Realm of File Systems
Welcome to the enlightening chapter on File Systems. While not mandatory for our operating system, file systems serve as a crucial abstraction, often playing a central role in the functionality of many operating systems, especially UNIX-like systems. As we embark on the exploration of file systems, we'll unravel the intricacies and understand the significance of integrating them into our operating system.

### Why a File System?
Before we delve into the intricacies of file systems, let's address the fundamental questions: How do we specify which programs to run in our OS? What is the first program to run? How do programs handle input and output? The file system provides elegant solutions to these challenges, particularly in UNIX-like systems with their "almost-everything-is-a-file" convention.

### A Simple Read-Only File System
Our journey into file systems begins with a simple read-only file system. Initially, we might have a single file existing only in RAM, loaded by GRUB before the kernel starts. However, as the OS evolves, this approach becomes limiting. A more sophisticated read-only file system involves files with metadata. A utility program can add metadata, such as file type and size, to files during build time. This results in a "file system in a file," constructed by concatenating multiple files with metadata into a single large file, residing in memory post-GRUB loading.

### Inodes and Writable File Systems
When the necessity for a writable file system arises, the concept of an inode becomes invaluable. Inodes, explored further in recommended reading, provide a structured approach to managing file system information. Understanding these structures lays the foundation for developing writable file systems.

### A Virtual File System
To abstract the complexities of concrete file systems, the Virtual File System (VFS) emerges as a powerful tool. VFS primarily handles the path system and file hierarchy, delegating file operations to underlying file systems. The original paper on VFS offers a concise and insightful read. With VFS, the possibility of mounting special file systems on paths like /dev becomes feasible, allowing streamlined management of devices such as keyboards and consoles.

## Decoding System Calls: A Developer's Guide
### Chapter 13: Bridging User Mode and Kernel - The Art of System Calls
Welcome to the enlightening chapter on System Calls. As the gateway for user-mode applications to interact with the kernel, system calls play a pivotal role in resource allocation and operation requests. In this chapter, we'll dissect the intricacies of designing and implementing system calls, offering you a deep understanding of their significance in kernel development.

### Designing System Calls
As kernel developers, the responsibility lies with us to design the system calls that application developers can utilize. We can draw inspiration from established standards like POSIX or take cues from Linux system calls. The design process requires thoughtful consideration to create an API that is both robust and user-friendly. Refer to the "Further Reading" section for valuable references on system call design.

### Implementing System Calls
System calls are conventionally invoked through software interrupts. User applications prepare registers or the stack with appropriate values and trigger a predefined interrupt, transferring control to the kernel. For instance, Linux employs interrupt number 0x80 to signify a system call request.

During the execution of system calls, the privilege level typically shifts from PL3 to PL0 (if the application is in user mode). To facilitate this transition, the Descriptor Privilege Level (DPL) of the entry in the Interrupt Descriptor Table (IDT) for the system call interrupt must allow PL3 access.

When inter-privilege level interrupts occur, the processor automatically pushes critical registers onto the stack. This stack switch involves loading new values for registers ss and esp from the Task State Segment (TSS). Setting up the TSS is a prerequisite for enabling system calls. This can be accomplished in C by configuring the ss0 and esp0 fields of a "packed struct" representing a TSS. Before loading this structure into the processor, a TSS descriptor must be added to the Global Descriptor Table (GDT). Details about TSS and TSS descriptor setup can be found in the Intel manual [33].

To enable system calls, a TSS must be set up before entering user mode. The current TSS segment selector is specified by loading it into the tr register using the ltr assembly instruction. Notably, interrupts must be enabled in user mode to facilitate system call execution. This involves setting the Interrupt Flag (IF) bit in the eflags value on the stack to enable interrupts when the iret instruction is executed.

## Mastering Multitasking: Navigating the Scheduling Landscape
### Chapter 14: Unraveling the Threads of Multitasking
Welcome to the enthralling world of multitasking! In this chapter, we delve into the intricacies of managing multiple processes, exploring the art of creating, scheduling, and switching between them.

### The Multitasking Dilemma
How can an operating system make it seem like multiple processes are running simultaneously? In the absence of multi-core processors or multiple CPUs, the solution is to "fake it" – to switch rapidly between processes, creating the illusion of simultaneous execution. This rapid switching is orchestrated by a crucial component of the operating system known as the scheduling algorithm.

### Creating New Processes: Forks and Execs
The creation of new processes involves two primary system calls: `fork` and `exec`. `fork` duplicates the current process, while `exec` replaces the current process with a new one specified by a file system path. Implementing `exec` is recommended as it aligns closely with the steps outlined in the "Setting up for user mode" section in the "User Mode" chapter.

#### Cooperative Scheduling with Yielding
Cooperative scheduling empowers processes to manage their own switching. Through a system call termed `yield`, a process voluntarily relinquishes control, allowing another process to take the CPU. The key to cooperative scheduling is the mutual cooperation between processes, and the `iret` assembly code instruction facilitates the seamless transfer of control during switching.

### Preemptive Scheduling with Interrupts
Alternatively, preemptive scheduling automates the process switching after a predefined time interval. Leveraging the Programmable Interval Timer (PIT), the operating system configures interrupts to trigger periodic switches between processes. This form of scheduling eliminates the need for processes to manage their own switching, introducing a layer of automation.

#### Programmable Interval Timer (PIT)
The PIT must be configured to raise interrupts at regular intervals. Configuration involves sending a byte to the command port (0x43), specifying options such as using channel 0, setting the divider as a square wave in binary mode. A crucial aspect is the division factor, influencing the frequency of interrupts. The configuration byte, for example, could be 00110110.

#### Separate Kernel Stacks for Processes
To avoid conflicts when switching processes in kernel mode, each process should have its own kernel stack. Similar to user mode stacks, kernel stacks must be updated during process switches. This ensures that interruptions in kernel mode do not result in data corruption due to stack overwrites.

#### Challenges with Preemptive Scheduling
While preemptive scheduling brings automation, it introduces complexities absent in cooperative scheduling. Processes can be interrupted in either user mode or kernel mode, requiring careful handling of stack changes and privilege level updates. Calculating the value of esp before an interrupt becomes crucial, especially when managing switches to processes running in kernel mode.

# Conclusion
Having immersed myself in this extensive exploration of operating system development, I am thrilled to reflect on the wealth of knowledge gained throughout this transformative journey. From comprehending the intricacies of kernel architecture to unraveling the mysteries of multitasking, each section has contributed to a profound understanding of the core elements that constitute an operating system.

As I navigate through the realms of memory management, user mode intricacies, file systems, system calls, and cooperative/preemptive scheduling, the hands-on experience and theoretical insights have solidified my grasp on low-level programming. This journey has not only provided a comprehensive view of operating systems but has also ignited a passion for delving into the depths of computing.

This is not just a mere conclusion but a commencement into the world of endless possibilities. Armed with the skills and insights acquired, I am ready to embark on projects, explore advanced topics, and contribute to the dynamic field of operating system development. The meticulous exploration of each topic has instilled in me a sense of curiosity and a commitment to continuous learning.

As I reflect on the complexities and beauty inherent in operating systems, I realize that this journey is just a stepping stone. Operating system development is an art and a science, and with each line of code, I am refining my craft. The masterful orchestration of processes, memory, and resources is now within my grasp, empowering me to unravel the intricacies behind the devices we use daily.

In the spirit of curiosity and with an eagerness to embrace challenges, I look forward to applying these principles to real-world projects and engaging with the vibrant community of developers. My journey in operating system development has just begun, and I am excited to see where my newfound expertise will lead me. Happy coding!

