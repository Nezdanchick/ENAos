#pragma once

// include all headers

// std
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// memory
#include <memory.h>

// device
#include <io.h>
#include <pci.h>
#include <serial.h>
#include <pcspeaker.h>

// print
#include <screen.h>
#include <panic.h>

// interrupts
#include <interrupts.h>
#include <timer.h>
#include <keyboard.h>

// multiboot2
#include <multiboot2.h>
#include <framebuffer.h>

extern uint64_t kernel_start;
extern uint64_t kernel_end;
void init(uint64_t multiboot_addr, uint64_t multiboot_magic);
