/*******************************************************************************
* 6502.c
*
* 6502 processor emulator.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "6502.h"
#include "6502_opc.h"
#include "6502_addrm.h"

opCode_st mos6502__opCodes[] = {
/*  0,0x00 */	{mos6502_BRK,	"BRK",	IMP,	2},	// impl
/*  1,0x01 */	{mos6502_ORA,	"ORA",	INX,	2},	// X, ind
/*  2,0x02 */	{mos6502_JAM,	"JAM",	NON,	1},	// none
/*  3,0x03 */	{mos6502_SLO,	"SLO",	INX,	2},	// X, ind
/*  4,0x04 */	{mos6502_NOP,	"NOP",	ZPG,	2},	// zpg
/*  5,0x05 */	{mos6502_ORA,	"ORA",	ZPG,	2},	// zpg
/*  6,0x06 */	{mos6502_ASL,	"ASL",	ZPG,	2},	// zpg
/*  7,0x07 */	{mos6502_SLO,	"SLO",	ZPG,	2},	// zpg
/*  8,0x08 */	{mos6502_PHP,	"PHP",	IMP,	1},	// impl
/*  9,0x09 */	{mos6502_ORA,	"ORA",	IMM,	2},	// #
/* 10,0x0A */	{mos6502_ASL,	"ASL",	ACC,	1},	// A
/* 11,0x0B */	{mos6502_ANC,	"ANC",	IMM,	2},	// #
/* 12,0x0C */	{mos6502_NOP,	"NOP",	ABS,	3},	// abs
/* 13,0x0D */	{mos6502_ORA,	"ORA",	ABS,	3},	// abs
/* 14,0x0E */	{mos6502_ASL,	"ASL",	ABS,	3},	// abs
/* 15,0x0F */	{mos6502_SLO,	"SLO",	ABS,	3},	// abs
/* 16,0x10 */	{mos6502_BPL,	"BPL",	REL,	2},	// rel
/* 17,0x11 */	{mos6502_ORA,	"ORA",	INY,	2},	// ind, Y
/* 18,0x12 */	{mos6502_JAM,	"JAM",	NON,	1},	// none
/* 19,0x13 */	{mos6502_SLO,	"SLO",	INY,	2},	// ind, Y
/* 20,0x14 */	{mos6502_NOP,	"NOP",	ZPX,	2},	// zpg, X
/* 21,0x15 */	{mos6502_ORA,	"ORA",	ZPX,	2},	// zpg, X
/* 22,0x16 */	{mos6502_ASL,	"ASL",	ZPX,	2},	// zpg, X
/* 23,0x17 */	{mos6502_SLO,	"SLO",	ZPX,	2},	// zpg, X
/* 24,0x18 */	{mos6502_CLC,	"CLC",	IMP,	1},	// impl
/* 25,0x19 */	{mos6502_ORA,	"ORA",	ABY,	3},	// abs, Y
/* 26,0x1A */	{mos6502_NOP,	"NOP",	IMP,	1},	// impl
/* 27,0x1B */	{mos6502_SLO,	"SLO",	ABY,	3},	// abs, Y
/* 28,0x1C */	{mos6502_NOP,	"NOP",	ABX,	3},	// abs, X
/* 29,0x1D */	{mos6502_ORA,	"ORA",	ABX,	3},	// abs, X
/* 30,0x1E */	{mos6502_ASL,	"ASL",	ABX,	3},	// abs, X
/* 31,0x1F */	{mos6502_SLO,	"SLO",	ABX,	3},	// abs, X
/* 32,0x20 */	{mos6502_JSR,	"JSR",	ABS,	3},	// abs
/* 33,0x21 */	{mos6502_AND,	"AND",	INX,	2},	// X, ind
/* 34,0x22 */	{mos6502_JAM,	"JAM",	NON,	1},	// none
/* 35,0x23 */	{mos6502_RLA,	"RLA",	INX,	2},	// X, ind
/* 36,0x24 */	{mos6502_BIT,	"BIT",	ZPG,	2},	// zpg
/* 37,0x25 */	{mos6502_AND,	"AND",	ZPG,	2},	// zpg
/* 38,0x26 */	{mos6502_ROL,	"ROL",	ZPG,	2},	// zpg
/* 39,0x27 */	{mos6502_RLA,	"RLA",	ZPG,	2},	// zpg
/* 40,0x28 */	{mos6502_PLP,	"PLP",	IMP,	1},	// impl
/* 41,0x29 */	{mos6502_AND,	"AND",	IMM,	2},	// #
/* 42,0x2A */	{mos6502_ROL,	"ROL",	ACC,	1},	// A
/* 43,0x2B */	{mos6502_ANC,	"ANC",	IMM,	2},	// #
/* 44,0x2C */	{mos6502_BIT,	"BIT",	ABS,	3},	// abs
/* 45,0x2D */	{mos6502_AND,	"AND",	ABS,	3},	// abs
/* 46,0x2E */	{mos6502_ROL,	"ROL",	ABS,	3},	// abs
/* 47,0x2F */	{mos6502_RLA,	"RLA",	ABS,	3},	// abs
/* 48,0x30 */	{mos6502_BMI,	"BMI",	REL,	2},	// rel
/* 49,0x31 */	{mos6502_AND,	"AND",	INY,	2},	// ind, Y
/* 50,0x32 */	{mos6502_JAM,	"JAM",	NON,	1},	// none
/* 51,0x33 */	{mos6502_RLA,	"RLA",	INY,	2},	// ind, Y
/* 52,0x34 */	{mos6502_NOP,	"NOP",	ZPX,	2},	// zpg, X
/* 53,0x35 */	{mos6502_AND,	"AND",	ZPX,	2},	// zpg, X
/* 54,0x36 */	{mos6502_ROL,	"ROL",	ZPX,	2},	// zpg, X
/* 55,0x37 */	{mos6502_RLA,	"RLA",	ZPX,	2},	// zpg, X
/* 56,0x38 */	{mos6502_SEC,	"SEC",	IMP,	1},	// impl
/* 57,0x39 */	{mos6502_AND,	"AND",	ABY,	3},	// abs, Y
/* 58,0x3A */	{mos6502_NOP,	"NOP",	IMP,	1},	// impl
/* 59,0x3B */	{mos6502_RLA,	"RLA",	ABY,	3},	// abs, Y
/* 60,0x3C */	{mos6502_NOP,	"NOP",	ABX,	3},	// abs, X
/* 61,0x3D */	{mos6502_AND,	"AND",	ABX,	3},	// abs, X
/* 62,0x3E */	{mos6502_ROL,	"ROL",	ABX,	3},	// abs, X
/* 63,0x3F */	{mos6502_RLA,	"RLA",	ABX,	3},	// abs, X
/* 64,0x40 */	{mos6502_RTI,	"RTI",	IMP,	1},	// impl
/* 65,0x41 */	{mos6502_EOR,	"EOR",	INX,	2},	// X, ind
/* 66,0x42 */	{mos6502_JAM,	"JAM",	NON,	1},	// none
/* 67,0x43 */	{mos6502_SRE,	"SRE",	INX,	2},	// X, ind
/* 68,0x44 */	{mos6502_NOP,	"NOP",	ZPG,	2},	// zpg
/* 69,0x45 */	{mos6502_EOR,	"EOR",	ZPG,	2},	// zpg
/* 70,0x46 */	{mos6502_LSR,	"LSR",	ZPG,	2},	// zpg
/* 71,0x47 */	{mos6502_SRE,	"SRE",	ZPG,	2},	// zpg
/* 72,0x48 */	{mos6502_PHA,	"PHA",	IMP,	1},	// impl
/* 73,0x49 */	{mos6502_EOR,	"EOR",	IMM,	2},	// #
/* 74,0x4A */	{mos6502_LSR,	"LSR",	ACC,	1},	// A
/* 75,0x4B */	{mos6502_ALR,	"ALR",	IMM,	2},	// #
/* 76,0x4C */	{mos6502_JMP,	"JMP",	ABS,	3},	// abs
/* 77,0x4D */	{mos6502_EOR,	"EOR",	ABS,	3},	// abs
/* 78,0x4E */	{mos6502_LSR,	"LSR",	ABS,	3},	// abs
/* 79,0x4F */	{mos6502_SRE,	"SRE",	ABS,	3},	// abs
/* 80,0x50 */	{mos6502_BVC,	"BVC",	REL,	2},	// rel
/* 81,0x51 */	{mos6502_EOR,	"EOR",	INY,	2},	// ind, Y
/* 82,0x52 */	{mos6502_JAM,	"JAM",	NON,	1},	// none
/* 83,0x53 */	{mos6502_SRE,	"SRE",	INY,	2},	// ind, Y
/* 84,0x54 */	{mos6502_NOP,	"NOP",	ZPX,	2},	// zpg, X
/* 85,0x55 */	{mos6502_EOR,	"EOR",	ZPX,	2},	// zpg, X
/* 86,0x56 */	{mos6502_LSR,	"LSR",	ZPX,	2},	// zpg, X
/* 87,0x57 */	{mos6502_SRE,	"SRE",	ZPX,	2},	// zpg, X
/* 88,0x58 */	{mos6502_CLI,	"CLI",	IMP,	1},	// impl
/* 89,0x59 */	{mos6502_EOR,	"EOR",	ABY,	3},	// abs, Y
/* 90,0x5A */	{mos6502_NOP,	"NOP",	IMP,	1},	// impl
/* 91,0x5B */	{mos6502_SRE,	"SRE",	ABY,	3},	// abs, Y
/* 92,0x5C */	{mos6502_NOP,	"NOP",	ABX,	3},	// abs, X
/* 93,0x5D */	{mos6502_EOR,	"EOR",	ABX,	3},	// abs, X
/* 94,0x5E */	{mos6502_LSR,	"LSR",	ABX,	3},	// abs, X
/* 95,0x5F */	{mos6502_SRE,	"SRE",	ABX,	3},	// abs, X
/* 96,0x60 */	{mos6502_RTS,	"RTS",	IMP,	1},	// impl
/* 97,0x61 */	{mos6502_ADC,	"ADC",	INX,	2},	// X, ind
/* 98,0x62 */	{mos6502_JAM,	"JAM",	NON,	1},	// none
/* 99,0x63 */	{mos6502_RRA,	"RRA",	INX,	2},	// X, ind
/* 100,0x64 */	{mos6502_NOP,	"NOP",	ZPG,	2},	// zpg
/* 101,0x65 */	{mos6502_ADC,	"ADC",	ZPG,	2},	// zpg
/* 102,0x66 */	{mos6502_ROR,	"ROR",	ZPG,	2},	// zpg
/* 103,0x67 */	{mos6502_RRA,	"RRA",	ZPG,	2},	// zpg
/* 104,0x68 */	{mos6502_PLA,	"PLA",	IMP,	1},	// impl
/* 105,0x69 */	{mos6502_ADC,	"ADC",	IMM,	2},	// #
/* 106,0x6A */	{mos6502_ROR,	"ROR",	ACC,	1},	// A
/* 107,0x6B */	{mos6502_ARR,	"ARR",	IMM,	2},	// #
/* 108,0x6C */	{mos6502_JMP,	"JMP",	IND,	3},	// ind
/* 109,0x6D */	{mos6502_ADC,	"ADC",	ABS,	3},	// abs
/* 110,0x6E */	{mos6502_ROR,	"ROR",	ABS,	3},	// abs
/* 111,0x6F */	{mos6502_RRA,	"RRA",	ABS,	3},	// abs
/* 112,0x70 */	{mos6502_BVS,	"BVS",	REL,	2},	// rel
/* 113,0x71 */	{mos6502_ADC,	"ADC",	INY,	2},	// ind, Y
/* 114,0x72 */	{mos6502_JAM,	"JAM",	NON,	1},	// none
/* 115,0x73 */	{mos6502_RRA,	"RRA",	INY,	2},	// ind, Y
/* 116,0x74 */	{mos6502_NOP,	"NOP",	ZPX,	2},	// zpg, X
/* 117,0x75 */	{mos6502_ADC,	"ADC",	ZPX,	2},	// zpg, X
/* 118,0x76 */	{mos6502_ROR,	"ROR",	ZPX,	2},	// zpg, X
/* 119,0x77 */	{mos6502_RRA,	"RRA",	ZPX,	2},	// zpg, X
/* 120,0x78 */	{mos6502_SEI,	"SEI",	IMP,	1},	// impl
/* 121,0x79 */	{mos6502_ADC,	"ADC",	ABY,	3},	// abs, Y
/* 122,0x7A */	{mos6502_NOP,	"NOP",	IMP,	1},	// impl
/* 123,0x7B */	{mos6502_RRA,	"RRA",	ABY,	3},	// abs, Y
/* 124,0x7C */	{mos6502_NOP,	"NOP",	ABX,	3},	// abs, X
/* 125,0x7D */	{mos6502_ADC,	"ADC",	ABX,	3},	// abs, X
/* 126,0x7E */	{mos6502_ROR,	"ROR",	ABX,	3},	// abs, X
/* 127,0x7F */	{mos6502_RRA,	"RRA",	ABX,	3},	// abs, X
/* 128,0x80 */	{mos6502_NOP,	"NOP",	IMM,	2},	// #
/* 129,0x81 */	{mos6502_STA,	"STA",	INX,	2},	// X, ind
/* 130,0x82 */	{mos6502_NOP,	"NOP",	IMM,	2},	// #
/* 131,0x83 */	{mos6502_SAX,	"SAX",	INX,	2},	// X, ind
/* 132,0x84 */	{mos6502_STY,	"STY",	ZPG,	2},	// zpg
/* 133,0x85 */	{mos6502_STA,	"STA",	ZPG,	2},	// zpg
/* 134,0x86 */	{mos6502_STX,	"STX",	ZPG,	2},	// zpg
/* 135,0x87 */	{mos6502_SAX,	"SAX",	ZPG,	2},	// zpg
/* 136,0x88 */	{mos6502_DEY,	"DEY",	IMP,	1},	// impl
/* 137,0x89 */	{mos6502_NOP,	"NOP",	IMM,	2},	// #
/* 138,0x8A */	{mos6502_TXA,	"TXA",	IMP,	1},	// impl
/* 139,0x8B */	{mos6502_ANE,	"ANE",	IMM,	2},	// #
/* 140,0x8C */	{mos6502_STY,	"STY",	ABS,	3},	// abs
/* 141,0x8D */	{mos6502_STA,	"STA",	ABS,	3},	// abs
/* 142,0x8E */	{mos6502_STX,	"STX",	ABS,	3},	// abs
/* 143,0x8F */	{mos6502_SAX,	"SAX",	ABS,	3},	// abs
/* 144,0x90 */	{mos6502_BCC,	"BCC",	REL,	2},	// rel
/* 145,0x91 */	{mos6502_STA,	"STA",	INY,	2},	// ind, Y
/* 146,0x92 */	{mos6502_JAM,	"JAM",	NON,	1},	// none
/* 147,0x93 */	{mos6502_SHA,	"SHA",	INY,	2},	// ind, Y
/* 148,0x94 */	{mos6502_STY,	"STY",	ZPX,	2},	// zpg, X
/* 149,0x95 */	{mos6502_STA,	"STA",	ZPX,	2},	// zpg, X
/* 150,0x96 */	{mos6502_STX,	"STX",	ZPY,	2},	// zpg, Y
/* 151,0x97 */	{mos6502_SAX,	"SAX",	ZPY,	2},	// zpg, Y
/* 152,0x98 */	{mos6502_TYA,	"TYA",	IMP,	1},	// impl
/* 153,0x99 */	{mos6502_STA,	"STA",	ABY,	3},	// abs, Y
/* 154,0x9A */	{mos6502_TXS,	"TXS",	IMP,	1},	// impl
/* 155,0x9B */	{mos6502_TAS,	"TAS",	ABY,	3},	// abs, Y
/* 156,0x9C */	{mos6502_SHY,	"SHY",	ABX,	3},	// abs, X
/* 157,0x9D */	{mos6502_STA,	"STA",	ABX,	3},	// abs, X
/* 158,0x9E */	{mos6502_SHX,	"SHX",	ABY,	3},	// abs, Y
/* 159,0x9F */	{mos6502_SHA,	"SHA",	ABY,	3},	// abs, Y
/* 160,0xA0 */	{mos6502_LDY,	"LDY",	IMM,	2},	// #
/* 161,0xA1 */	{mos6502_LDA,	"LDA",	INX,	2},	// X, ind
/* 162,0xA2 */	{mos6502_LDX,	"LDX",	IMM,	2},	// #
/* 163,0xA3 */	{mos6502_LAX,	"LAX",	INX,	2},	// X, ind
/* 164,0xA4 */	{mos6502_LDY,	"LDY",	ZPG,	2},	// zpg
/* 165,0xA5 */	{mos6502_LDA,	"LDA",	ZPG,	2},	// zpg
/* 166,0xA6 */	{mos6502_LDX,	"LDX",	ZPG,	2},	// zpg
/* 167,0xA7 */	{mos6502_LAX,	"LAX",	ZPG,	2},	// zpg
/* 168,0xA8 */	{mos6502_TAY,	"TAY",	IMP,	1},	// impl
/* 169,0xA9 */	{mos6502_LDA,	"LDA",	IMM,	2},	// #
/* 170,0xAA */	{mos6502_TAX,	"TAX",	IMP,	1},	// impl
/* 171,0xAB */	{mos6502_LXA,	"LXA",	IMM,	2},	// #
/* 172,0xAC */	{mos6502_LDY,	"LDY",	ABS,	3},	// abs
/* 173,0xAD */	{mos6502_LDA,	"LDA",	ABS,	3},	// abs
/* 174,0xAE */	{mos6502_LDX,	"LDX",	ABS,	3},	// abs
/* 175,0xAF */	{mos6502_LAX,	"LAX",	ABS,	3},	// abs
/* 176,0xB0 */	{mos6502_BCS,	"BCS",	REL,	2},	// rel
/* 177,0xB1 */	{mos6502_LDA,	"LDA",	INY,	2},	// ind, Y
/* 178,0xB2 */	{mos6502_JAM,	"JAM",	NON,	1},	// none
/* 179,0xB3 */	{mos6502_LAX,	"LAX",	INY,	2},	// ind, Y
/* 180,0xB4 */	{mos6502_LDY,	"LDY",	ZPG,	2},	// zpg
/* 181,0xB5 */	{mos6502_LDA,	"LDA",	ZPG,	2},	// zpg
/* 182,0xB6 */	{mos6502_LDX,	"LDX",	ZPG,	2},	// zpg
/* 183,0xB7 */	{mos6502_LAX,	"LAX",	ZPG,	2},	// zpg
/* 184,0xB8 */	{mos6502_CLV,	"CLV",	IMP,	1},	// impl
/* 185,0xB9 */	{mos6502_LDA,	"LDA",	ABY,	3},	// abs, Y
/* 186,0xBA */	{mos6502_TSX,	"TSX",	IMP,	1},	// impl
/* 187,0xBB */	{mos6502_LAS,	"LAS",	ABY,	3},	// abs, Y
/* 188,0xBC */	{mos6502_LDY,	"LDY",	ABS,	3},	// abs
/* 189,0xBD */	{mos6502_LDA,	"LDA",	ABX,	3},	// abs, X
/* 190,0xBE */	{mos6502_LDX,	"LDX",	ABY,	3},	// abs, Y
/* 191,0xBF */	{mos6502_LAX,	"LAX",	ABY,	3},	// abs, Y
/* 192,0xC0 */	{mos6502_CPY,	"CPY",	IMM,	2},	// #
/* 193,0xC1 */	{mos6502_CMP,	"CMP",	INX,	2},	// X, ind
/* 194,0xC2 */	{mos6502_NOP,	"NOP",	IMM,	2},	// #
/* 195,0xC3 */	{mos6502_DCP,	"DCP",	INX,	2},	// X, ind
/* 196,0xC4 */	{mos6502_CPY,	"CPY",	ZPG,	2},	// zpg
/* 197,0xC5 */	{mos6502_CMP,	"CMP",	ZPG,	2},	// zpg
/* 198,0xC6 */	{mos6502_DEC,	"DEC",	ZPG,	2},	// zpg
/* 199,0xC7 */	{mos6502_DCP,	"DCP",	ZPG,	2},	// zpg
/* 200,0xC8 */	{mos6502_INY,	"INY",	IMP,	1},	// impl
/* 201,0xC9 */	{mos6502_CMP,	"CMP",	IMM,	2},	// #
/* 202,0xCA */	{mos6502_DEX,	"DEX",	IMP,	1},	// impl
/* 203,0xCB */	{mos6502_SBX,	"SBX",	IMM,	2},	// #
/* 204,0xCC */	{mos6502_CPY,	"CPY",	ABS,	3},	// abs
/* 205,0xCD */	{mos6502_CMP,	"CMP",	ABS,	3},	// abs
/* 206,0xCE */	{mos6502_DEC,	"DEC",	ABS,	3},	// abs
/* 207,0xCF */	{mos6502_DCP,	"DCP",	ABS,	3},	// abs
/* 208,0xD0 */	{mos6502_BNE,	"BNE",	REL,	2},	// rel
/* 209,0xD1 */	{mos6502_CMP,	"CMP",	INY,	2},	// ind, Y
/* 210,0xD2 */	{mos6502_JAM,	"JAM",	NON,	1},	// none
/* 211,0xD3 */	{mos6502_DCP,	"DCP",	INY,	2},	// ind, Y
/* 212,0xD4 */	{mos6502_NOP,	"NOP",	ZPX,	2},	// zpg, X
/* 213,0xD5 */	{mos6502_CMP,	"CMP",	ZPX,	2},	// zpg, X
/* 214,0xD6 */	{mos6502_DEC,	"DEC",	ZPX,	2},	// zpg, X
/* 215,0xD7 */	{mos6502_DCP,	"DCP",	ZPX,	2},	// zpg, X
/* 216,0xD8 */	{mos6502_CLD,	"CLD",	IMP,	1},	// impl
/* 217,0xD9 */	{mos6502_CMP,	"CMP",	ABY,	3},	// abs, Y
/* 218,0xDA */	{mos6502_NOP,	"NOP",	IMP,	1},	// impl
/* 219,0xDB */	{mos6502_DCP,	"DCP",	ABY,	3},	// abs, Y
/* 220,0xDC */	{mos6502_NOP,	"NOP",	ABX,	3},	// abs, X
/* 221,0xDD */	{mos6502_CMP,	"CMP",	ABX,	3},	// abs, X
/* 222,0xDE */	{mos6502_DEC,	"DEC",	ABX,	3},	// abs, X
/* 223,0xDF */	{mos6502_DCP,	"DCP",	ABX,	3},	// abs, X
/* 224,0xE0 */	{mos6502_CPX,	"CPX",	IMM,	2},	// #
/* 225,0xE1 */	{mos6502_SBC,	"SBC",	INX,	2},	// X, ind
/* 226,0xE2 */	{mos6502_NOP,	"NOP",	IMM,	2},	// #
/* 227,0xE3 */	{mos6502_ISC,	"ISC",	INX,	2},	// X, ind
/* 228,0xE4 */	{mos6502_CPX,	"CPX",	ZPG,	2},	// zpg
/* 229,0xE5 */	{mos6502_SBC,	"SBC",	ZPG,	2},	// zpg
/* 230,0xE6 */	{mos6502_INC,	"INC",	ZPG,	2},	// zpg
/* 231,0xE7 */	{mos6502_ISC,	"ISC",	ZPG,	2},	// zpg
/* 232,0xE8 */	{mos6502_INX,	"INX",	IMP,	1},	// impl
/* 233,0xE9 */	{mos6502_SBC,	"SBC",	IMM,	2},	// #
/* 234,0xEA */	{mos6502_NOP,	"NOP",	IMP,	1},	// impl
/* 235,0xEB */	{mos6502_USBC,	"USBC",	IMM,	2},	// #
/* 236,0xEC */	{mos6502_CPX,	"CPX",	ABS,	3},	// abs
/* 237,0xED */	{mos6502_SBC,	"SBC",	ABS,	3},	// abs
/* 238,0xEE */	{mos6502_INC,	"INC",	ABS,	3},	// abs
/* 239,0xEF */	{mos6502_ISC,	"ISC",	ABS,	3},	// abs
/* 240,0xF0 */	{mos6502_BEQ,	"BEQ",	REL,	2},	// rel
/* 241,0xF1 */	{mos6502_SBC,	"SBC",	INY,	2},	// ind, Y
/* 242,0xF2 */	{mos6502_JAM,	"JAM",	NON,	1},	// none
/* 243,0xF3 */	{mos6502_ISC,	"ISC",	INY,	2},	// ind, Y
/* 244,0xF4 */	{mos6502_NOP,	"NOP",	ZPX,	2},	// zpg, X
/* 245,0xF5 */	{mos6502_SBC,	"SBC",	ZPX,	2},	// zpg, X
/* 246,0xF6 */	{mos6502_INC,	"INC",	ZPX,	2},	// zpg, X
/* 247,0xF7 */	{mos6502_ISC,	"ISC",	ZPX,	2},	// zpg, X
/* 248,0xF8 */	{mos6502_SED,	"SED",	IMP,	1},	// impl
/* 249,0xF9 */	{mos6502_SBC,	"SBC",	ABY,	3},	// abs, Y
/* 250,0xFA */	{mos6502_NOP,	"NOP",	IMP,	1},	// impl
/* 251,0xFB */	{mos6502_ISC,	"ISC",	ABY,	3},	// abs, Y
/* 252,0xFC */	{mos6502_NOP,	"NOP",	ABX,	3},	// abs, X
/* 253,0xFD */	{mos6502_SBC,	"SBC",	ABX,	3},	// abs, X
/* 254,0xFE */	{mos6502_INC,	"INC",	ABX,	3},	// abs, X
/* 255,0xFF */	{mos6502_ISC,	"ISC",	ABX,	3}	// abs, X
};

void printCurrOp(
	mos6502_processor_st *	pProcessor,
	mos6502_addrm			programCounter,
	U8						opCode,
	U8 *					operands,
	U8						cycles
);

/*******************************************************************************
* Initialize the processor.
*
* Arguments:
*	pProcessor - Pointer to a processor data structure.
*
* Returns: Nothing
*******************************************************************************/
void mos6502_init(mos6502_processor_st * pProcessor) {
	pProcessor->reg.PC = 0;
	pProcessor->reg.AC = 0;
	pProcessor->reg.X = 0;
	pProcessor->reg.Y = 0;
	pProcessor->reg.SR = SR_FLAG_UNUSED | SR_FLAG_B | SR_FLAG_I;
	pProcessor->reg.SP = 0xFF;
	DBG_PRINT(printf("                                  |  Registers  | Flags  |              \n"));
	DBG_PRINT(printf("\033[4m| ADDR | OP       | ASM           | AC XR YR SP | NVDIZC | Cycles (Tot)\033[m\n"));
}

/*******************************************************************************
* Handle next instruction
*
* Arguments:
*	pProcessor - Pointer to a processor data structure.
*
* Returns: Cycles elapsed
*******************************************************************************/
U8 mos6502_handleOp(mos6502_processor_st * pProcessor) {

	mos6502_addr oldPC = pProcessor->reg.PC;
	U8 operands[2];

	/* Read operands for debug printing. */
	operands[0] = pProcessor->memIf.read8(pProcessor->reg.PC + 1);
	operands[1] = pProcessor->memIf.read8(pProcessor->reg.PC + 2);

	/* Handle instruction. */
	U8 opCode = pProcessor->memIf.read8(addrm_imm(pProcessor));
	U8 retCode = mos6502__opCodes[opCode].handler(pProcessor, opCode);
	pProcessor->cycleCount += retCode;

	DBG_PRINT(printCurrOp(pProcessor, oldPC, opCode, operands, retCode));

	if (retCode == 0xFF) {
		printf("\n\nNon-implemented op-code %s (0x%02X)\n\n", mos6502__opCodes[opCode].mnemonic, opCode);
	} else if (retCode == 0xFE) {
		printf("\n\nProcessor %s by op-code 0x%02X\n\n", mos6502__opCodes[opCode].mnemonic, opCode);
	} else if (opCode == 0) {
		printf("\n\nBreak hit at 0x%04X.\n\n", oldPC);
		return 0xFF;
	}

	return retCode;
}

/*******************************************************************************
* Prints information of current operation.
*
* Arguments:
*	pProcessor	- Pointer to a processor data structure.
*	PC			- Program counter value for printed operation.
*	opCode		- Numeric value representing the op code.
*	operBytes	- Operand bytes for current operation.
*	cycles		- Count of cycles passed during current operation.
*
* Returns: Nothing
*******************************************************************************/
void printCurrOp(
	mos6502_processor_st *	pProcessor,
	mos6502_addrm			PC,
	U8						opCode,
	U8 *					operBytes,
	U8						cycles
) {
	char * asmOperand = calloc(9, sizeof(char));
	U8 operands[4];

	operands[0] = operBytes[0] >> 4;
	operands[1] = operBytes[0] & 0xF;
	operands[2] = operBytes[1] >> 4;
	operands[3] = operBytes[1] & 0xF;

	for (int operIdx = 0; operIdx < sizeof(operands); operIdx++) {
		if (operands[operIdx] < 10) {
			operands[operIdx] += '0';
		} else {
			operands[operIdx] += 'A' - 10;
		}
	}

	switch (mos6502__opCodes[opCode].addrMode)
	{
		case ACC:
			sprintf(asmOperand, " A      ");
			break;
		case ABS:
			sprintf(
				asmOperand, " $%c%c%c%c  ", operands[2], operands[3], operands[0], operands[1]);
			break;
		case ABX:
			sprintf(asmOperand, " $%c%c%c%c,X", operands[2], operands[3], operands[0], operands[1]);
			break;
		case ABY:
			sprintf(asmOperand, " $%c%c%c%c,Y", operands[2], operands[3], operands[0], operands[1]);
			break;
		case IMM:
			sprintf(asmOperand, "#$%c%c    ", operands[0], operands[1]);
			break;
		case IND:
			sprintf(asmOperand, "($%c%c%c%c) ", operands[2], operands[3], operands[0], operands[1]);
			break;
		case INX:
			sprintf(asmOperand, "($%c%c,X) ", operands[0], operands[1]);
			break;
		case INY:
			sprintf(asmOperand, "($%c%c),Y ", operands[0], operands[1]);
			break;
		case REL:
			sprintf(asmOperand, "  %-6d", (signed char)operBytes[0]);
			break;
		case ZPG:
			sprintf(asmOperand, " $%c%c    ", operands[0], operands[1]);
			break;
		case ZPX:
			sprintf(asmOperand, " $%c%c,X  ", operands[0], operands[1]);
			break;
		case ZPY:
			sprintf(asmOperand, " $%c%c,Y  ", operands[0], operands[1]);
			break;
		
		case NON:
		case IMP:
			sprintf(asmOperand, "        ");
			break;
	}

	printf("| %04X | %02X %c%c %c%c | %-4s %s | %02X %02X %02X %02X | %d%d%d%d%d%d | %3d (%llu)\n",
		PC,
		opCode,
		mos6502__opCodes[opCode].bytes > 1 ? operands[0] : ' ',
		mos6502__opCodes[opCode].bytes > 1 ? operands[1] : ' ',
		mos6502__opCodes[opCode].bytes > 2 ? operands[2] : ' ',
		mos6502__opCodes[opCode].bytes > 2 ? operands[3] : ' ',
		mos6502__opCodes[opCode].mnemonic,
		asmOperand,
		pProcessor->reg.AC,
		pProcessor->reg.X,
		pProcessor->reg.Y,
		pProcessor->reg.SP,
		pProcessor->reg.SR & SR_FLAG_N ? 1 : 0,
		pProcessor->reg.SR & SR_FLAG_V ? 1 : 0,
		pProcessor->reg.SR & SR_FLAG_D ? 1 : 0,
		pProcessor->reg.SR & SR_FLAG_I ? 1 : 0,
		pProcessor->reg.SR & SR_FLAG_Z ? 1 : 0,
		pProcessor->reg.SR & SR_FLAG_C ? 1 : 0,
		cycles,
		pProcessor->cycleCount
	);
}
