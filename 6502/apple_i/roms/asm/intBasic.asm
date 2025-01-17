

IN_BUF	= $0200		;  Input buffer to RAM_027F__
KBD		= $D010		;  PIA.A keyboard input
KBDCR	= $D011		;  PIA.A keyboard control register
DSP		= $D012		;  PIA.B display output register

; RAM

RAM_0128__	:= $0128
RAM_0120__	:= $0120
RAM_0157__	:= $0157
RAM_014F__	:= $014F
RAM_0147__	:= $0147
RAM_013F__	:= $013F
RAM_015F__	:= $015F
RAM_0167__	:= $0167
RAM_0137__	:= $0137
RAM_012F__	:= $012F
RAM_0127__	:= $0127
RAM_011F__	:= $011F
RAM_0107__	:= $0107
RAM_0117__	:= $0117
RAM_010F__	:= $010F
RAM_0118__	:= $0118
RAM_0110__	:= $0110
RAM_0108__	:= $0108
RAM_0100__	:= $0100
RAM_0DDC__	:= $0DDC
RAM_6000__	:= $6000
RAM_AAAA__	:= $AAAA
RAM_C5A0__	:= $C5A0
RAM_CFD4__	:= $CFD4

.SEGMENT "ZEROPAGE"
ZP00		= $00

ZP03		= $03

ZP06		= $06
ZP07		= $07
ZP08		= $08

ZP0A		= $0A

ZP1D		= $1D

ZP23		= $23
IN_COUNT	= $24

ZP30		= $30
ZP35		= $35
ZP37		= $37

ZP40		= $40

ZP48		= $48

ZP4A		= $4A
ZP4B		= $4B
ZP4C		= $4C
ZP4D		= $4D
ZP4E		= $4E
ZP4F		= $4F

ZP50		= $50
ZP51		= $51

ZP53		= $53

ZP55		= $55
ZP56		= $56

ZP58		= $58

ZP5B		= $5B

ZP60		= $60

ZP75		= $75
ZP76		= $76
ZP77		= $77
ZP78		= $78
ZP79		= $79

ZP7B		= $7B

ZP7F		= $7F

ZP80		= $80

ZP85		= $85

ZP8B		= $8B

ZP95		= $95

ZP9D		= $9D

ZP9F		= $9F

ZPA0		= $A0

ZPA3		= $A3

ZPA5		= $A5

ZPA8		= $A8

ZPAF		= $AF

ZPB3		= $B3
ZPB4		= $B4
ZPB5		= $B5
ZPB6		= $B6

ZPB8		= $B8

ZPBC		= $BC

ZPC2		= $C2
ZPC3		= $C3
ZPC4		= $C4
ZPC6		= $C6
ZPC8		= $C8
ZPC9		= $C9
ZPCA		= $CA
ZPCB		= $CB
ZPCC		= $CC
ZPCD		= $CD
ZPCE		= $CE
ZPCF		= $CF
ZPD0		= $D0
ZPD1		= $D1
ZPD2		= $D2
ZPD3		= $D3
ZPD4		= $D4
ZPD5		= $D5
ZPD6		= $D6
ZPD7		= $D7
ZPD8		= $D8
ZPD9		= $D9
ZPDA		= $DA
ZPDB		= $DB
ZPDC		= $DC
ZPDD		= $DD

ZPE0		= $E0
ZPE1		= $E1
ZPE2		= $E2
ZPE3		= $E3
LAST_CHROUT__		= $E4
ZPE5		= $E5
ZPE6		= $E6
ZPE7		= $E7
ZPE8		= $E8

ZPF0		= $F0
ZPF1		= $F1
ZPF2		= $F2
ZPF3		= $F3
ZPF4		= $F4
ZPF5		= $F5
ZPF6		= $F6
ZPF7		= $F7
ZPF8		= $F8
ZPF9		= $F9
ZPFA		= $FA
ZPFB		= $FB
ZPFC		= $FC
ZPFD		= $FD
ZPFE		= $FE
ZPFF		= $FF


.SEGMENT "CODE"
				JMP		ENTRY_POINT		; Entry point

; Gets next keyboard input -----------------------------------------------------
GET_KBD:		LDA		KBDCR
				BPL		GET_KBD
				LDA		KBD
				RTS
;-------------------------------------------------------------------------------

;			--------------------------------------------------------------------
J_00C__:		TXA
				AND		#$20
				BEQ		B_034__
PRINT_SPC:		LDA		#' '+$80	; Space
				STA		LAST_CHROUT__
				JMP		OUT_CHAR
;-------------------------------------------------------------------------------

;			--------------------------------------------------------------------
J_018__:		LDA		#$20
J_01A__:		CMP		IN_COUNT
				BCS		JB_02A__
				LDA		#$8D
				LDY		#$07
B_022__:		JSR		OUT_CHAR
				LDA		#$A0
				DEY
				BNE		B_022__
JB_02A__:		LDY		#$00
				LDA		(ZPE2),Y
				INC		ZPE2
				BNE		B_034__
				INC		ZPE3
B_034__:		RTS
;-------------------------------------------------------------------------------

; DATA ? -----------------------------------------------------------------------
				JSR		J_715__
				JSR		JB_576__
	
JB_03B__:		LDA		ZPE2
				CMP		ZPE6
				LDA		ZPE3
				SBC		ZPE7
				BCS		B_034__
				JSR		J_06D__
				JMP		JB_03B__

				LDA		ZPCA
				STA		ZPE2
				LDA		ZPCB
				STA		ZPE3
				LDA		ZP4C
				STA		ZPE6
				LDA		ZP4D
				STA		ZPE7
				BNE		JB_03B__
				JSR		J_715__
				JSR		J_56D__
				LDA		LAST_CHROUT__
				STA		ZPE2
				LDA		ZPE5
				STA		ZPE3
				BCS		B_034__
J_06D__:		STX		ZPD8
				LDA		#$A0
				STA		ZPFA
				JSR		JB_02A__
				TYA
;-------------------------------------------------------------------------------
B_077__:		STA		LAST_CHROUT__
				JSR		JB_02A__
				TAX
				JSR		JB_02A__
				JSR		J_51B__
B_083__:		JSR		J_018__
				STY		ZPFA
				TAX
				BPL		B_0A3__
				ASL		A
				BPL		B_077__
				LDA		LAST_CHROUT__
				BNE		B_095__
				JSR		PRINT_SPC
B_095__:		TXA
B_096__:		JSR		OUT_CHAR
B_099__:		LDA		#$25
				JSR		J_01A__
				TAX
				BMI		B_096__
				STA		LAST_CHROUT__
B_0A3__:		CMP		#$01
				BNE		B_0AC__
				LDX		ZPD8
				JMP		OUT_NL

B_0AC__:		PHA
				STY		ZPCE
				LDX		#$ED
				STX		ZPCF
				CMP		#$51
				BCC		B_0BB__
				DEC		ZPCF
				SBC		#$50
B_0BB__:		PHA
				LDA		(ZPCE),Y
B_0BE__:		TAX
				DEY
				LDA		(ZPCE),Y
				BPL		B_0BE__
				CPX		#$C0
				BCS		B_0CC__
				CPX		#$00
				BMI		B_0BE__
B_0CC__:		TAX
				PLA
				SBC		#$01
				BNE		B_0BB__
				BIT		LAST_CHROUT__
				BMI		B_0D9__
				JSR		J_FF8__
B_0D9__:		LDA		(ZPCE),Y
				BPL		B_0ED__
				TAX
				AND		#$3F
				STA		LAST_CHROUT__
				CLC
				ADC		#$A0
				JSR		OUT_CHAR
				DEY
				CPX		#$C0
				BCC		B_0D9__
B_0ED__:		JSR		J_00C__
				PLA
				CMP		#$5D
				BEQ		B_099__
				CMP		#$28
				BNE		B_083__
				BEQ		B_099__
				JSR		J_118__
				STA		ZP50,X
				CMP		ZP78,X
J_102__:		BCC		B_115__
B_104__:		LDY		#$2B		; String offset of "STRING"
B_106__:		JMP		JB_3E0__

				JSR		J_E34__
				CMP		ZP50,X
				BCC		B_104__
				JSR		J_FE4__
				STA		ZP78,X
B_115__:		JMP		J_823__

J_118__:		JSR		J_E34__
				BEQ		B_104__
				SEC
				SBC		#$01
				RTS

				JSR		J_118__
				STA		ZP50,X
				CLC
				SBC		ZP78,X
				JMP		J_102__

B_12C__:		LDY		#$14
				BNE		B_106__
				JSR		J_118__
				INX
J_134__:		LDA		ZP50,X
				STA		ZPDA
				ADC		ZPCE
				PHA
				TAY
				LDA		ZP78,X
				STA		ZPDB
				ADC		ZPCF
				PHA
				CPY		ZPCA
				SBC		ZPCB
				BCS		B_12C__
				LDA		ZPDA
				ADC		#$FE
				STA		ZPDA
				LDA		#$FF
				TAY
				ADC		ZPDB
				STA		ZPDB
B_156__:		INY
				LDA		(ZPDA),Y
				CMP		ZPCC,Y
				BNE		B_16D__
				TYA
				BEQ		B_156__
B_161__:		PLA
				STA		(ZPDA),Y
				STA		ZPCC,Y
				DEY
				BPL		B_161__
				INX
				RTS

				NOP
B_16D__:		LDY		#$80
B_16F__:		BNE		B_106__
J_171__:		LDA		#$00
				JSR		J_70A__
				LDY		#$02
				STY		ZP78,X
				JSR		J_70A__
				LDA		#$BF
				JSR		OUT_CHAR
				LDY		#$00
				JSR		JB_29E__
				STY		ZP78,X
				NOP
				NOP
				NOP
				LDA		ZP51,X
				STA		ZPCE
				LDA		ZP79,X
				STA		ZPCF
				INX
				INX
				JSR		J_1BC__
B_199__:		LDA		ZP4E,X
				CMP		ZP76,X
				BCS		B_1B4__
				INC		ZP4E,X
				TAY
				LDA		(ZPCE),Y
				LDY		ZP50,X
				CPY		LAST_CHROUT__
				BCC		B_1AE__
				LDY		#$83
				BNE		B_16F__
B_1AE__:		STA		(ZPDA),Y
				INC		ZP50,X
				BCC		B_199__
B_1B4__:		LDY		ZP50,X
				TXA
				STA		(ZPDA),Y
				INX
				INX
				RTS

J_1BC__:		LDA		ZP51,X
				STA		ZPDA
				SEC
				SBC		#$02
				STA		LAST_CHROUT__
				LDA		ZP79,X
				STA		ZPDB
				SBC		#$00
				STA		ZPE5
				LDY		#$00
				LDA		(LAST_CHROUT__),Y
				CLC
				SBC		ZPDA
				STA		LAST_CHROUT__
				RTS

J_1D7__:		LDA		ZP53,X
				STA		ZPCE
				LDA		ZP7B,X
				STA		ZPCF
				LDA		ZP51,X
				STA		ZPDA
				LDA		ZP79,X
				STA		ZPDB
				INX
				INX
				INX
				LDY		#$00
				STY		ZP78,X
				STY		ZPA0,X
				INY
				STY		ZP50,X
B_1F3__:		LDA		ZP4D,X
				CMP		ZP75,X
				PHP
				PHA
				LDA		ZP4F,X
				CMP		ZP77,X
				BCC		B_206__
				PLA
				PLP
				BCS		B_205__
B_203__:		LSR		ZP50,X
B_205__:		RTS

B_206__:		TAY
				LDA		(ZPCE),Y
				STA		LAST_CHROUT__
				PLA
				TAY
				PLP
				BCS		B_203__
				LDA		(ZPDA),Y
				CMP		LAST_CHROUT__
				BNE		B_203__
				INC		ZP4F,X
				INC		ZP4D,X
				BCS		B_1F3__
				JSR		J_1D7__
				JMP		J_736__

				JSR		J_254__
B_225__:		ASL		ZPCE
				ROL		ZPCF
				BCC		B_238__
				CLC
				LDA		ZPE6
				ADC		ZPDA
				STA		ZPE6
				LDA		ZPE7
				ADC		ZPDB
				STA		ZPE7
B_238__:		DEY
				BEQ		JB_244__
				ASL		ZPE6
				ROL		ZPE7
				BPL		B_225__
				JMP		JB_77E__

JB_244__:		LDA		ZPE6
				JSR		J_708__
				LDA		ZPE7
				STA		ZPA0,X
				ASL		ZPE5
				BCC		B_279__
				JMP		J_76F__

J_254__:		LDA		#$55
				STA		ZPE5
				JSR		J_25B__

J_25B__:		LDA		ZPCE
				STA		ZPDA
				LDA		ZPCF
				STA		ZPDB
				JSR		J_715__
				STY		ZPE6
				STY		ZPE7
				LDA		ZPCF
				BPL		B_277__
				DEX
				ASL		ZPE5
				JSR		J_76F__
				JSR		J_715__
B_277__:		LDY		#$10
B_279__:		RTS

J_27A__:		JSR		J_E6C__
				BEQ		JB_244__
				.BYTE	$FF
B_280__:		CMP		#$84
				BNE		B_286__
				LSR		ZPF8
B_286__:		CMP		#$DF
				BEQ		B_29B__
				CMP		#$9B
				BEQ		B_294__
				STA		IN_BUF,Y
				INY
				BPL		JB_29E__
B_294__:		LDY		#$8B		; String offset of "\\r"
				JSR		OUT_STR
J_299__:		LDY		#$01
B_29B__:		DEY
				BMI		B_294__
JB_29E__:		JSR		GET_KBD
				NOP
				NOP
B_2A3__:		JSR		OUT_CHAR
				CMP		#$8D		; Check if \r
				BNE		B_280__
				LDA		#$DF
				STA		IN_BUF,Y
				RTS

ENTRY_POINT:	JSR		J_FD3__		; Entry point
RET_CLI:		JSR		OUT_NL
NEXT_CMD:		LSR		ZPD9
				LDA		#'>'+$80
				JSR		OUT_CHAR
				LDY		#$00
				STY		ZPFA
				BIT		ZPF8
				BPL		B_2D1__
				LDX		ZPF6
				LDA		ZPF7
				JSR		J_51B__
				LDA		#$A0
				JSR		OUT_CHAR
B_2D1__:		LDX		#$FF
				TXS
				JSR		JB_29E__
				STY		ZPF1
				TXA
				STA		ZPC8
				LDX		#$20
				JSR		J_491__
				LDA		ZPC8
				ADC		#$00
				STA		ZPE0
				LDA		#$00
				TAX
				ADC		#$02
				STA		ZPE1
				LDA		(ZPE0,X)
				AND		#$F0
				CMP		#$B0
				BEQ		B_2F9__
				JMP		J_883__

B_2F9__:		LDY		#$02
B_2FB__:		LDA		(ZPE0),Y
				STA		ZPCD,Y
				DEY
				BNE		B_2FB__
				JSR		J_38A__
				LDA		ZPF1
				SBC		ZPC8
				CMP		#$04
				BEQ		NEXT_CMD
				STA		(ZPE0),Y
				LDA		ZPCA
				SBC		(ZPE0),Y
				STA		LAST_CHROUT__
				LDA		ZPCB
				SBC		#$00
				STA		ZPE5
				LDA		LAST_CHROUT__
				CMP		ZPCC
				LDA		ZPE5
				SBC		ZPCD
				BCC		ERR_MEMFL
B_326__:		LDA		ZPCA
				SBC		(ZPE0),Y
				STA		ZPE6
				LDA		ZPCB
				SBC		#$00
				STA		ZPE7
				LDA		(ZPCA),Y
				STA		(ZPE6),Y
				INC		ZPCA
				BNE		B_33C__
				INC		ZPCB
B_33C__:		LDA		ZPE2
				CMP		ZPCA
				LDA		ZPE3
				SBC		ZPCB
				BCS		B_326__
B_346__:		LDA		LAST_CHROUT__,X
				STA		ZPCA,X
				DEX
				BPL		B_346__
				LDA		(ZPE0),Y
				TAY
B_350__:		DEY
				LDA		(ZPE0),Y
				STA		(ZPE6),Y
				TYA
				BNE		B_350__
				BIT		ZPF8
				BPL		B_365__
B_35C__:		LDA		ZPF7,X
				ADC		ZPF5,X
				STA		ZPF7,X
				INX
				BEQ		B_35C__
B_365__:		BPL		B_3E5__
				BRK
				BRK
				BRK
				BRK
ERR_MEMFL:		LDY		#$14		; String offset of "MEM FULL"
				BNE		JB_3E0__
				JSR		J_715__
				LDA		ZPE2
				STA		ZPE6
				LDA		ZPE3
				STA		ZPE7
				JSR		J_575__
				LDA		ZPE2
				STA		LAST_CHROUT__
				LDA		ZPE3
				STA		ZPE5
				BNE		B_395__
				JSR		J_715__
J_38A__:		JSR		J_56D__
				LDA		ZPE6
				STA		ZPE2
				LDA		ZPE7
				STA		ZPE3
B_395__:		LDY		#$00
B_397__:		LDA		ZPCA
				CMP		LAST_CHROUT__
				LDA		ZPCB
				SBC		ZPE5
				BCS		B_3B7__
				LDA		LAST_CHROUT__
				BNE		B_3A7__
				DEC		ZPE5
B_3A7__:		DEC		LAST_CHROUT__
				LDA		ZPE6
				BNE		B_3AF__
				DEC		ZPE7
B_3AF__:		DEC		ZPE6
				LDA		(LAST_CHROUT__),Y
				STA		(ZPE6),Y
				BCC		B_397__
B_3B7__:		LDA		ZPE6
				STA		ZPCA
				LDA		ZPE7
				STA		ZPCB
				RTS

; Terminal out routines --------------------------------------------------------
PRINT_LOOP:		JSR		OUT_CHAR	; Loop for printing strings
				INY
OUT_STR:		LDA		DATA_STR,Y	; Print a string indexed by Y
				BMI		PRINT_LOOP	; Last char of the string bit $80 is off
OUT_CHAR:		CMP		#$8D		; Check if CR
				BNE		OUT_ACC
OUT_NL:			LDA		#$00		; Print a new line
				STA		IN_COUNT	; Reset character count
				LDA		#$8D		; CR + $80
OUT_ACC:		INC		IN_COUNT	; Print character stored in accumulator
ECHO:			BIT		DSP			; Screen ready ?
				BMI		ECHO
				STA		DSP			; Send character to the screen
				RTS
;-------------------------------------------------------------------------------

ERR_2LONG:		LDY		#$06		; String offset of "TOO LONG"
JB_3E0__:		JSR		PRINT_ERR
				BIT		ZPD9
B_3E5__:		BMI		B_3EA__
				JMP		NEXT_CMD

B_3EA__:		JMP		J_B9A__

B_3ED__:		ROL		A
				ADC		#$A0
				CMP		IN_BUF,X
				BNE		B_448__
				LDA		(ZPFE),Y
				ASL		A
				BMI		B_400__
				DEY
				LDA		(ZPFE),Y
				BMI		B_428__
				INY
B_400__:		STX		ZPC8
				TYA
				PHA
				LDX		#$00
				LDA		(ZPFE,X)
				TAX
B_409__:		LSR		A
				EOR		#$48
				ORA		(ZPFE),Y
				CMP		#$C0
				BCC		B_413__
				INX
B_413__:		INY
				BNE		B_409__
				PLA
				TAY
				TXA
				JMP		JB_4C0__

J_41C__:		INC		ZPF1
				LDX		ZPF1
				BEQ		ERR_2LONG
				STA		IN_BUF,X
B_425__:		RTS

B_426__:		LDX		ZPC8
B_428__:		LDA		#$A0
B_42A__:		INX
				CMP		IN_BUF,X
				BCS		B_42A__
				LDA		(ZPFE),Y
				AND		#$3F
				LSR		A
				BNE		B_3ED__
				LDA		IN_BUF,X
				BCS		B_442__
				ADC		#$3F
				CMP		#$1A
				BCC		B_4B1__
B_442__:		ADC		#$4F
				CMP		#$0A
				BCC		B_4B1__
B_448__:		LDX		ZPFD
B_44A__:		INY
				LDA		(ZPFE),Y
				AND		#$E0
				CMP		#$20
				BEQ		B_4CD__
				LDA		ZPA8,X
				STA		ZPC8
				LDA		ZPD1,X
				STA		ZPF1
B_45B__:		DEY
				LDA		(ZPFE),Y
				ASL		A
				BPL		B_45B__
				DEY
				BCS		B_49C__
				ASL		A
				BMI		B_49C__
				LDY		ZP58,X
				STY		ZPFF
				LDY		ZP80,X
				INX
				BPL		B_44A__
B_470__:		BEQ		B_425__
				CMP		#$7E
				BCS		B_498__
				DEX
				BPL		B_47D__
				LDY		#$06
				BPL		B_4A6__
B_47D__:		STY		ZP80,X
				LDY		ZPFF
				STY		ZP58,X
				LDY		ZPC8
				STY		ZPA8,X
				LDY		ZPF1
				STY		ZPD1,X
				AND		#$1F
				TAY
				LDA		ROM_EC20__,Y
J_491__:		ASL		A
				TAY
				LDA		#$76
				ROL		A
				STA		ZPFF
B_498__:		BNE		B_49B__
				INY
B_49B__:		INY
B_49C__:		STX		ZPFD
				LDA		(ZPFE),Y
				BMI		B_426__
				BNE		B_4A9__
				LDY		#$0E		; String offset of "SYNTAX"
B_4A6__:		JMP		JB_3E0__

B_4A9__:		CMP		#$03
				BCS		B_470__
				LSR		A
				LDX		ZPC8
				INX
B_4B1__:		LDA		IN_BUF,X
				BCC		B_4BA__
				CMP		#$A2
				BEQ		B_4C4__
B_4BA__:		CMP		#$DF
				BEQ		B_4C4__
				STX		ZPC8
JB_4C0__:		JSR		J_41C__
				INY
B_4C4__:		DEY
				LDX		ZPFD
B_4C7__:		LDA		(ZPFE),Y
				DEY
				ASL		A
				BPL		B_49C__
B_4CD__:		LDY		ZP58,X
				STY		ZPFF
				LDY		ZP80,X
				INX
				LDA		(ZPFE),Y
				AND		#$9F
				BNE		B_4C7__
				STA		ZPF2
				STA		ZPF3
				TYA
				PHA
				STX		ZPFD
				LDY		ZPD0,X
				STY		ZPC9
				CLC
B_4E7__:		LDA		#$0A
				STA		ZPF9
				LDX		#$00
				INY
				LDA		IN_BUF,Y
				AND		#$0F
B_4F3__:		ADC		ZPF2
				PHA
				TXA
				ADC		ZPF3
				BMI		B_517__
				TAX
				PLA
				DEC		ZPF9
				BNE		B_4F3__
				STA		ZPF2
				STX		ZPF3
				CPY		ZPF1
				BNE		B_4E7__
				LDY		ZPC9
				INY
				STY		ZPF1
				JSR		J_41C__
				PLA
				TAY
				LDA		ZPF3
				BCS		JB_4C0__
B_517__:		LDY		#$00
				BPL		B_4A6__
J_51B__:		STA		ZPF3
				STX		ZPF2
				LDX		#$04
				STX		ZPC9
B_523__:		LDA		#$B0
				STA		ZPF9
B_527__:		LDA		ZPF2
				CMP		DATA_E563__,X
				LDA		ZPF3
				SBC		DATA_E568__,X
				BCC		B_540__
				STA		ZPF3
				LDA		ZPF2
				SBC		DATA_E563__,X
				STA		ZPF2
				INC		ZPF9
				BNE		B_527__
B_540__:		LDA		ZPF9
				INX
				DEX
				BEQ		B_554__
				CMP		#$B0
				BEQ		B_54C__
				STA		ZPC9
B_54C__:		BIT		ZPC9
				BMI		B_554__
				LDA		ZPFA
				BEQ		B_55F__
B_554__:		JSR		OUT_CHAR
				BIT		ZPF8
				BPL		B_55F__
				STA		IN_BUF,Y
				INY
B_55F__:		DEX
				BPL		B_523__
				RTS

DATA_E563__:	.BYTE	$01		
				.BYTE	$0A
				.BYTE	$64
				.BYTE	$E8
				.BYTE	$10
DATA_E568__:	.BYTE	$00
				.BYTE	$00
				.BYTE	$00
				.BYTE	$03
				.BYTE	$27

J_56D__:		LDA		$CA
				STA		$E6
				LDA		$CB
				STA		$E7
J_575__:
				INX
JB_576__:		LDA		ZPE7
				STA		ZPE5
				LDA		ZPE6
				STA		LAST_CHROUT__
				CMP		ZP4C
				LDA		ZPE5
				SBC		ZP4D
				BCS		B_5AC__
				LDY		#$01
				LDA		(LAST_CHROUT__),Y
				SBC		ZPCE
				INY
				LDA		(LAST_CHROUT__),Y
				SBC		ZPCF
				BCS		B_5AC__
				LDY		#$00
				LDA		ZPE6
				ADC		(LAST_CHROUT__),Y
				STA		ZPE6
				BCC		B_5A0__
				INC		ZPE7
				CLC
B_5A0__:		INY
				LDA		ZPCE
				SBC		(LAST_CHROUT__),Y
				INY
				LDA		ZPCF
				SBC		(LAST_CHROUT__),Y
				BCS		JB_576__
B_5AC__:		RTS

J_5AD__:		LSR		ZPF8		; ------------------------------------------
				LDA		ZP4C
				STA		ZPCA
				LDA		ZP4D
				STA		ZPCB
J_5B7__:		LDA		ZP4A
				STA		ZPCC
				LDA		ZP4B
				STA		ZPCD
				LDA		#$00
				STA		ZPFB
				STA		ZPFC
				STA		ZPFE
				LDA		#$00
				STA		ZP1D
				RTS					; ------------------------------------------

B_5CC__:		LDA		ZPD0
				ADC		#$05
				STA		ZPD2
				LDA		ZPD1
				ADC		#$00
				STA		ZPD3
				LDA		ZPD2
				CMP		ZPCA
				LDA		ZPD3
				SBC		ZPCB
				BCC		B_5E5__
				JMP		ERR_MEMFL

B_5E5__:		LDA		ZPCE
				STA		(ZPD0),Y
				LDA		ZPCF
				INY
				STA		(ZPD0),Y
				LDA		ZPD2
				INY
				STA		(ZPD0),Y
				LDA		ZPD3
				INY
				STA		(ZPD0),Y
				LDA		#$00
				INY
				STA		(ZPD0),Y
				INY
				STA		(ZPD0),Y
				LDA		ZPD2
				STA		ZPCC
				LDA		ZPD3
				STA		ZPCD
				LDA		ZPD0
				BCC		B_64F__
B_60C__:		STA		ZPCE
				STY		ZPCF
				JSR		J_6FF__
				BMI		B_623__
				CMP		#$40
				BEQ		B_623__
				JMP		J_628__

				ASL		ZPC9
				EOR		#$D0
				.BYTE	$07
				LDA		#$49
B_623__:		STA		ZPCF
				JSR		J_6FF__
J_628__:
				LDA		ZP4B
				STA		ZPD1
				LDA		ZP4A
B_62E__:		STA		ZPD0
				CMP		ZPCC
				LDA		ZPD1
				SBC		ZPCD
				BCS		B_5CC__
				LDA		(ZPD0),Y
				INY
				CMP		ZPCE
				BNE		B_645__
				LDA		(ZPD0),Y
				CMP		ZPCF
				BEQ		B_653__
B_645__:		INY
				LDA		(ZPD0),Y
				PHA
				INY
				LDA		(ZPD0),Y
				STA		ZPD1
				PLA
B_64F__:		LDY		#$00
				BEQ		B_62E__
B_653__:		LDA		ZPD0
				ADC		#$03
				JSR		J_70A__
				LDA		ZPD1
				ADC		#$00
				STA		ZP78,X
				LDA		ZPCF
				CMP		#$40
				BNE		B_682__
				DEY
				TYA
				JSR		J_70A__
				DEY
				STY		ZP78,X
				LDY		#$03
B_670__:		INC		ZP78,X
				INY
				LDA		(ZPD0),Y
				BMI		B_670__
				BPL		B_682__
J_679__:		LDA		#$00
				STA		ZPD4
				STA		ZPD5
				LDX		#$20
B_681__:		PHA
B_682__:		LDY		#$00
				LDA		(ZPE0),Y
B_686__:		BPL		B_6A0__
				ASL		A
				BMI		B_60C__
				JSR		J_6FF__
				JSR		J_708__
				JSR		J_6FF__
				STA		ZPA0,X
B_696__:		BIT		ZPD4
				BPL		B_69B__
				DEX
B_69B__:		JSR		J_6FF__
				BCS		B_686__
B_6A0__:		CMP		#$28
				BNE		B_6C3__
				LDA		ZPE0
				JSR		J_70A__
				LDA		ZPE1
				STA		ZP78,X
				BIT		ZPD4
				BMI		B_6BC__
				LDA		#$01
				JSR		J_70A__
				LDA		#$00
				STA		ZP78,X
B_6BA__:		INC		ZP78,X
B_6BC__:		JSR		J_6FF__
				BMI		B_6BA__
				BCS		B_696__
B_6C3__:		BIT		ZPD4
				BPL		B_6CD__
				CMP		#$04
				BCS		B_69B__
				LSR		ZPD4
B_6CD__:		TAY
				STA		ZPD6
				LDA		ROM_E998__,Y
				AND		#$55
				ASL		A
				STA		ZPD7
J_6D8__:		PLA
				TAY
				LDA		ROM_E998__,Y
				AND		#$AA
				CMP		ZPD7
				BCS		B_6EC__
				TYA
				PHA
				JSR		J_6FF__
				LDA		ZPD6
				BCC		B_681__
B_6EC__:		LDA		ROM_EA10__,Y
				STA		ZPCE
				LDA		ROM_EA88__,Y
				STA		ZPCF
				JSR		J_6FC__
				JMP		J_6D8__

J_6FC__:		JMP		(ZPCE)

J_6FF__:		INC		ZPE0
				BNE		B_705__
				INC		ZPE1
B_705__:		LDA		(ZPE0),Y
				RTS

J_708__:		STY		ZP77,X
J_70A__:		DEX
				BMI		B_710__
				STA		ZP50,X
				RTS

B_710__:		LDY		#$66
B_712__:		JMP		JB_3E0__

J_715__:		LDY		#$00
				LDA		ZP50,X
				STA		ZPCE
				LDA		ZPA0,X
				STA		ZPCF
				LDA		ZP78,X
				BEQ		B_731__
				STA		ZPCF
				LDA		(ZPCE),Y
				PHA
				INY
				LDA		(ZPCE),Y
				STA		ZPCF
				PLA
				STA		ZPCE
				DEY
B_731__:		INX
				RTS

				JSR		J_74A__
J_736__:		JSR		J_715__
				TYA
				JSR		J_708__
				STA		ZPA0,X
				CMP		ZPCE
				BNE		B_749__
				CMP		ZPCF
				BNE		B_749__
				INC		ZP50,X
B_749__:		RTS

J_74A__:		JSR		J_782__
				JSR		J_759__
				JSR		J_715__
				BIT		ZPCF
				BMI		JB_772__
B_757__:		DEX
B_758__:		RTS

J_759__:		JSR		J_715__
				LDA		ZPCF
				BNE		B_764__
				LDA		ZPCE
				BEQ		B_757__
B_764__:		LDA		#$FF
				JSR		J_708__
				STA		ZPA0,X
				BIT		ZPCF
				BMI		B_758__
J_76F__:		JSR		J_715__
JB_772__:		TYA
				SEC
				SBC		ZPCE
				JSR		J_708__
				TYA
				SBC		ZPCF
				BVC		B_7A1__
JB_77E__:		LDY		#$00
				BPL		B_712__
J_782__:		JSR		J_76F__
				JSR		J_715__
				LDA		ZPCE
				STA		ZPDA
				LDA		ZPCF
				STA		ZPDB
				JSR		J_715__
J_793__:		CLC
				LDA		ZPCE
				ADC		ZPDA
				JSR		J_708__
				LDA		ZPCF
				ADC		ZPDB
				BVS		JB_77E__
B_7A1__:		STA		ZPA0,X
				RTS

				JSR		J_715__
				LDY		ZPCE
				BEQ		B_7B0__
				DEY
				LDA		ZPCF
				BEQ		B_7BC__
B_7B0__:		RTS

J_7B1__:		LDA		IN_COUNT
				ORA		#$07
				TAY
				INY
B_7B7__:		LDA		#$A0
				JSR		OUT_CHAR
B_7BC__:		CPY		IN_COUNT
				BCS		B_7B7__
				RTS

				JSR		J_7B1__
B_7C4__:		JSR		J_715__
				LDA		ZPCF
				BPL		B_7D5__
				LDA		#$AD
				JSR		OUT_CHAR
				JSR		JB_772__
				BVC		B_7C4__
B_7D5__:		DEY
				STY		ZPD5
				STX		ZPCF
				LDX		ZPCE
				JSR		J_51B__
				LDX		ZPCF
				RTS

				JSR		J_715__
				LDA		ZPCE
				STA		ZPF6
				LDA		ZPCF
				STA		ZPF7
				DEY
				STY		ZPF8
				INY
				LDA		#$0A
B_7F3__:		STA		ZPF4
				STY		ZPF5
				RTS

				JSR		J_715__
				LDA		ZPCE
				LDY		ZPCF
				BPL		B_7F3__
J_801__:		JSR		J_715__
				LDA		ZP50,X
				STA		ZPDA
				LDA		ZP78,X
				STA		ZPDB
				LDA		ZPCE
				STA		(ZPDA),Y
				INY
				LDA		ZPCF
				STA		(ZPDA),Y
				INX
				RTS

				.BYTE	$68, $68, $24, $D5, $10, $05, $20, $CD
				.BYTE	$E3, $46, $D5, $60

J_823__:		LDY		#$FF
				STY		ZPD7
				RTS

				JSR		J_FCD__
				BEQ		B_834__
				LDA		#$25
				STA		ZPD6
				DEY
				STY		ZPD4
B_834__:		INX
				RTS

J_836__:		LDA		ZPCA
				LDY		ZPCB
				BNE		B_896__
				LDY		#$41
				LDA		ZPFC
				CMP		#$08
				BCS		B_8A2__
				TAY
				INC		ZPFC
				LDA		ZPE0
				STA		RAM_0100__,Y
				LDA		ZPE1
				STA		RAM_0108__,Y
				LDA		ZPDC
				STA		RAM_0110__,Y
				LDA		ZPDD
				STA		RAM_0118__,Y
J_85B__:		JSR		J_715__
				JSR		J_56D__
				BCC		B_867__
				LDY		#$37
				BNE		B_8A2__
B_867__:		LDA		LAST_CHROUT__
				LDY		ZPE5
B_86B__:		STA		ZPDC
				STY		ZPDD
				BIT		KBDCR
				BMI		JB_8C3__
				CLC
				ADC		#$03
				BCC		JB_87A__
				INY
JB_87A__:		LDX		#$FF
				STX		ZPD9
				TXS
				STA		ZPE0
				STY		ZPE1
J_883__:		JSR		J_679__
				BIT		ZPD9
				BPL		B_8D3__
				CLC
				LDY		#$00
				LDA		ZPDC
				ADC		(ZPDC),Y
				LDY		ZPDD
				BCC		B_896__
				INY
B_896__:		CMP		ZP4C
				BNE		B_86B__
				CPY		ZP4D
				BNE		B_86B__
				LDY		#$34		; String offset of "END"
				LSR		ZPD9
B_8A2__:		JMP		JB_3E0__

				LDY		#$4A
				LDA		ZPFC
				BEQ		B_8A2__
				DEC		ZPFC
				TAY
				LDA		RAM_010F__,Y
				STA		ZPDC
				LDA		RAM_0117__,Y
				STA		ZPDD
				LDX		A:ZPFF,Y
				LDA		RAM_0107__,Y
B_8BE__:		TAY
				TXA
				JMP		JB_87A__

JB_8C3__:		LDY		#$63		; String offset of "STOPPED AT "
				JSR		OUT_STR
				LDY		#$01
				LDA		(ZPDC),Y
				TAX
				INY
				LDA		(ZPDC),Y
				JSR		J_51B__
B_8D3__:		JMP		RET_CLI

B_8D6__:		DEC		ZPFB
				LDY		#$5B
				LDA		ZPFB
B_8DC__:		BEQ		B_8A2__
				TAY
				LDA		ZP50,X
				CMP		RAM_011F__,Y
				BNE		B_8D6__
				LDA		ZP78,X
				CMP		RAM_0127__,Y
				BNE		B_8D6__
				LDA		RAM_012F__,Y
				STA		ZPDA
				LDA		RAM_0137__,Y
				STA		ZPDB
				JSR		J_715__
				DEX
				JSR		J_793__
				JSR		J_801__
				DEX
				LDY		ZPFB
				LDA		RAM_0167__,Y
				STA		ZP9F,X
				LDA		RAM_015F__,Y
				LDY		#$00
				JSR		J_708__
				JSR		J_782__
				JSR		J_759__
				JSR		J_715__
				LDY		ZPFB
				LDA		ZPCE
				BEQ		B_925__
				EOR		RAM_0137__,Y
				BPL		B_937__
B_925__:		LDA		RAM_013F__,Y
				STA		ZPDC
				LDA		RAM_0147__,Y
				STA		ZPDD
				LDX		RAM_014F__,Y
				LDA		RAM_0157__,Y
				BNE		B_8BE__
B_937__:		DEC		ZPFB
				RTS

				LDY		#$54
				LDA		ZPFB
				CMP		#$08
				BEQ		B_8DC__
				INC		ZPFB
				TAY
				LDA		ZP50,X
				STA		RAM_0120__,Y
				LDA		ZP78,X
				STA		RAM_0128__,Y
				RTS

				JSR		J_715__
				LDY		ZPFB
				LDA		ZPCE
				STA		RAM_015F__,Y
				LDA		ZPCF
				STA		RAM_0167__,Y
				LDA		#$01
				STA		RAM_012F__,Y
				LDA		#$00
J_966__:		STA		RAM_0137__,Y
				LDA		$DC
				STA		RAM_013F__,Y
				LDA		$DD
				STA		RAM_0147__,Y
				LDA		ZPE0
				STA		RAM_014F__,Y
				LDA		ZPE1
				STA		RAM_0157__,Y
				RTS

				JSR		J_715__
				LDY		ZPFB
				LDA		ZPCE
				STA		RAM_012F__,Y
				LDA		ZPCF
				JMP		J_966__

; Data @ $E98D
				.BYTE	$00, $00, $00
; Data @ $E990
				.BYTE	$00, $00, $00, $00, $00, $00, $00, $00
ROM_E998__:		.BYTE	$00, $00, $00, $AB, $03, $03, $03, $03
				.BYTE	$03, $03, $03, $03, $03, $03, $03, $03
				.BYTE	$03, $03, $3F, $3F, $C0, $C0, $3C, $3C
				.BYTE	$3C, $3C, $3C, $3C, $3C, $30, $0F, $C0
				.BYTE	$CC, $FF, $55, $00, $AB, $AB, $03, $03
				.BYTE	$FF, $FF, $55, $FF, $FF, $55, $CF, $CF
				.BYTE	$CF, $CF, $CF, $FF, $55, $C3, $C3, $C3
				.BYTE	$55, $F0, $F0, $CF, $56, $56, $56, $55
				.BYTE	$FF, $FF, $55, $03, $03, $03, $03, $03
				.BYTE	$03, $03, $FF, $FF, $FF, $03, $03, $03
				.BYTE	$03, $03, $03, $03, $03, $03, $03, $03
				.BYTE	$03, $03, $03, $03, $03, $00, $AB, $03
				.BYTE	$57, $03, $03, $03, $03, $07, $03, $03
; Data @ $EA00
				.BYTE	$03, $03, $03, $03, $03, $03, $03, $03
				.BYTE	$03, $03, $AA, $FF, $FF, $FF, $FF, $FF
ROM_EA10__:		.BYTE	$17, $FF, $FF, $19, $5D, $35, $4B, $F2
				.BYTE	$EC, $87, $6F, $AD, $B7, $E2, $F8, $54
				.BYTE	$80, $96, $85, $82, $22, $10, $33, $4A
				.BYTE	$13, $06, $0B, $4A, $01, $40, $47, $7A
				.BYTE	$00, $FF, $23, $09, $5B, $16, $B6, $CB
				.BYTE	$FF, $FF, $FB, $FF, $FF, $24, $F6, $4E
				.BYTE	$59, $50, $00, $FF, $23, $A3, $6F, $36
				.BYTE	$23, $D7, $1C, $22, $C2, $AE, $BA, $23
				.BYTE	$FF, $FF, $21, $30, $1E, $03, $C4, $20
				.BYTE	$00, $C1, $FF, $FF, $FF, $A0, $30, $1E
				.BYTE	$A4, $D3, $B6, $BC, $AA, $3A, $01, $50
				.BYTE	$7E, $D8, $D8, $A5, $3C, $FF, $16, $5B
				.BYTE	$28, $03, $C4, $1D, $00, $0C, $4E, $00
				.BYTE	$3E, $00, $A6, $B0, $00, $BC, $C6, $57
				.BYTE	$8C, $01, $27, $FF, $FF, $FF, $FF, $FF
ROM_EA88__:		.BYTE	$E8, $FF, $FF, $E8, $E0, $E0, $E0, $EF
				.BYTE	$EF, $E3, $E3, $E5, $E5, $E7, $E7, $EE
				.BYTE	$EF, $EF, $E7, $E7, $E2, $EF, $E7, $E7
				.BYTE	$EC, $EC, $EC, $E7, $EC, $EC, $EC, $E2
				.BYTE	$00, $FF, $E8, $E1, $E8, $E8, $EF, $EB
				.BYTE	$FF, $FF, $E0, $FF, $FF, $EF, $EE, $EF
				.BYTE	$E7, $E7, $00, $FF, $E8, $E7, $E7, $E7
				.BYTE	$E8, $E1, $E2, $EE, $EE, $EE, $EE, $E8
				.BYTE	$FF, $FF, $E1, $E1, $EF, $EE, $E7, $E8
				.BYTE	$EE, $E7, $FF, $FF, $FF, $EE, $E1, $EF
				.BYTE	$E7, $E8, $EF, $EF, $EB, $E9, $E8, $E9
				.BYTE	$E9, $E8, $E8, $E8, $E8, $FF, $E8, $E8
				.BYTE	$E8, $EE, $E7, $E8, $EF, $EF, $EE, $EF
				.BYTE	$EE, $EF, $EE, $EE, $EF, $EE, $EE, $EE
				.BYTE	$E1, $E8, $E8, $FF, $FF, $FF, $FF, $FF
				
; STRINGS @ $EB00
				; ">32767"				OFFS = $00
DATA_STR:		.BYTE	$BE, $B3, $B2, $B7, $B6, $37

				; "TOO LONG"			OFFS = $06
				.BYTE	$D4, $CF, $CF, $A0, $CC, $CF, $CE, $47

				; "SYNTAX"				OFFS = $0E
				.BYTE	$D3, $D9, $CE, $D4, $C1, $58

				; "MEM FULL"			OFFS = $14
				.BYTE	$CD, $C5, $CD, $A0, $C6, $D5, $CC, $4C

				; "TOO MANY PARENS"		OFFS = $1C
				.BYTE	$D4, $CF, $CF, $A0, $CD, $C1, $CE, $D9, $A0, $D0, $C1, $D2, $C5, $CE, $53

				; "STRING"				OFFS = $2B
				.BYTE	$D3, $D4, $D2, $C9, $CE, $47

				; "NO END"				OFFS = $31
				; "END"					OFFS = $34
				.BYTE	$CE, $CF, $A0
				.BYTE	$C5, $CE, $44

				; "BAD BRANCH"			OFFS = $37
				.BYTE	$C2, $C1, $C4, $A0, $C2, $D2, $C1, $CE, $C3, $48

				; ">8 GOSUBS"			OFFS = $41
				.BYTE	$BE, $B8, $A0, $C7, $CF, $D3, $D5, $C2, $53

				; "BAD RETURN"			OFFS = $4A
				.BYTE	$C2, $C1, $C4, $A0, $D2, $C5, $D4, $D5, $D2, $4E

				; ">8 FORS"				OFFS = $54
				.BYTE	$BE, $B8, $A0, $C6, $CF, $D2, $53

				; "BAD NEXT"			OFFS = $5B
				.BYTE	$C2, $C1, $C4, $A0, $CE, $C5, $D8, $54

				; "STOPPED AT "			OFFS = $63
				.BYTE	$D3, $D4, $CF, $D0, $D0, $C5, $C4, $A0, $C1, $D4, $20

				; "*** "				OFFS = $6E
				.BYTE	$AA, $AA, $AA, $20

				; "ERR\r"				OFFS = $72
				.BYTE	$A0, $C5, $D2, $D2, $0D

				; ">255"				OFFS = $77
				.BYTE	$BE, $B2, $B5, $35

				; "RANGE"				OFFS = $7B
				.BYTE	$D2, $C1, $CE, $C7, $45

				; "DIMSTR OVFL"			OFFS = $80
				.BYTE	$C4, $C9, $4D, $D3, $D4, $D2, $A0, $CF, $D6, $C6, $4C
				
				; "\\r"					OFFS = $8B
				.BYTE	$DC, $0D

				; "RETYPE LINE\r?"		OFFS = $8D
				; "?"					OFFS = $99
				.BYTE	$D2, $C5, $D4 ,$D9, $D0, $C5, $A0, $CC, $C9, $CE, $C5, $8D
				.BYTE	$3F

J_B9A__:		LSR		ZPD9
				BCC		B_BA1__
				JMP		JB_8C3__

B_BA1__:		LDX		ZPCF
				TXS
				LDX		ZPCE
				LDY		#$8D		; String offset of "RETYPE LINE\r?"
				BNE		B_BAC__
B_BAA__:		LDY		#$99		
B_BAC__:		JSR		OUT_STR
				STX		ZPCE
				TSX
				STX		ZPCF
				LDY		#$FE
				STY		ZPD9
				INY
				STY		ZPC8
				JSR		J_299__
				STY		ZPF1
				LDX		#$20
				LDA		#$30
				JSR		J_491__
				INC		ZPD9
				LDX		ZPCE
				LDY		ZPC8
				ASL		A
B_BCE__:		STA		ZPCE
				INY
				LDA		IN_BUF,Y
				CMP		#$74
				BEQ		B_BAA__
				EOR		#$B0
				CMP		#$0A
				BCS		B_BCE__
				INY
				INY
				STY		ZPC8
				.BYTE	$B9
B_BE3__:		.BYTE	$00
				.BYTE	$02, $48
				LDA		$01FF,Y
				LDY		#$00
				JSR		J_708__
				PLA
				STA		ZPA0,X
				LDA		ZPCE
				CMP		#$C7
				BNE		B_BFA__
				JSR		J_76F__
B_BFA__:		JMP		J_801__

				.BYTE	$FF
				.BYTE	$FF
				.BYTE	$FF
				BVC		B_C22__
				.BYTE	$13
				.BYTE	$EC, $D0, $15
				JSR		J_C0B__
				BNE		B_C1B__
J_C0B__:		JSR		J_782__
				JSR		J_76F__
				BVC		B_C16__
				JSR		J_782__
B_C16__:		JSR		J_759__
				LSR		ZP50,X
B_C1B__:		JMP		J_736__

				.BYTE	$FF
				.BYTE	$FF
ROM_EC20__:		.BYTE	$C1, $FF
B_C22__:		.BYTE	$7F
				CMP		(ZPCC),Y
				.BYTE	$C7
				.BYTE	$CF
				.BYTE	$CE, $C5, $9A
				TYA
				.BYTE	$8B
				STX		ZP95,Y
				.BYTE	$93
				.BYTE	$BF
				.BYTE	$B2
				.BYTE	$32, $2D, $2B, $BC
				BCS		B_BE3__
				.BYTE	$BE, $35, $8E
				ADC		(ZPFF,X)
				.BYTE	$FF
				.BYTE	$FF
				.BYTE	$DD, $FB, $20
				CMP		#$EF
				ORA		ZP4F,X
				BPL		B_C4C__
				JSR		J_FC9__
				.BYTE	$35, $4F
B_C4C__:		STA		ZP50,X
				BPL		B_C1B__
				JMP		J_FC9__

				RTI

				RTS

				STA		$8B60
				.BYTE	$00, $7E, $8C, $33
				.BYTE	$00
				.BYTE	$00
				RTS

				.BYTE	$03
				.BYTE	$BF
				.BYTE	$12
				.BYTE	$00
				RTI

				.BYTE	$89
				CMP		#$47
				STA		$6817,X
				STA		A:ZP0A,X
				RTI

				RTS

				STA		$8B60
				.BYTE	$00, $7E, $8C, $3C
				.BYTE	$00
				.BYTE	$00
				RTS

				.BYTE	$03
				.BYTE	$BF
				.BYTE	$1B
				.BYTE	$4B
				.BYTE	$67
				.BYTE	$B4
B_C7F__:		LDA		(ZP07,X)
				STY		$AE07
				LDA		#$AC
				TAY
				.BYTE	$67
				STY		$B407
				.BYTE	$AF
				LDY		$67B0
				.BYTE	$9D
				.BYTE	$B2
B_C91__:		.BYTE	$AF
				LDY		$A3AF
				.BYTE	$67
				STY		$A507
				.BYTE	$AB
B_C9A__:		.BYTE	$AF
				BCS		B_C91__
				LDX		$B2A9
				BCS		B_D21__
B_CA2__:		ASL		$B427
				LDX		$B2A9
				BCS		B_D29__
B_CAA__:		ASL		$B428
				LDX		$B2A9
				BCS		B_D16__
				.BYTE	$07
				LDX		$A9
				.BYTE	$67
				.BYTE	$AF
				LDY		ZPAF,X
				.BYTE	$A7
				SEI
				LDY		ZPA5,X
				LDY		$7F78
				.BYTE	$02
				LDA		$B2A5
				.BYTE	$67
				.BYTE	$A2, $B5
				.BYTE	$B3
				.BYTE	$AF
				.BYTE	$A7
				.BYTE	$EE, $B2, $B5
				LDY		ZPA5,X
				.BYTE	$B2, $7E, $8C, $39
				LDY		ZPB8,X
				.BYTE	$A5, $AE
				.BYTE	$67
				BCS		B_C7F__
				LDY		ZPB3,X
				.BYTE	$27
				.BYTE	$AF
				LDY		ZP07,X
				STA		$B219,X
				.BYTE	$AF
				LDX		$7F
				ORA		$37
				LDY		ZPB5,X
				BCS		B_C9A__
				LDA		#$7F
				ORA		$28
				LDY		ZPB5,X
				BCS		B_CA2__
				LDA		#$7F
				ORA		$2A
				LDY		ZPB5,X
				BCS		B_CAA__
				LDA		#$E4
				LDX		A:$A5
				.BYTE	$FF
				.BYTE	$FF
				.BYTE	$47
				.BYTE	$A2
				.BYTE	$A1
				.BYTE	$B4
				.BYTE	$7F
				.BYTE	$0D
				.BYTE	$30
				.BYTE	$AD
				.BYTE	$A9
				.BYTE	$A4
				.BYTE	$7F
				.BYTE	$0D
				.BYTE	$23
				.BYTE	$AD
				.BYTE	$A9		
				.BYTE	$A4
				.BYTE	$67
				.BYTE	$AC
				.BYTE	$AC

B_D16__:		LDA		(ZPA3,X)
				.BYTE	$00
				RTI

				.BYTE	$80
				.BYTE	$C0		
				.BYTE	$C1
				.BYTE	$80
				.BYTE	$00
				.BYTE	$47
				.BYTE	$8C
B_D21__:		PLA
				STY		$67DB
				.BYTE	$9B
				PLA
				.BYTE	$9B
				.BYTE	$50
B_D29__:		STY		$8C63
				.BYTE	$7F
				ORA		(ZP51,X)
				.BYTE	$07
				DEY
				.BYTE	$29, $84
				.BYTE	$80
				CPY		$80
				.BYTE	$57
				ADC		(ZP07),Y
				DEY
				.BYTE	$14
				SBC		$ADA5
				.BYTE	$AF
				LDY		$A5ED
				LDA		$A8A9
				.BYTE	$F2
				.BYTE	$AF
				LDY		$A3AF
				ADC		(ZP08),Y
				DEY
				.BYTE	$AE, $A5, $AC
				PLA
				.BYTE	$83
				PHP
				PLA
				STA		$7108,X
				.BYTE	$07
				DEY
				RTS

				.BYTE	$76, $B4, $AF
				.BYTE	$AE, $76, $8D, $76, $8B
				EOR		(ZP07),Y
				DEY
				ORA		$A4B8,Y
				.BYTE	$AE
				.BYTE	$B2
				.BYTE	$F2
				.BYTE	$B3
				LDA		ZPF3,X
				.BYTE	$A2, $A1
				.BYTE	$EE, $A7, $B3
				CPX		$AE
				.BYTE	$B2
				.BYTE	$EB
				.BYTE	$A5, $A5
				BCS		B_DCC__
				.BYTE	$07
				DEY
				.BYTE	$39, $81, $C1, $4F
				.BYTE	$7F
				.BYTE	$0F
				.BYTE	$2F
				.BYTE	$00
				EOR		(ZP06),Y
				DEY
				.BYTE	$29, $C2, $0C
				.BYTE	$82
				.BYTE	$57
				STY		$8C6A
				.BYTE	$42
				.BYTE	$AE, $A5, $A8
				LDY		ZP60,X
				.BYTE	$AE, $A5, $A8
				LDY		ZP4F,X
				.BYTE	$7E, $1E, $35
				STY		$5127
				.BYTE	$07
				DEY
				ORA		#$8B
				.BYTE	$FE, $E4, $AF
				LDA		$AFF2
				CPX		$AE
				LDA		(ZPDC,X)
				.BYTE	$DE, $9C, $DD
				.BYTE	$9C
				DEC		$9EDD,X
				.BYTE	$C3
				.BYTE	$DD, $CF, $CA
				CMP		A:$CB
				.BYTE	$47
				STA		$A5AD,X
				LDA		$ACAF
				.BYTE	$76, $9D
				LDA		$ADA5
				LDA		#$A8
				.BYTE	$E6
B_DCC__:		LDX		$AF
				RTS

				STY		$AF20
				LDY		ZPB5,X
				LDA		(ZPF2,X)
				.BYTE	$AC		
				.BYTE	$A3
				.BYTE	$F2
				.BYTE	$A3
				.BYTE	$B3
				RTS

				STY		$AC20
				.BYTE	$A5, $A4
				.BYTE	$EE, $B5, $B2
				RTS

				.BYTE	$AE, $B5, $B2
				.BYTE	$F4
				.BYTE	$B3
				LDA		#$AC
				RTS

				STY		$B420
				.BYTE	$B3
				LDA		#$AC
				.BYTE	$7A
				ROR		$229A,X
				JSR		RAM_6000__
				.BYTE	$03
				.BYTE	$BF
				RTS

				.BYTE	$03
				.BYTE	$BF
				.BYTE	$1F
				JSR		J_7B1__
J_E03__:		INX
				INX
				LDA		ZP4F,X
				STA		ZPDA
				LDA		ZP77,X
				STA		ZPDB
				LDY		ZP4E,X
J_E0F__:		TYA
				CMP		ZP76,X
				BCS		B_E1D__
				LDA		(ZPDA),Y
				JSR		OUT_CHAR
				INY
				JMP		J_E0F__

B_E1D__:		LDA		#$FF
				STA		$D5
				RTS

				INX
				LDA		#$00
				STA		ZP78,X
				STA		ZPA0,X
				LDA		ZP77,X
				SEC
				SBC		ZP4F,X
				STA		ZP50,X
				JMP		J_823__

				.BYTE	$FF
J_E34__:		JSR		J_715__
				LDA		ZPCF
				BNE		ERR_LT255
				LDA		ZPCE
				RTS

				JSR		J_E34__
				LDY		ZPC8
				CMP		#$30
				BCS		B_E68__
				CPY		#$28
				BCS		B_E68__
				RTS

				NOP
				NOP
				JSR		J_E34__
				RTS

				NOP
J_E53__:		TXA
				LDX		#$01
B_E56__:		LDY		ZPCE,X
				STY		ZP4C,X
				LDY		ZP48,X
				STY		ZPCA,X
				DEX
				BEQ		B_E56__
				TAX
				RTS

ERR_LT255:		LDY		#$77		; String offset of ">255"
B_E65__:		JMP		JB_3E0__

B_E68__:		LDY		#$7B
				BNE		B_E65__
J_E6C__:		JSR		J_254__
				LDA		ZPDA
				BNE		B_E7A__
				LDA		ZPDB
				BNE		B_E7A__
				JMP		JB_77E__

B_E7A__:		ASL		ZPCE
				ROL		ZPCF
				ROL		ZPE6
				ROL		ZPE7
				LDA		ZPE6
				CMP		ZPDA
				LDA		ZPE7
				SBC		ZPDB
				BCC		B_E96__
				STA		ZPE7
				LDA		ZPE6
				SBC		ZPDA
				STA		ZPE6
				INC		ZPCE
B_E96__:		DEY
				BNE		B_E7A__
				RTS

				.BYTE	$FF
				.BYTE	$FF
				.BYTE	$FF
				.BYTE	$FF
				.BYTE	$FF
				.BYTE	$FF
				JSR		J_715__
				JMP		(ZPCE)

B_EA6__:		LDA		ZP4C
				BNE		B_EAC__
				DEC		ZP4D
B_EAC__:		DEC		ZP4C
				LDA		$48
				BNE		B_EB4__
				DEC		$49
B_EB4__:		DEC		$48
J_EB6__:		LDY		#$00
				LDA		(ZP4C),Y
				STA		(ZP48),Y
				LDA		ZPCA
				CMP		ZP4C
				LDA		ZPCB
				SBC		ZP4D
				BCC		B_EA6__
				JMP		J_E53__

				CMP		#$28
JB_ECB__:		BCS		B_E68__
				TAY
				LDA		ZPC8
				RTS

				NOP
				NOP

; Prints error message from string offset = Y ----------------------------------
PRINT_ERR:		TYA
				TAX
				LDY		#$6E		; String offset of "*** "
				JSR		OUT_STR
				TXA
				TAY
				JSR		OUT_STR
				LDY		#$72		; String offset of "ERR\r"
				JMP		OUT_STR
;-------------------------------------------------------------------------------

J_EE4__:		JSR		J_715__
B_EE7__:		ASL		ZPCE
				ROL		ZPCF
				BMI		B_EE7__
				BCS		JB_ECB__
				BNE		B_EF5__
				CMP		ZPCE
				BCS		JB_ECB__
B_EF5__:		RTS

				JSR		J_715__
				LDA		(ZPCE),Y
				STY		ZP9F,X
				JMP		J_708__

				JSR		J_E34__
				LDA		ZPCE
				PHA
				JSR		J_715__
				PLA
				STA		(ZPCE),Y
				RTS

				.BYTE	$FF
				.BYTE	$FF
				.BYTE	$FF
				JSR		J_E6C__
				LDA		ZPCE
				STA		ZPE6
				LDA		ZPCF
				STA		ZPE7
				JMP		JB_244__

				JSR		J_EE4__
				JMP		J_134__

				JSR		J_EE4__
				LDY		ZP78,X
				LDA		ZP50,X
				ADC		#$FE
				BCS		B_F30__
				DEY
B_F30__:		STA		ZPDA
				STY		ZPDB
				CLC
				ADC		ZPCE
				STA		ZP50,X
				TYA
				ADC		ZPCF
				STA		ZP78,X
				LDY		#$00
				LDA		ZP50,X
				CMP		(ZPDA),Y
				INY
				LDA		ZP78,X
				SBC		(ZPDA),Y
				BCS		JB_ECB__
				JMP		J_823__

				JSR		J_715__
				LDA		ZP4E
				JSR		J_708__
				LDA		$4F
				BNE		B_F5E__
				CMP		ZP4E
				ADC		#$00
B_F5E__:		AND		#$7F
				STA		$4F
				STA		ZPA0,X
				LDY		#$11
B_F66__:		LDA		$4F
				ASL		A
				CLC
				ADC		#$40
				ASL		A
				ROL		ZP4E
				ROL		$4F
				DEY
				BNE		B_F66__
				LDA		ZPCE
				JSR		J_708__
				LDA		ZPCF
				STA		ZPA0,X
				JMP		J_27A__

				JSR		J_715__
				LDY		ZPCE
				CPY		ZP4C
				LDA		ZPCF
				SBC		ZP4D
				BCC		B_FAC__
				STY		$48
				LDA		ZPCF
				STA		$49
				JMP		J_EB6__

				JSR		J_715__
				LDY		ZPCE
				CPY		ZPCA
				LDA		ZPCF
				SBC		ZPCB
				BCS		B_FAC__
				STY		$4A
				LDA		ZPCF
				STA		$4B
				JMP		J_5B7__

B_FAC__:		JMP		JB_ECB__

				NOP
				NOP
				NOP
				NOP
				JSR		J_FC9__
				JSR		J_171__
				JMP		J_FBF__


				JSR		J_E03__
J_FBF__:		LDA		#$FF
				STA		ZPC8
				LDA		#$74
				STA		IN_BUF
				RTS

J_FC9__:		JSR		J_736__
				INX
J_FCD__:		JSR		J_736__
				LDA		ZP50,X
				RTS

J_FD3__:		LDA		#$00		; ------------------------------------------
				STA		$4A
				STA		ZP4C
				LDA		#$08
				STA		$4B
				LDA		#$10
				STA		ZP4D
				JMP		J_5AD__		; ------------------------------------------

J_FE4__:		.BYTE	$D5, $78
				.BYTE	$D0
B_FE7__:		.BYTE	$01
				CLC
				JMP		J_102__

				JSR		J_5B7__
				JMP		J_836__

				JSR		J_5B7__
				JMP		J_85B__

J_FF8__:		CPX		#$80
				BNE		B_FFD__
				DEY
B_FFD__:		JMP		J_00C__

