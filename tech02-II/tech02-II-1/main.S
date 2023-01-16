.global main

.text
main:
	// Save stack frame and return address
  	stp x29, x30, [sp, -16]! // sp <- sp - 16
	mov x29, sp
	// Allocate local variables
	sub sp, sp, 16

  	// Call scanf for values
  	adr x0, INPUT_STR
  	sub x1, x29, 4
	sub x2, x29, 8
  	bl scanf

	// Add values together
	ldr w1, [x29, -4]
	ldr w2, [x29, -8]
	add w1, w1, w2

	// Call printf. Output value is already in w1
	adr x0, OUTPUT_STR
  	bl printf

  	// Free local variables
  	mov sp, x29
  	// Restore fp, sp and free the memory
  	ldp x29, x30, [sp], 16
  	// Exit with return code 0
  	mov w0, 0
	ret

.data
INPUT_STR: .string "%d %d"
OUTPUT_STR: .string "%d"
