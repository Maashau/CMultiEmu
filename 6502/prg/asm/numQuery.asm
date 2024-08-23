.IMPORT		__SCRSTART__
.IMPORT		__KBMEM__

.SEGMENT "STARTUP"
		JMP		MAIN

.SEGMENT "ZEROPAGE"
TMP2:	.BYTE	0
TMP3:	.BYTE	0

.SEGMENT "CODE"
MAIN:
		LDA		ADDR2
		LDY		#1
		STA		STR_ADDR,Y
		LDA		ADDR2,Y
		INY
		STA		STR_ADDR,Y
		LDX		#0
		LDY		#0
		JSR		PRINT_STR
		
		LDY		#2
		INX
		JSR		GET_NUM
		STA		TMP2
		LDY		#3
		JSR		GET_NUM
		STA		TMP3
		BRK


; Prints string from address set to STR_ADDR to screen memory indexed by X
; Removes string address from stack
; Increments X
PRINT_STR:
STR_ADDR:
		LDA		$FFFF,Y
		JSR		PRINT_CHR
		CMP		#0
		BEQ		PRINT_STR_RET
		INY
		JMP		PRINT_STR
PRINT_STR_RET:
		RTS

; Prints A to X indexed screen memory
; Increments X
PRINT_CHR:
		STA		__SCRSTART__,X
		INX
		RTS

; GET_NUM
;
; Gets number with char count of Y
;
; parameters
;		Y = 1 ... 3 count of characters.
;
; Returns A = input value 0 ... 255
GET_NUM_RES:	.BYTE 0
GET_NUM:
		LDA		#0
		STA		GET_NUM_RES
GET_NUM_LOOP:
		JSR		GET_NUM_A
		STA		__SCRSTART__,X
		SBC		#48
		INX
		DEY
		CPY		#0
		BEQ		GET_NUM_RET
		PHA
		LDA		GET_NUM_RES
		JSR		MULT10
		STA		GET_NUM_RES
		PLA
		ADC		GET_NUM_RES
		STA		GET_NUM_RES
		JMP		GET_NUM_LOOP
GET_NUM_RET:
		PHA
		LDA		GET_NUM_RES
		JSR		MULT10
		STA		GET_NUM_RES
		PLA
		ADC		GET_NUM_RES
		RTS

MULT10_TEMP:	.BYTE 0
MULT10:	ASL
		STA		MULT10_TEMP
		ASL
		ASL
		CLC
		ADC		MULT10_TEMP
		PHA
		LDA		#0
		STA		MULT10_TEMP
		PLA
		RTS

; GET_NUM_A
;
; Get ASCII number
;
; Parameters
;		X = 0 ... 255 index of writable screen memory
;
; Returns A = ASCII number entered
GET_NUM_A:
		JSR		GET_A_BLINK
		CMP		#48			; ASCII 0
		BCC		GET_NUM_A	; Carry clear, A < 0
		CMP		#57			; ASCII 9
		BEQ		GET_NUM_A	; Zero set, A == 9
		RTS

; GET_A
;
; Get keyboard key press.
; 
; Parameters
;		A = 0 ... 255 tries
;
; Returns A = Key press / 0xFF (no key pressed)
GET_A_TEMP:	.BYTE	0
GET_A:
		STA		GET_A_TEMP
		TXA
		PHA
		LDX		GET_A_TEMP
GET_A_LOOP:
		LDA		__KBMEM__	; Get keyboard loop
		CMP		#$FF		;
		BNE		GET_A_END	;
		DEX					;
		CPX		#0			;
		BNE		GET_A_LOOP	; \Get keyboard loop
GET_A_END:
		STA		GET_A_TEMP
		PLA
		TAX
		LDA		GET_A_TEMP
		RTS

; GET_A_DEBOUNCED
;
; Works identically with GET_A, but debounces the input.
;
; Parameters
;		A = 0 ... 255 tries
;
; Returns A = Key press / 0xFF (no key pressed)
GET_A_DEBOUNCED:
		JSR		GET_A
		STA		GET_A_TEMP
RE_KB_DEBOUNCE:
		LDA		__KBMEM__		; Debounce loop
		CMP		#$FF			;
		BEQ		KB_DEBOUNCED	;
		CMP		GET_A_TEMP		;
		BEQ		RE_KB_DEBOUNCE	; \Debounce loop
KB_DEBOUNCED:
		LDA		GET_A_TEMP
		RTS

; GET_A_BLINK
;
; Works idenctically with GET_A_DEBOUNCED, but blinks cursor until key press is
; detected.
;
; Parameters
;		X = 0 ... 255 index of writable screen memory
;		__SCRSTART__ = 0 index address of the writable area
;
; Returns A = Key press / 0xFF (no key pressed)
GET_A_BLINK_TEMP: .BYTE 0
GET_A_BLINK:
		TYA
		PHA
NEXT_BLINK:
		JSR		BLINK
		LDY		#$50
SKIP_BLINK:
		JSR		GET_A_DEBOUNCED
		CMP		#$FF
		BNE		GET_A_BLINK_RET
		DEY
		CPY		#0
		BNE		SKIP_BLINK
		JMP		NEXT_BLINK
GET_A_BLINK_RET:
		STA		GET_A_BLINK_TEMP
		PLA
		TAY
		LDA		GET_A_BLINK_TEMP
		RTS

BLINK:	LDA		__SCRSTART__,X
		CMP		#$5F
		BNE		SPACE
		LDA		#$20
		JMP		SWITCH
SPACE:	LDA		#$5F
SWITCH:	STA		__SCRSTART__,X
		RTS


.SEGMENT "DATA"
ADDR2:	.ADDR	TXT2
ADDR3:	.ADDR	TXT3
TXT2:	.ASCIIZ	"Enter 2 char number: "
TXT3:	.ASCIIZ	"Enter 3 char number: "