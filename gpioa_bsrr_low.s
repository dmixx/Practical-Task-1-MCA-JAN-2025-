 AREA |.text|, CODE, READONLY  ;  Define code section
 EXPORT gpioa_bsrr_low_asm

gpioa_bsrr_low_asm
    LDR R0, =0x40020018    ; Load GPIOA_BSRR address
    MOV R1, #0x00200000    ; Reset PA5 (1 << (5 + 16))
    STR R1, [R0]           ; Write to BSRR
    BX LR

