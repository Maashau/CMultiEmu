.IMPORT		__SCRSTART__
.IMPORT		__KBMEM__

.SEGMENT "ZEROPAGE"

.SEGMENT "CODE"
RESET:
		LDX		#$00
GETKEY:	JSR		GET_A_BLINK
		CMP		#$FF
		BEQ		GETKEY
		STA		__SCRSTART__,X
		INX
		CPX		#$05
		BNE		GETKEY
		BRK

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
		CMP		GET_A_TEMP	;
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
GET_A_BLINK:
		JSR		BLINK
		LDY		#$50
SKIP_BLINK:
		JSR		GET_A_DEBOUNCED
		CMP		#$FF
		BNE		GET_A_BLINK_RET
		DEY
		CPY		#0
		BNE		SKIP_BLINK
		JMP		GET_A_BLINK
GET_A_BLINK_RET:
		RTS

BLINK:	LDA		__SCRSTART__,X
		CMP		#$5F
		BNE		SPACE
		LDA		#$20
		JMP		SWITCH
SPACE:	LDA		#$5F
SWITCH:	STA		__SCRSTART__,X
		RTS

.SEGMENT "VEC_NMI"
	.WORD $0F00     ; NMI
.SEGMENT "VEC_RST"
	.WORD RESET     ; RESET
.SEGMENT "VEC_IRQBRK"
	.WORD $0000     ; BRK/IRQ
