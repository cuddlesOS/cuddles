#include "pci.h"
#include "def.h"
#include "io.h"
#include "font.h"
#include "heap.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

usize pci_num_devices = 0;
pci_dev *pci_devices = nil;

void pci_init()
{
	typedef struct __attribute__((packed)) {
		u8 offset;
		unsigned int func : 3;
		unsigned int dev : 5;
		u8 bus;
		unsigned int reserved : 7;
		bool enable : 1;
	} pci_config_addr;

	for (int bus = 0; bus < 256; bus++)
	for (int dev = 0; dev < 32; dev++) {
		pci_config_addr addr = {
			.offset = 0,
			.func = 0,
			.dev = dev,
			.bus = bus,
			.enable = true,
		};

		outl(PCI_CONFIG_ADDRESS, BITCAST(addr, pci_config_addr, u32));
		u32 reg_0 = inl(PCI_CONFIG_DATA);
		if (reg_0 == 0xFFFFFFFF)
			continue;

		addr.offset = 0x8;
		outl(PCI_CONFIG_ADDRESS, BITCAST(addr, pci_config_addr, u32));
		u32 reg_2 = inl(PCI_CONFIG_DATA);

		pci_devices = krealloc(pci_devices, ++pci_num_devices * sizeof *pci_devices);
		pci_devices[pci_num_devices-1] = (pci_dev) {
			.bus = addr.bus,
			.dev = addr.dev,
			.vendor = reg_0 & 0xFFFF,
			.id = (reg_0 >> 16) & 0xFFFF,
			.class = reg_2 >> 24,
			.subclass = (reg_2 >> 16) & 0xff,
		};
	}
}
