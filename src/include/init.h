#pragma once

// include all headers

// std
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// memory
#include <memory.h>
#include <mmu/pmm.h>
#include <mmu/vmm.h>

// device
#include <io.h>
#include <pci.h>
#include <serial.h>
#include <cpuid.h>
#include <pcspeaker.h>

// print
#include <cursor.h>
#include <screen.h>
#include <fb_terminal.h>
#include <panic.h>
#include <shell.h>

// interrupts
#include <interrupts.h>
#include <timer.h>
#include <keyboard.h>

// multiboot2
#include <multiboot2.h>
#include <framebuffer.h>

// fs
#include <fs/fs.h>

// task sheduler
#include <task_sheduler.h>
