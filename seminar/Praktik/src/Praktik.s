	.data
b: .long 5
	.bss
a: .space 12
	.text
	.globl _main
_main:
	xorl %eax, %eax
	xorl %esi, %esi
	movl $a, %esi
	movl $1, (%esi)
	movl b, %eax
	addl (%esi), %eax
	movl %eax, 4(%esi)
	movl $3, %eax
	movl b, %ebx
	mull %ebx
	movl %eax, 8(%esi) 
	leave
	ret
