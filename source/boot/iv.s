
.globl _iv
_iv:
	ldr pc, _reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort //12
	ldr	pc, _data_abort  //16
	ldr	pc, _not_used  //20
	ldr	pc, _irq  //24
	ldr	pc, _fiq  //28
	_zero:
		.word 0
	_reset:
		.word reset
	_undefined_instruction:
		.word undefined_instruction
	_software_interrupt:
		.word software_interrupt
	_prefetch_abort:
		.word prefetch_abort
	_data_abort:
		.word data_abort
	_not_used:
		.word not_used
	_irq:
		.word irq
	_fiq:
		.word fiq
	.balignl 16,0xdeadbeef
