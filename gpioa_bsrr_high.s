   AREA |.text|, CODE, READONLY  ;  Define code section
   EXPORT gpioa_bsrr_high_asm

gpioa_bsrr_high_asm
    LDR R0, =0x40020018    ; Load GPIOA_BSRR address
    MOV R1, #0x20          ; Set PA5 high (1 << 5)
    STR R1, [R0]           ; Write to BSRR
    BX LR
