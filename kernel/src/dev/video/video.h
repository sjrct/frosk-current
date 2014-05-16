/*
 * kernel/src/dev/video/video.h
 */

#ifndef DEV_VIDEO_VIDEO_H
#define DEV_VIDEO_VIDEO_H

#include "../../fs/fs.h"

void video_detect(void);

extern fs_entry_t * video_file;

#endif
