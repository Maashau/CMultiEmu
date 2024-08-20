; Input: Y = year (0=1900, 1=1901, ..., 255=2155)
;        X = month (1=Jan, 2=Feb, ..., 12=Dec)
;        A = day (1 to 31)
;
; Output: Weekday in A (0=Sunday, 1=Monday, ..., 6=Saturday)

CUR_LOC = $F690
KB_MEM = $FFF0

; Entry point
.ORG $0000
		LDX		#0
		LDA		(TXT2,X)
		PHA
		INX
		LDA		(TXT2,X)
		PHA
		JSR		PRT_TXT
		LDY		#2
		JSR		GET_NUM
		STA		TMP2
		LDY		#3
		JSR		GET_NUM
		STA		TMP3
		BRK

; Print text with X as length/index.
TXTSRC:	.ADDR	0
PRT_TXT:
		PLA
		STA		(TXTSRC,X)
		DEX
		PLA
		STA		(TXTSRC,X)
		LDY		#0
		LDA		(TXTSRC),Y
TXT_LOOP:
		STA		CUR_LOC,Y
		LDA		(TXTSRC),Y
		CMP		#$00
		BNE		TXT_LOOP
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
		JSR		GET_A_BLINK
		STA		CUR_LOC,X
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
		LDA		KB_MEM			; Get keyboard loop
		CMP		#$FF			;
		BNE		GET_A_END	;
		DEX						;
		CPX		#0				;
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
		LDA		KB_MEM			; Debounce loop
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
;		CUR_LOC = 0 index address of the writable area
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

BLINK:	LDA		CUR_LOC,X
		CMP		#$5F
		BNE		SPACE
		LDA		#$20
		JMP		SWITCH
SPACE:	LDA		#$5F
SWITCH:	STA		CUR_LOC,X
		RTS


; Data section.
ADDR2:	.ADDR	(TXT2)
ADDR3:	.ADDR	(TXT3)
TXT2:	.ASCIIZ	"Enter 2 char number: "
TXT3:	.ASCIIZ	"Enter 3 char number: "
TMP2:	.BYTE	0
TMP3:	.BYTE	0