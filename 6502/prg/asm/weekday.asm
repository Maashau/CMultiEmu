; Input: Y = year (0=1900, 1=1901, ..., 255=2155)
;        X = month (1=Jan, 2=Feb, ..., 12=Dec)
;        A = day (1 to 31)
;
; Output: Weekday in A (0=Sunday, 1=Monday, ..., 6=Saturday)

; Entry point
.ORG $0000
        LDY     #124
        LDX     #8
        LDA     #19
        JSR     WEEKDAY
        BRK

WEEKDAY:
        CPX #3          ; Year starts in March to bypass
        BCS MARCH       ; leap year problem
        DEY             ; If Jan or Feb, decrement year
MARCH:  EOR #$7F        ; Invert A so carry works right
        CPY #200        ; Carry will be 1 if 22nd century
        ADC MTAB-1,X    ; A is now day+month offset
        STA TMP
        TYA             ; Get the year
        JSR MOD7        ; Do a modulo to prevent overflow
        SBC TMP         ; Combine with day+month
        STA TMP
        TYA             ; Get the year again
        LSR             ; Divide it by 4
        LSR
        CLC             ; Add it to y+m+d and fall through
        ADC TMP
MOD7:   ADC #7          ; Returns (A+3) modulo 7
        BCC MOD7        ; for A in 0..255
        RTS

; Data section.
MTAB:   .BYTE 1,5,6,3,1,5,3,0,4,2,6,4   	; Month offsets
TMP:    .BYTE $6                ; Temporary storage