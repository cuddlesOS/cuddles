#include "pci.h"
#include "def.h"
#include "io.h"
#include "font.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

void pci_enumerate()
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
		u32 x = inl(PCI_CONFIG_DATA);
		if (x != 0xFFFFFFFF) {
			print(S("bus: ")); print_num_pad(bus, 16, 2, ' ');
			print(S(" dev: ")); print_num_pad(dev, 16, 1, ' ');
			print(S(" vendor: ")); print_num_pad(x & 0xFFFF, 16, 4, ' ');
			print(S(" id: ")); print_num_pad((x >> 16) & 0xFFFF, 16, 4, ' ');
			print(S("\n"));
		}
	}
}
