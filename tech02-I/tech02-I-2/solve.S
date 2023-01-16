.global solve
.text

solve:
	// x0 = A, x1 = B, x2 = C, x3 = D
	mov x6, #0 // x = 0
loop:
	madd w4, w0, w6, w1 // w4 = A*x + B
	madd w4, w4, w6, w2 // w4 = (A*x + B)*x + C
	madd w4, w4, w6, w3 // w4 = ((A*x + B)*x + C)*x + D
	cmp w4, #0 // Compare w4 with 0
	beq exitte // If w4 == 0, end loop

	add x6, x6, #1 // Else - x++
	cmp x6, #255
	bge exitte
	b loop // Continue loop
exitte:
	mov w0, w6// Move x to return register
	ret
