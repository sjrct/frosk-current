
target remote | exec qemu-system-x86_64 -gdb stdio -drive file=frosk.img,format=raw
add-symbol-file kernel/kernel.dbg
