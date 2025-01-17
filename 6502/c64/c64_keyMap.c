
/* A = Column, B = Row*/
enum {  /*B0*/                /*B1*/               /*B2*/               /*B3*/               /*B4*/               /*B5*/               /*B6*/               /*B7*/
/*A0*/  C64_KB_DEL  = 0x0101, C64_KB_RET = 0x0102, C64_KB_CL  = 0x0104, C64_KB_F7  = 0x0108, C64_KB_F1  = 0x0110, C64_KB_F3  = 0x0120, C64_KB_F5  = 0x0140, C64_KB_CU    = 0x0180,
/*A1*/  C64_KB_3    = 0x0201, C64_KB_W   = 0x0202, C64_KB_A   = 0x0204, C64_KB_4   = 0x0208, C64_KB_Z   = 0x0210, C64_KB_S   = 0x0220, C64_KB_E   = 0x0240, C64_KB_LSH   = 0x0280,
/*A2*/  C64_KB_5    = 0x0401, C64_KB_R   = 0x0402, C64_KB_D   = 0x0404, C64_KB_6   = 0x0408, C64_KB_C   = 0x0410, C64_KB_F   = 0x0420, C64_KB_T   = 0x0440, C64_KB_X     = 0x0480,
/*A3*/  C64_KB_7    = 0x0801, C64_KB_Y   = 0x0802, C64_KB_G   = 0x0804, C64_KB_8   = 0x0808, C64_KB_B   = 0x0810, C64_KB_H   = 0x0820, C64_KB_U   = 0x0840, C64_KB_V     = 0x0880,
/*A4*/  C64_KB_9    = 0x1001, C64_KB_I   = 0x1002, C64_KB_J   = 0x1004, C64_KB_0   = 0x1008, C64_KB_M   = 0x1010, C64_KB_K   = 0x1020, C64_KB_O   = 0x1040, C64_KB_N     = 0x1080,
/*A5*/  C64_KB_PLS  = 0x2001, C64_KB_P   = 0x2002, C64_KB_L   = 0x2004, C64_KB_MIN = 0x2008, C64_KB_DOT = 0x2010, C64_KB_COL = 0x2020, C64_KB_AT  = 0x2040, C64_KB_COMMA = 0x2080,
/*A6*/  C64_KB_PND  = 0x4001, C64_KB_AST = 0x4002, C64_KB_SCO = 0x4004, C64_KB_HOM = 0x4008, C64_KB_RSH = 0x4010, C64_KB_EQ  = 0x4020, C64_KB_UAR = 0x4040, C64_KB_SLASH = 0x4080,
/*A7*/  C64_KB_1    = 0x8001, C64_KB_LAR = 0x8002, C64_KB_CTR = 0x8004, C64_KB_2   = 0x8008, C64_KB_SPC = 0x8010, C64_KB_COM = 0x8020, C64_KB_Q   = 0x8040, C64_KB_RUN   = 0x8080
};
/*
Unassigned:
C64_KB_CL
C64_KB_F7
C64_KB_F1
C64_KB_F3
C64_KB_F5
C64_KB_CU
C64_KB_PND
C64_KB_LSH
C64_KB_RSH
C64_KB_UAR
C64_KB_LAR
C64_KB_HOM
C64_KB_CTR
C64_KB_COM
*/
U16 c64_keyMap[] = {
    0, //	_NULL
    0, //	SOH
    0, //	STX
    0, //	ETX
    0, //	EOT
    0, //	ENQ
    0, //	ACK
    0, //	BEL
    0, //	BS
    C64_KB_RUN, //	TAB
    0, //	LF
    0, //	VT
    0, //	FF
    C64_KB_RET, //	CR
    0, //	SO
    0, //	SI
    0, //	DLE
    0, //	DC1
    0, //	DC2
    0, //	DC3
    0, //	DC4
    0, //	NAK
    0, //	SYN
    0, //	ETB
    0, //	CAN
    0, //	EM
    0, //	SUB
    0, //	ESC
    0, //	SF
    0, //	GS
    0, //	RS
    0, //	US
    C64_KB_SPC, //	SPACE
    C64_KB_LSH | C64_KB_1, //	EXCLAMAION_MARK
    C64_KB_LSH | C64_KB_2, //	QUOTATION_MARK_DOUBLE
    C64_KB_LSH | C64_KB_3, //	HASHTAG
    C64_KB_LSH | C64_KB_4, //	SING_DOLLAR
    C64_KB_LSH | C64_KB_5, //	PRECENT
    C64_KB_LSH | C64_KB_6, //	AMPERSANT
    C64_KB_LSH | C64_KB_7, //	QUOTATION_MARK_SINGLE
    C64_KB_LSH | C64_KB_8, //	PARENTHESES_OPEN
    C64_KB_LSH | C64_KB_9, //	PARENTHESES_CLOSE
    C64_KB_AST, //	ASTERISK
    C64_KB_PLS, //	PLUS
    C64_KB_COMMA, //	COMMA
    C64_KB_MIN, //	MINUS
    C64_KB_DOT, //	DOT
    C64_KB_SLASH, //	SLASH_FOWARD
    C64_KB_0, //	NUMBER_0
    C64_KB_1, //	NUMBER_1
    C64_KB_2, //	NUMBER_2
    C64_KB_3, //	NUMBER_3
    C64_KB_4, //	NUMBER_4
    C64_KB_5, //	NUMBER_5
    C64_KB_6, //	NUMBER_6
    C64_KB_7, //	NUMBER_7
    C64_KB_8, //	NUMBER_8
    C64_KB_9, //	NUMBER_9
    C64_KB_COL, //	COLON
    C64_KB_SCO, //	COLON_SEMI
    C64_KB_LSH | C64_KB_COMMA, //	LESS_THAN
    C64_KB_EQ, //	EQUAL_TO
    C64_KB_LSH | C64_KB_DOT, //	GREATER_THAN
    C64_KB_LSH | C64_KB_SLASH, //	QUESTION_MARK
    C64_KB_AT, //	SING_AT
    C64_KB_A, //	A_UPPER
    C64_KB_B, //	B_UPPER
    C64_KB_C, //	C_UPPER
    C64_KB_D, //	D_UPPER
    C64_KB_E, //	E_UPPER
    C64_KB_F, //	F_UPPER
    C64_KB_G, //	G_UPPER
    C64_KB_H, //	H_UPPER
    C64_KB_I, //	I_UPPER
    C64_KB_J, //	J_UPPER
    C64_KB_K, //	K_UPPER
    C64_KB_L, //	L_UPPER
    C64_KB_M, //	M_UPPER
    C64_KB_N, //	N_UPPER
    C64_KB_O, //	O_UPPER
    C64_KB_P, //	P_UPPER
    C64_KB_Q, //	Q_UPPER
    C64_KB_R, //	R_UPPER
    C64_KB_S, //	S_UPPER
    C64_KB_T, //	T_UPPER
    C64_KB_U, //	U_UPPER
    C64_KB_V, //	V_UPPER
    C64_KB_W, //	W_UPPER
    C64_KB_X, //	X_UPPER
    C64_KB_Y, //	Y_UPPER
    C64_KB_Z, //	Z_UPPER
    C64_KB_LSH | C64_KB_COL, //	BRACKET_OPEN
    0, //	SLASH_BACKWARD
    C64_KB_LSH | C64_KB_SCO, //	BRACKET_CLOSE
    0, //	CARET
    0, //	UNDERSCORE
    0, //	GRAVE_ACCENT
    C64_KB_A, //	A_LOWER
    C64_KB_B, //	B_LOWER
    C64_KB_C, //	C_LOWER
    C64_KB_D, //	D_LOWER
    C64_KB_E, //	E_LOWER
    C64_KB_F, //	F_LOWER
    C64_KB_G, //	G_LOWER
    C64_KB_H, //	H_LOWER
    C64_KB_I, //	I_LOWER
    C64_KB_J, //	J_LOWER
    C64_KB_K, //	K_LOWER
    C64_KB_L, //	L_LOWER
    C64_KB_M, //	M_LOWER
    C64_KB_N, //	N_LOWER
    C64_KB_O, //	O_LOWER
    C64_KB_P, //	P_LOWER
    C64_KB_Q, //	Q_LOWER
    C64_KB_R, //	R_LOWER
    C64_KB_S, //	S_LOWER
    C64_KB_T, //	T_LOWER
    C64_KB_U, //	U_LOWER
    C64_KB_V, //	V_LOWER
    C64_KB_W, //	W_LOWER
    C64_KB_X, //	X_LOWER
    C64_KB_Y, //	Y_LOWER
    C64_KB_Z, //	Z_LOWER
    0, //	BRACKET_CURLY_OPEN
    0, //	VERTICAL_BAR
    0, //	BRACKET_CURLY_CLOSE
    0, //	TILDE
    C64_KB_DEL, //	DEL
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};