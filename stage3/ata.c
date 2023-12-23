#include "ata.h"
#include "halt.h"
#include "heap.h"
#include "io.h"
#include "font.h"

#define ATA_IO_DATA 0
#define ATA_IO_ERR 1
#define ATA_IO_FEATURES 1
#define ATA_IO_SECTORS 2
#define ATA_IO_LBA_LOW 3
#define ATA_IO_LBA_MID 4
#define ATA_IO_LBA_HIGH 5
#define ATA_IO_HEAD 6
#define ATA_IO_STATUS 7
#define ATA_IO_COMMAND 7

#define IO_ATA0_DATA 0x1F0
#define IO_ATA0_CTRL 0x3F6

#define ATA_CMD_IDENTIFY 0xEC
#define ATA_CMD_READ_SECTORS_EXT 0x24

typedef struct __attribute__((packed)) {
	bool err : 1;
	bool idx : 1;
	bool corr : 1; // corrected data
	bool drq : 1;  // has data or ready to accept data
	bool srv : 1;  // overlapped
	bool df : 1;   // drive fault
	bool rdy : 1;  // clear if spun down or error
	bool bsy : 1;  // preparing/busy
} ata_status;

typedef struct {
	unsigned int bits : 4;
	bool doll : 1;
	bool one0 : 1;
	bool lba : 1;
	bool one1 : 1;
} __attribute__((packed)) ata_head;

void ata_recv(u16 *buffer)
{
	for (;;) {
		ata_status status = BITCAST(inb(IO_ATA0_DATA + ATA_IO_STATUS), u8, ata_status);

		if (status.err) {
			u8 err = inb(IO_ATA0_DATA + ATA_IO_ERR);

			str errors[] = {
				S("address mark not found\n"),
				S("track zero not found\n"),
				S("aborted command\n"),
				S("media change request\n"),
				S("id not found\n"),
				S("media changed\n"),
				S("uncorrectable data error\n"),
				S("bad block detected\n"),
			};

			for (int i = 0; i < 8; i++)
				if (err & (1 << i))
					print(errors[i]);

			panic(S("ata0-witch error\n"));
		} else if (status.drq)
			break;
	}

	for (int i = 0; i < 256; i++)
		buffer[i] = inw(IO_ATA0_DATA + ATA_IO_DATA);
}

void ata_delay()
{
	for (int i = 0; i < 15; i++)
		inb(IO_ATA0_DATA + ATA_IO_STATUS);
}

void ata_init()
{
	u8 floating = inb(IO_ATA0_DATA + ATA_IO_STATUS);
	if (floating == 0xFF)
		panic(S("ata0 floating\n"));

	outb(IO_ATA0_DATA + ATA_IO_HEAD, BITCAST(((ata_head) {
		.bits = 0,
		.doll = false,
		.one0 = true,
		.lba = false,
		.one1 = true,
	}), ata_head, u8));
	ata_delay();
	outb(IO_ATA0_DATA + ATA_IO_LBA_LOW, 0);
	outb(IO_ATA0_DATA + ATA_IO_LBA_MID, 0);
	outb(IO_ATA0_DATA + ATA_IO_LBA_HIGH, 0);
	outb(IO_ATA0_DATA + ATA_IO_COMMAND, ATA_CMD_IDENTIFY);

	u8 status_byte = inb(IO_ATA0_DATA + ATA_IO_STATUS);
	if (status_byte == 0)
		panic(S("no ata0-witch drive\n"));

	while (BITCAST(status_byte, u8, ata_status).bsy)
		status_byte = inb(IO_ATA0_DATA + ATA_IO_STATUS);

	if (inb(IO_ATA0_DATA + ATA_IO_LBA_MID) != 0 || inb(IO_ATA0_DATA + ATA_IO_LBA_HIGH) != 0)
		panic(S("ata0-witch is not ATA\n"));

	u16 *idvec = malloc(256 * sizeof *idvec);
	ata_recv(idvec);

	if (!(idvec[83] & (1 << 10)))
		panic(S("ata0-witch does not support LBA48 mode\n"));

	// u64 lba48_sectors = *(u64 *) &idvec[100];
	// print_num(lba48_sectors, 10, 0); print("\n");

	free(idvec);

	print(S("ata0-witch initialized\n"));
}

void ata_read(u64 lba, u16 sectors, void *buffer)
{
	outb(IO_ATA0_DATA + ATA_IO_HEAD, BITCAST(((ata_head) {
		.bits = 0,
		.doll = false,
		.one0 = true,
		.lba = true,
		.one1 = true,
	}), ata_head, u8));
	ata_delay();
	outb(IO_ATA0_DATA + ATA_IO_SECTORS, sectors >> 8);
	outb(IO_ATA0_DATA + ATA_IO_LBA_LOW, (lba >> 24) & 0xff);
	outb(IO_ATA0_DATA + ATA_IO_LBA_MID, (lba >> 32) & 0xff);
	outb(IO_ATA0_DATA + ATA_IO_LBA_HIGH, (lba >> 40) & 0xff);
	outb(IO_ATA0_DATA + ATA_IO_SECTORS, sectors & 0xff);
	outb(IO_ATA0_DATA + ATA_IO_LBA_LOW, lba & 0xff);
	outb(IO_ATA0_DATA + ATA_IO_LBA_MID, (lba >> 8) & 0xff);
	outb(IO_ATA0_DATA + ATA_IO_LBA_HIGH, (lba >> 16) & 0xff);
	outb(IO_ATA0_DATA + ATA_IO_COMMAND, ATA_CMD_READ_SECTORS_EXT);

	for (u16 i = 0; i < sectors; i++)
		ata_recv(buffer + i*512);
}

void *ata_read_full(u64 lba, u64 sectors)
{
	void *buffer = malloc(512 * sectors);

	for (u64 off = 0; off < sectors; off += 0x10000) {
		u64 sects = sectors - off;
		if (sects >= 0x10000)
			sects = 0;
		ata_read(lba + off, (u16) sects, buffer + off * 512);
	}

	return buffer;
}
