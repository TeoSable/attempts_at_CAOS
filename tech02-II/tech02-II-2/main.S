.global main

.text
main:
	// Save stack frame and return adress
	stp x29, x30, [sp, -16]!
	mov x29, sp
	// Allocate local variables
	sub sp, sp, 16

loop:
	// Read char
	bl getchar_unlocked
	// Check if char is EOF and exit loop if so
	cmp x0, 0
	blt exit
	// Check if char is in range('0', '9'), skip to next loop iteration if not
	cmp x0, '0'
	blt loop
	cmp x0, '9'
	bgt loop
	// Output char
	bl putchar_unlocked
	b loop
exit:
	// Free local variables
	mov sp, x29
	// Restore fp, sp and free memory
	ldp x29, x30, [sp], 16
	// Exit with code 0
	mov x0, 0
	ret
