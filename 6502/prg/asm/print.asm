.IMPORT		__SCRSTART__

.SEGMENT "ZEROPAGE"

.SEGMENT "CODE"
RESET:
		LDY		#0
		LDA		TXT1ADDR
		PHA
		INY
		LDA		TXT1ADDR,Y
		PHA
		LDX		#0
		JSR		PRINT_STR
		LDY		#0
		LDA		TXT2ADDR
		PHA
		INY
		LDA		TXT2ADDR,Y
		PHA
		LDX		#60
		JSR		PRINT_STR
		BRK


; Prints string from address pushed to stack to screen memory indexed by X
; Removes string address from stack
; Increments X
TMP_JSR_ADDR: .ADDR 0
PRINT_STR:
		LDY		#1					; Pop JSR from stack
		PLA							;
		STA		TMP_JSR_ADDR,Y		; 
		PLA							;
		STA		TMP_JSR_ADDR		; 
		LDY		#2					; Pop TXT address from stack
		PLA							;
		STA		LOADTXT,Y			;
		DEY							;
		PLA							;
		STA		LOADTXT,Y			;
		LDY		#1					; Push JSR back to stack
		LDA		TMP_JSR_ADDR		;
		PHA							;
		LDA		TMP_JSR_ADDR,Y		;
		PHA							;
		DEY
PRINT_STR_LOOP:
LOADTXT:LDA		$FFFF,Y
		JSR		PRINT_CHR
		CMP		#0
		BEQ		PRINT_STR_RET
		INY
		JMP		PRINT_STR_LOOP
PRINT_STR_RET:
		RTS

; Prints A to X indexed screen memory
; Increments X
PRINT_CHR:
		STA	__SCRSTART__,X
		INX
		RTS

.SEGMENT "DATA"
TXT1ADDR: .ADDR   TXT1
TXT2ADDR: .ADDR   TXT2
TXT1: .ASCIIZ "Test text 1"
TXT2: .ASCIIZ "Test text 2"

.SEGMENT "VEC_NMI"
	.WORD $0F00     ; NMI
.SEGMENT "VEC_RST"
	.WORD RESET     ; RESET
.SEGMENT "VEC_IRQBRK"
	.WORD $0000     ; BRK/IRQ