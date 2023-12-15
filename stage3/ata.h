#ifndef ATA_H
#define ATA_H

#include "def.h"

void ata_recv(u16 *buffer);
void ata_delay();
void ata_init();
void ata_read(u64 lba, u16 sectors, void *buffer);
void *ata_read_full(u64 lba, u64 sectors);

#endif
