.global main
.text

main:
  stp x29, x30, [sp, -16]!
  mov x29, sp
  mov x20, 0 // buffer = nullptr
  mov x21, 0 // size of array = 0
  mov x22, 1 // cap of array = 1
  mov x0, 0
  mov x1, x22
  bl realloc
  mov x20, x0

  input:
    bl getchar_unlocked

    // If I get EOF
    cmp x0, 0
    blt output
    
    // put new symbol in array
    str x0, [x20, x21]
    add x21, x21, 1

    // if size < cap then
    cmp x21, x22
    blt input
    
    // else 
    lsl x22, x22, 1
    mov x0, x20
    mov x1, x22
    bl realloc
    mov x20, x0
    b input
    
    
  output:
    cmp x21, 0
    beq exit
    sub x21, x21, 1
    ldr x0, [x20, x21]
    bl putchar_unlocked
    b output

  exit:
    // Free all that needed
    mov x0, x20
    bl free
    mov sp, x29
    ldp x29, x30, [sp], 16
    mov x0, 0
    ret
    
