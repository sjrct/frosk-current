//
// kernel/src/leftovers.h
//

#ifndef LEFTOVERS_H
#define LEFTOVERS_H

#include <common.h>
#include <types.h>

#pragma pack(push, 1)
typedef struct {
	qword base;
	qword size;
	dword type;
	dword attrib;
} mmr_t;

typedef union {
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
} interface_t;

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
} disk_data;

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
} leftovers_t;
#pragma pack(pop)

#define leftovers (*(leftovers_t *)LEFTOVERS_LOC)

#endif
