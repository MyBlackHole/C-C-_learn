	.file	"main.c"
	.text
	.section	.rodata
.LC0:
	.string	"[%d, %d]"
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$3, -20(%rbp)
	movl	$2, -16(%rbp)
	movl	$4, -12(%rbp)
	movl	$3, -44(%rbp)
	movl	$6, -40(%rbp)
	movl	$8, %edi
	call	malloc@PLT
	movq	%rax, -32(%rbp)
	movl	$0, -56(%rbp)
	movl	$0, -52(%rbp)
	jmp	.L2
.L8:
	movl	-52(%rbp), %eax
	cltq
	movl	-20(%rbp,%rax,4), %edx
	movq	-32(%rbp), %rax
	movl	%edx, (%rax)
	movq	-32(%rbp), %rax
	movl	(%rax), %eax
	movl	-40(%rbp), %edx
	subl	%eax, %edx
	movl	%edx, %eax
	movl	%eax, -36(%rbp)
	movl	-52(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -48(%rbp)
	jmp	.L3
.L5:
	movl	-48(%rbp), %eax
	cltq
	movl	-20(%rbp,%rax,4), %eax
	cmpl	%eax, -36(%rbp)
	jne	.L4
	movl	-48(%rbp), %edx
	movl	-52(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$1, -56(%rbp)
.L4:
	addl	$1, -48(%rbp)
.L3:
	movl	-48(%rbp), %eax
	cmpl	-44(%rbp), %eax
	jl	.L5
	cmpl	$0, -56(%rbp)
	jne	.L11
	addl	$1, -52(%rbp)
.L2:
	movl	-44(%rbp), %eax
	subl	$1, %eax
	cmpl	%eax, -52(%rbp)
	jl	.L8
	jmp	.L7
.L11:
	nop
.L7:
	movl	$0, %eax
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L10
	call	__stack_chk_fail@PLT
.L10:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
