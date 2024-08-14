        * = $0000
        LDX TXTY
        JSR PRTX
        LDA TXTY
        ADC #1
        JSR GETC
        BRK
        LDY #$78
        LDX #$03
        LDA #$07
        JSR L000D
L0009	BRK
        JMP L0009
L000D	CPX #$03
        BCS L0012
        DEY
L0012	EOR #$7F
        CPY #$C8
        ADC $0031,X
        STA $0040
        TYA
        JSR L002D
        SBC $0040
        STA $0040
        TYA
        LSR A
        LSR A
        CLC
        ADC $0040
L002D	ADC #$07
        BCC L002D
        RTS
        ORA ($05,X)
        ASL $03
        ORA ($05,X)
        .BYTE $03	;%00000011
        BRK
        .BYTE $04	;%00000100
        .BYTE $02	;%00000010
        ASL $04
        BRK
PRTX	LDA TXTY,X
        DEX
        STA SCRMEM,X
        CPX #$00
        BNE PRTX
        RTS
GETC	TAX
BLINK	LDA #$5F
BLONK	STA SCRMEM,X
        STX BSTORE
        LDY	#50
DELAY	LDX KBRD
        CPX #0
        BNE	GOTC
        DEY
        CPY #0
        BNE DELAY
        LDX BSTORE
        CMP #$5F
        BNE BLINK
        LDA #$20
        JMP BLONK
GOTC	RTS
BSTORE	.BYTE 0
TXTY	.BYTE 10
        .ASCII "ENTER YEAR"
TXTM	.BYTE 11
        .ASCII "ENTER MONTH"
TXTD	.BYTE 9
        .ASCII "ENTER DAY"
SCRMEM	EQU $F690
KBRD	EQU $FFF0
        .END