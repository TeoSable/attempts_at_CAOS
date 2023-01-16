.global sum
.text

sum:
	// x0 - x0, x1 - N, x2 - &X
	mov x15, x1 // x15 will be the iterator (i)
loop:
	sub x15, x15, #1 // --i
	lsl x3, x15, 2 // x3 = i * 4
	ldr x1, [x2, x3] // x1 = X[i] <=> x1 = *(X + i*4)
	add x0, x0, x1 // x0 += x1 <=> x0 += X
	cmp x15, #0 // Compare i and 0
	beq exit // If i = 0, end loop
	b loop // Continue loop
exit:
	ret // Return x0
