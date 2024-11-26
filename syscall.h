#pragma once
#include "syscalldefs.h"
#include "interrupts.h"
#include "errno.h"
#include "kprintf.h"

void syscall_handler(struct InterruptContext* ctx);