    AREA |.text|, CODE, READONLY 
    EXPORT gpio_led2_on_odr_asm

gpio_led2_on_odr_asm
    LDR R0, =0x40020414    ; Load GPIOB_ODR address
    LDR R1, [R0]           ; Read current state
    ORR R1, R1, #0x2000    ; Set PB13 HIGH (1 << 13)
    STR R1, [R0]           ; Write back to ODR
    BX LR
    END
