#pragma once

#include "utils.h"

u32 pci_read_addr(u32 addr, u32 reg);
void pci_write_addr(u32 addr, u32 reg, u32 val);
u32 pci_read(u32 bus, u32 dev, u32 func, u32 reg);
u32 pci_scan_for_device(unsigned klass, unsigned subclass);