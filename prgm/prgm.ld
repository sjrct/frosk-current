/*
 * prgm.ld
 */

OUTPUT_FORMAT(binary)
ENTRY(main)

SECTIONS
{
	/* reflected as USPACE_BOT in include/kernel/common.both */
	. = 0x100000 - 0x20;

	.hdr . :
	{
		/* header format (reflected in kernel/src/exec.c):
		 *   dword fbe_mark
		 *   dword fbe_version
		 *   qword code_data_size
		 *   qword data_size
		 *   qword entry_point
		 */

		LONG(0xFBE0FBE0);
		LONG(0x1);
		QUAD(SIZEOF(.text) + SIZEOF(.data))
		QUAD(SIZEOF(.bss))
		QUAD(main)
	}

	.text ALIGN(8) : SUBALIGN(8)
	{
		* (.text)
		* (.rdata .rodata)
	}

	.data ALIGN(8) : SUBALIGN(8)
	{
		* (.data)
	}

	.bss ALIGN(8) : SUBALIGN(8)
	{
		* (.bss COMMON)
	}

	_heap_bottom = ALIGN(8);

	/DISCARD/ :
	{
		* (.eh_frame)
		* (.comment)
		* (.debug*)
		* (.note*)
		* (.stab*)
	}
}
