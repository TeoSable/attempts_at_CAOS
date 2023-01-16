.global calculate
.global R

.text
calculate:
	// Load variables A, B, C and D to registers x0, x1, x2 and x3 correspondently
	ldr x0, A
	ldr x1, B
	ldr x2, C
	ldr x3, D
	// x0 = x0 * x1 + x2 * x3
	mul x2, x2, x3
	madd x0, x0, x1, x2
	// Store result to R
	adr x1, R
	str x0, [x1]
	// Return
	ret

.data
// Define 8-bit global variable R
R: .zero 8
