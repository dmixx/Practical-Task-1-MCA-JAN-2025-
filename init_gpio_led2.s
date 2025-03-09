    AREA |.text|, CODE, READONLY
    EXPORT init_gpio_led2_asm

init_gpio_led2_asm
    LDR R0, =0x40023830    ; Load RCC_AHB1ENR address
    LDR R1, [R0]           
    ORR R1, R1, #0x04      ; Enable GPIOC clock
    STR R1, [R0]           

    LDR R0, =0x40020800    ; Load GPIOC_MODER address
    LDR R1, [R0]           
    BIC R1, R1, #(3 << 26) ; Clear PC13 mode bits
    ORR R1, R1, #(1 << 26) ; Set PC13 as output
    STR R1, [R0]           

    BX LR
    END
