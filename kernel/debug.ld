ENTRY(kmain)

_PAGE_SIZE = 0x1000;

MEMORY
{
	/* assures that the kernel fits in valid memory */
	/* org reflected in include/kernel/common.both */
	kernel : org = 0x10000, len = 0x40000
}

SECTIONS
{
	. = ORIGIN(kernel);

	/* .text and .rodata are ro */
	_ro_start = .;
	.text . : SUBALIGN(8)
	{
		* (.text)
	} >kernel

	.rodata ALIGN(8) : SUBALIGN(8)
	{
		* (.rodata)
		* (.rdata)
	} >kernel
	_ro_end = .;

	/* so ro stuff/rw stuff are page aligned */
	. = ALIGN(_PAGE_SIZE);

	/* .data and .bss are rw */
	.data ALIGN(8) : SUBALIGN(8)
	{
		* (.data)
	} >kernel

	.bss ALIGN(8) : SUBALIGN(8)
	{
		_bss_start = .;
		* (.bss)
		* (COMMON)
		_bss_end = .;
	} >kernel

	/DISCARD/ :
	{
		* (.note*)
	}
}
