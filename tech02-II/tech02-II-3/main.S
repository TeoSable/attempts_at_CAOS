.global main

.text
main:
	// Save stack frame and return address
	stp x29, x30, [sp, -16]!
	mov x29, sp
	// Allocate memory for char array using realloc
	mov x0, 0
	mov x1, 1
	bl realloc
	// Store array pointer (p) in x20
	mov x20, x0
	// Store array capacity in x6 and array size in register x5. Default capacity is 8
	mov x22, 1
	mov x21, 0

	read_char:
		// Read char
		bl getchar_unlocked
		// Check for EOF, proceed to print sequence if found
		cmp x0, 0
		blt print_char
		// Store new char in array (save it to p[size])
		str x0, [x20, x21]
		// Increment size and check if it is less then capacity. Proceed to read next char if so
		add x21, x21, 1
		cmp x21, x22 
		blt read_char
		// Otherwise, double array capacity
		lsl x22, x22, 1
		// Reallocate memory under p
		mov x0, x20
		mov x1, x22
		bl realloc
		mov x20, x0
		b read_char

	print_char:
		// Decrement size and check if it is less than 0. Proceed to end program if so
		sub x21, x21, 1
		cmp x21, 0
		blt finale
		// Load char p[size]
		ldr x0, [x20, x21]
		// Print char
		bl putchar_unlocked
		// Proceed to print next char
		b print_char
	
	finale:	
		// Free memory under pointer p
		mov x0, x20
		bl free
		// Free local variables
		mov sp, x29
		// Restore fp, sp and free memory
		ldp x29, x30, [sp], 16
		// Exit with return code 0
		mov x0, 0
	ret
