; Definitions.
SCRMEM = $F690
KBMEM = $FFF0
CURLOC = $F6CC

; Entry point.
.ORG $0000
		LDY		#0
		LDX		TEXT
		JSR		PRTTXT
LOOP1:	JSR		GETINP
		STA		TMPAC
		CMP		#$1B	; ESC
		BEQ		END
		CMP		#$7F	; Backspace
		BNE		NOBS
		DEY
		LDA		#$20
NOBS:	JSR		PRTCHR
		LDX		#0
		LDA		TMPAC
		CMP		#$7F
		BNE		SKIP
		LDX		#1
SKIP:	JSR		DEBNC
		CPX		#1
		BEQ		LOOP1
		INY
		JMP		LOOP1
END:	BRK

; Print text with X as length/index.
PRTTXT:	LDA		TEXT,X
		DEX
		STA		SCRMEM,X
		CPX		#$00
		BNE		PRTTXT
		RTS

; Print character stored in AC.
PRTCHR:	STA		CURLOC,Y
		RTS

; Get pressed key to AC.
GETKB:	LDA		KBMEM
		RTS

; Get user input and blink cursor.
GETINP:	LDA		#$5F		; Underscore
BLINK:	STA		CURLOC,Y
		STA		TMPAC
		STY		TMPY
		LDY		#74
		LDX		#250
DELAY:	JSR		GETKB
		CMP		#$FF
		BNE		GETEND
		DEX
		CPX		#0
		BNE		DELAY
		DEY
		CPY		#0
		BNE		DELAY
		LDA		TMPAC
		LDY		TMPY
		CMP		#$5F		; Underscore
		BNE		GETINP
		LDA		#$20		; Space
		JMP		BLINK
GETEND:	LDY		TMPY
		STA		TMPAC
		LDA		#$20
		STA		CURLOC,Y
		LDA		TMPAC
		RTS

; Debounce keyboard
DEBNC:	STA		TMPAC
RETRY:	JSR		GETKB
		CMP		#$FF
		BEQ		BNCD
		CMP		TMPAC
		BEQ		RETRY
BNCD:	RTS

; Data.
TMPAC:	.BYTE	0
TMPY:	.BYTE	0
TEXT:	.BYTE	24
		.BYTE	"Write test (ESC to quit)"
