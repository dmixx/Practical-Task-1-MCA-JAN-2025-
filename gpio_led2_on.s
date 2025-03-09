    AREA |.text|, CODE, READONLY
    EXPORT gpio_led2_on_asm

gpio_led2_on_asm
    LDR R0, =0x40020818    ; Load GPIOB_BSRR address
    MOV R1, #0x2000        ; Set PC13 HIGH (1 << 13)
    STR R1, [R0]           ; Write to BSRR
    BX LR
    END
