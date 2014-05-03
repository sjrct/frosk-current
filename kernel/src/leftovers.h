//
// kernel/src/leftovers.h
//

#ifndef LEFTOVERS_H
#define LEFTOVERS_H

#include <types.h>
#include <common.h>

typedef struct {
	qword base;
	qword size;
	dword type;
	dword attrib;
} __attribute__((packed)) mmr_t;

typedef struct bm_edd {
	word io_base;
	word ctrl_base;
	byte flags;
	// there is more here, but I don't really care
} __attribute__((packed)) bm_edd_t;

typedef union bm_interface  {
	struct {
		word base;
		word pack[3];
	} isa;
	
	struct {
		byte bus;
		byte device;
		byte function;
		byte pack[5];
	} pci;
} __attribute__((packed)) bm_interface_t;

typedef union bm_device {
	struct {
		byte slave;
		byte lun;
	} atapi;

	byte scsi_lun;
	qword firewire_guid;
	qword fibre_wwn;
} __attribute__((packed)) bm_device_t;

typedef struct {
	word size;
	word flags;
	dword cylinders;
	dword heads;
	dword spt;
	qword sectors;
	word bps;
	dword edd;
	word sig;
	byte dpi_length;
	byte pack[3];
	byte hostbus[4];
	byte if_type[8];
	qword if_path;
	qword device_path;
	byte pack2;
	byte checksum;
} __attribute__((packed)) disk_data;

// reflected in 'boot/src/leftovers.inc'
typedef struct {
	word  partition_ds;
	word  partition_si;
	byte  drive_letter;
	byte  mm_entry_size;
	word  mm_size;
	word  kernel_size;	
	disk_data dd;	
	mmr_t memory_map[];
} __attribute__((packed)) leftovers_t;

#define leftovers (*(leftovers_t *)LEFTOVERS_LOC)

#endif
