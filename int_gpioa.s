RCC_AHB1ENR		EQU 	0x40023830 				;Reset and Clock Control for AHB1

;Addresses for GPIOA Configuration Registers
GPIOA_MODER		EQU		0x40020000
GPIOA_OTYPER	EQU		0x40020004
GPIOA_OSPEEDR	EQU		0x40020008
GPIOA_PUPDR		EQU		0x4002000C	

;Address for GPIOA Input Data Register
GPIOA_IDR		EQU		0x40020010
	
;Address for GPIOA Output Data Register	
GPIOA_ODR		EQU		0x40020014

				AREA asm_area, CODE, READONLY
					
; Export init_copc_asm function location so that C compiler can find it and link
				EXPORT  int_gpioa_asm

int_gpioa_asm 										;assembly entry point for C function, do not delete


				; Turns on clock for GPIOA
				LDR 	R0,=RCC_AHB1ENR			;Load address of RCC_AHB1 to R0
				LDR 	R1,=0x00000001		    ;Put mask bits into R1
				STR 	R1,[R0] 				;Put new value back into RCC_AHB1ENR
				
												; Setup PORTA Pin 5 to be output
				LDR 	R0,=GPIOA_MODER 		;Load GPIOA_MODER address to R0
				LDR 	R1,=0x00000400 			;Load new value to R1
				STR 	R1,[R0] 				;Put value into GPIOA_MODER									
				
												; Setup PORTA Pin 5 type to be push-pull
				LDR 	R0,=GPIOA_OTYPER 		;Load GPIOA_OTYPER address to R0
				LDR 	R1,=0x00000000 			;Load new value to R1
				STR 	R1,[R0] 				;Put value into GPIOA_OTYPER
				
												; Setup PORTA Pin 5 output speed to low				
				LDR 	R0,=GPIOA_OSPEEDR 		;Load GPIOA_OSPEEDR address to R0
				LDR 	R1,=0x00000000 			;Load new value to R1
				STR 	R1,[R0] 				;Put value into GPIOA_MODER	
				
; Return to C using link register 
				
				BX	LR
				
				END			; End of the program/function