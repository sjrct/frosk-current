/*
 * kernel/src/dev/video/video.c
 */

#include "video.h"
#include "../dev.h"
#include "../../leftovers.h"

fs_entry_t * video_file;

#define MODE_TEXT 1

void video_detect(void)
{
	if (leftovers.vmi.mode_attr & MODE_TEXT) {
		/* text mode */
		video_file = fs_enter("tvb",
				(void *)(ulong)(leftovers.vmi.wina_seg * 0x10),
				leftovers.vmi.width * leftovers.vmi.height * 2,
				dev_dir
		);
	} else {
		/* TODO pixel mode */
	}
}

