#ifndef SIMD_H
#define SIMD_H

int sse_available(void);
int sse2_available(void);
int sse3_available(void);
int fxsr_available(void);
int mmx_available(void);

void enable_sse(void);

#endif
