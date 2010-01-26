OUTPUT_FORMAT("elf32-bigarm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
 
/* Sections area */ 
MEMORY {
	ios_info_table		: ORIGIN = 0x0, LENGTH = 0x4000
	exe(rwx)		: ORIGIN = 0x20007200, LENGTH = 0xe00
	ram(rwx)		: ORIGIN = 0x200082ac, LENGTH = 0x0
	ios_exe(rwx)		: ORIGIN = 0x20000000, LENGTH = 0x7108
	bss(rwx)		: ORIGIN = 0x2004f9ec, LENGTH = 0x614
}

__exe_start_virt__		= 0x20007200;
__exe_start_phys__		= 0x13a17200;
__ram_start_virt__		= 0x200082ac;
__ram_start_phys__		= 0x13a182ac;
__bss_start_virt__		= 0x2004f9ec;

/* FFS addresses */
ffs_handle_open			= 0x20001810;
ffs_handle_close		= 0x20001fc0;
ffs_handle_read			= 0x200018d8;
ffs_handle_write		= 0x20001a34;
ffs_handle_seek			= 0x20001c18;
ffs_handle_ioctl		= 0x20001c74;
ffs_handle_ioctlv		= 0x20001ed8;


SECTIONS
{
	.debug_aranges 0 : {
		*(.debug_aranges)
	}
	.debug_pubnames 0 : {
		*(.debug_pubnames)
	}
	.debug_info 0 : {
		*(.debug_info)
	}
	.debug_abbrev 0 : {
		*(.debug_abbrev)
	}
	.debug_line 0 : {
		*(.debug_line)
	}
	.debug_frame 0 : {
		*(.debug_frame)
	}
	.debug_str 0 : {
		*(.debug_str)
	}
	.debug_loc 0 : {
		*(.debug_loc)
	}
	.debug_macinfo 0 : {
		*(.debug_macinfo)
	}
	.note.arm.ident 0 : {
		KEEP (*(.note.arm.ident))
	}

	/*
	 * FFS functions
	 */

	.ffs_printf 0x200060bc : {
		*(.ffs_printf)
	} > ios_exe

	/*
	 * End of FFS functions
	 */

	.init __exe_start_virt__ : AT (__exe_start_phys__) {
		. = .; KEEP (*(.init))
	} > exe

	.text ALIGN (0x4) : { 
		*(.text.entry)
		*(.text*)
		*(.gnu.warning) 
		*(.gnu.linkonce.t.*) 
		*(.glue_7) 
		*(.glue_7t)
		*(.rodata*) 
	} > exe

	.data __ram_start_virt__ : AT (__ram_start_phys__) {
		*(.data*)
		*(.data1)
		*(.gnu.linkonce.d.*)
		. = ALIGN (4);
		__CTOR_LIST__ = ABSOLUTE (.);
		KEEP (*(SORT (.ctors*)))
		__CTOR_END__  = ABSOLUTE (.);
		__DTOR_LIST__ = ABSOLUTE (.);
		KEEP (*(SORT (.dtors*)))
		__DTOR_END__  = ABSOLUTE (.);
		*(.dynamic)
		*(.sdata*)
		*(.gnu.linkonce.s.*)
		. = ALIGN (4);
		*(.2ram.*)
	} > ram 

	.fini : {
		. = .;
		*(.fini)
	} > ram

	.rodata ALIGN (0x4) : { 
		. = .; 
		*(.gnu.linkonce.r.*)
	} > ram

	.rodata1 ALIGN (0x4) : {
		. = .;
		*(.rodata1)
	} > ram

	.fixup ALIGN (0x4) : {
		. = .;
		*(.fixup)
	} > ram

	.gcc_except_table ALIGN (0x4) : {
		. = .;
		*(.gcc_except_table)
	} > ram

	.got ALIGN (0x4) : {
		*(.got.plt)
		*(.got)
	} > ram

	.bss __bss_start_virt__ : {
		*(.scommon)
		*(.dynsbss)
		*(.sbss*)
		*(.gnu.linkonce.sb.*)
		*(.dynbss)
		*(.bss*)
		*(.gnu.linkonce.b.*)
		*(COMMON)
	} > bss

	. = ALIGN(4);
}
