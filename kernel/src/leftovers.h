/*
 * kernel/src/leftovers.h
 */

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
	/* there is more here, but I don't really care */
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

typedef struct video_mode_info {
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

typedef struct {
	word mode_attr;
	byte wina_attr;
	byte winb_attr;
	word win_granularity;
	word win_size;
	word wina_seg;
	word winb_seg;
	dword far_win_pos_func;
	word bytes_per_scanline;

	/* offset 12h */
	word width;
	word height;
	byte char_width;
	byte char_height;
	byte planes;
	byte bits_per_pixel;
	byte banks;
	byte memory_model_type;
	byte bank_size;
	byte image_pages;
	byte is_vbe3_0;

	/* offset 1Fh */
	byte red_mask_size;
	byte red_field_pos;
	byte green_mask_size;
	byte green_field_size;
	byte blue_mask_size;
	byte blue_field_size;
	byte resv_mask_size;
	byte resv_mask_pos;
	byte direct_color_mode_info;

	/* offset 28h */
	dword linear_buf;
	dword offscreen_loc;
	word  offscreen_size;
	word  bytes_per_scanline_linear;
	byte  imgs_banked;
	byte  imgs_linear;
	byte  dir_red_mask_size;
	byte  dir_red_mask_pos;
	byte  dir_green_mask_size;
	byte  dir_green_mask_pos;
	byte  dir_blue_mask_size;
	byte  dir_blue_mask_pos;
	byte  dir_resv_mask_size;
	byte  dir_resv_mask_pos;
	dword max_pix_clock;

	/* offset 42h */
	byte reserved[0xBE]; /* pads to 256 bytes */
} __attribute__((packed)) video_mode_info_t;

/* reflected in 'boot/src/leftovers.inc' */
typedef struct {
	word  partition_ds;
	word  partition_si;
	byte  drive_letter;
	byte  mm_entry_size;
	word  mm_size;
	word  kernel_size;
	disk_data dd;
	video_mode_info_t vmi;
	mmr_t memory_map[];
} __attribute__((packed)) leftovers_t;

#define leftovers (*(leftovers_t *)LEFTOVERS_LOC)

#endif
