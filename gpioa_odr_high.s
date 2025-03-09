GPIOA_ODR		EQU	0x40020014
LED_ILLUM 		EQU 	0x00000020
	
				AREA asm_area, CODE, READONLY
					
; Export gpioa_odr_high_asm function location so that C compiler can find it and link
				EXPORT  gpioa_odr_high_asm

gpioa_odr_high_asm 						;assembly entry point for C function, do not delete


				LDR 	R0,=GPIOA_ODR 			;Load address of GPIOA_BSRR to R0
				LDR 	R1,=LED_ILLUM 			;Load value(BS5 equals logic 1) to R1
				STR 	R1,[R0] 				;Put value into GPIOA_ODR
				
; Return to C using link register 				
				BX 		LR
				
				END		; End of the program/function