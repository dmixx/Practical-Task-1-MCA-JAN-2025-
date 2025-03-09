    AREA    |.text|, CODE, READONLY
    EXPORT  LED1_Init_Asm
    EXPORT  LED1_On_Asm

LED1_Init_Asm
    ; Enable GPIOA Clock (RCC->AHB1ENR |= (1U << 0))
    LDR     R0, =0x40023830   ; Address of RCC->AHB1ENR
    LDR     R1, [R0]          ; Read current value
    ORR     R1, R1, #0x1      ; Set bit 0 to enable GPIOA
    STR     R1, [R0]          ; Store back to RCC->AHB1ENR

    ; Configure PA5 as General Output Mode (GPIOA->MODER |= (1U << (5*2)))
    LDR     R0, =0x40020000   ; Address of GPIOA
    LDR     R1, [R0]          ; Read current MODER value
    BIC     R1, R1, #(3 << 10) ; Clear bits for PA5
    ORR     R1, R1, #(1 << 10) ; Set PA5 as output
    STR     R1, [R0]          ; Store back to GPIOA->MODER
    BX      LR                ; Return

LED1_On_Asm
    LDR     R0, =0x40020018   ; Address of GPIOA->BSRR
    MOV     R1, #(1 << 5)     ; Set PA5 HIGH
    STR     R1, [R0]          ; Store value in BSRR
    BX      LR                ; Return
