	.global  _start
	.section .text.kentry

_start:
	la       sp, _stack_top # Setup stack
	# We're booting on top of OpenSBI, therefore:
	# a0: hartid
	# a1: pointer to device tree blob (dtb)
	call     kmain;

loop:
	j        loop
