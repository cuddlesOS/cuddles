#ifndef PCI_H
#define PCI_H

#include "def.h"

typedef struct {
	u8 bus;
	unsigned int dev : 5;
	u16 vendor;
	u16 id;
	u8 class;
	u8 subclass;
} pci_dev;

extern usize pci_num_devices;
extern pci_dev *pci_devices;

void pci_init();

#endif
