/*******************************************************************************
* VIC-II chip definitions
*******************************************************************************/
#define VIC_START_ADDR 0xD000

typedef enum {
	D000_sprite0x	= 0xD000,
	D001_sprite0y	= 0xD001,
	D002_sprite1x	= 0xD002,
	D003_sprite1y	= 0xD003,
	D004_sprite2x	= 0xD004,
	D005_sprite2y	= 0xD005,
	D006_sprite3x	= 0xD006,
	D007_sprite3y	= 0xD007,
	D008_sprite4x	= 0xD008,
	D009_sprite4y	= 0xD009,
	D00A_sprite5x	= 0xD00A,
	D00B_sprite5y	= 0xD00B,
	D00C_sprite6x	= 0xD00C,
	D00D_sprite6y	= 0xD00D,
	D00E_sprite7x	= 0xD00E,
	D00F_sprite7y	= 0xD00F,
	D010_sprite0_7xMSB	= 0xD010, // Bit 8 for all sprites x coordinates
	 /*
	  * 0-2: Vertical raster scroll
	  * 3: Screen height 0 = 24 rows / 1 = 25 rows
	  * 4: 0 = Screen off
	  * 5: 0 = text mode, 1 = bitmap
	  * 6: Extended background on/off
	  * 7: 8th bit of: Read current raster line or Write raster line to interrupt at
	  * Default: 0x1B 0b00011011
	  */
	D011_screenCtrl1	= 0xD011,
	/*
	 * Bits 0-7 of: Read current raster line or
	 * Write raster line to interrupt at
	 */
	D012_currRasterLine	= 0xD012,
	D013_lightPenX	= 0xD013,
	D014_lightPenY	= 0xD014,
	D015_spriteEnable	= 0xD015, // Bit mask bit x = sprite x
	/*
	 * 0-2: Horizontal raster scroll
	 * 3: Screen width 0 = 38 columns, 1 = 40 columns
	 * 4: Multicolor mode on/off
	 * Default: 0xC8 0b11001000
	 */
	D016_screenCtrl2	= 0xD016,
	D017_spriteDoubleHeight	= 0xD017, // Bit x = sprite x 2 * height
	/*
	 * 1-3: In text mode pointer to character memory
	 * 4-7: Pointer to screen memory
	 */
	D018_memSetupReg	= 0xD018,
	/*
	 * Read:
	 * 0: Raster line interrupt
	 * 1: Sprite-background collision
	 * 3: Sprite-sprite collision
	 * 4: Light pen signal
	 * 7: An event that may generate an interrupt occured, not acknowledged yet
	 * Write:
	 * 0: Ack raster interrupt
	 * 1: Ack sprite-background collision interrupt
	 * 2: Ack sprite-sprite collision interrupt
	 * 3: Ack light pen interrupt
	 */
	D019_intStatus	= 0xD019,
	/*
	 * 0: raster interrupt interrupt enable
	 * 1: sprite-background interrupt enable
	 * 2: sprite-sprite interrupt enable
	 * 3: light pen interrupt enable
	 */
	D01A_intCtrlReg	= 0xD01A,
	// Bit x = sprite x in front (0) or behind (1) screen contents
	D01B_spritePrioReg	= 0xD01B,
	// Bit x = sprite x single color (0) or multi color (1)
	D01C_spriteMultColorReg	= 0xD01C,
	D01D_spriteDoubleWidth	= 0xD01D, // Bit x = sprite x 2 * width
	// Read: Bit x = sprite x collided, Write: enable further detection.
	D01E_spriteSpriteCollReg	= 0xD01E,
	// Read: Bit x = sprite x collided, Write: enable further detection.
	D01F_spriteBackgroundCollReg	= 0xD01F,
	D020_borderColor	= 0xD020, // Bits 0-3
	D021_backgroundColor	= 0xD021, // Bits 0-3
	D022_extraBgColor1	= 0xD022, // Bits 0-3
	D023_extraBgColor2	= 0xD023, // Bits 0-3
	D024_extraBgColor3	= 0xD024, // Bits 0-3
	D025_spriteExtraColor1	= 0xD025, // Bits 0-3
	D026_spriteExtraColor2	= 0xD026, // Bits 0-3
	D027_sprite0Color	= 0xD027, // Bits 0-3
	D028_sprite1Color	= 0xD028, // Bits 0-3
	D029_sprite2Color	= 0xD029, // Bits 0-3
	D02A_sprite3Color	= 0xD02A, // Bits 0-3
	D02B_sprite4Color	= 0xD02B, // Bits 0-3
	D02C_sprite5Color	= 0xD02C, // Bits 0-3
	D02D_sprite6Color	= 0xD02D, // Bits 0-3
	D02E_sprite7Color	= 0xD02E, // Bits 0-3
	D02F_unused	= 0xD02F,
	D030_unused	= 0xD030,
	D031_unused	= 0xD031,
	D032_unused	= 0xD032,
	D033_unused	= 0xD033,
	D034_unused	= 0xD034,
	D035_unused	= 0xD035,
	D036_unused	= 0xD036,
	D037_unused	= 0xD037,
	D038_unused	= 0xD038,
	D039_unused	= 0xD039,
	D03A_unused	= 0xD03A,
	D03B_unused	= 0xD03B,
	D03C_unused	= 0xD03C,
	D03D_unused	= 0xD03D,
	D03E_unused	= 0xD03E,
	D03F_unused	= 0xD03F
} Vic_t;

/*******************************************************************************
* Functions
*******************************************************************************/
void c64_vicInit(C64_t * pC64);
void c64_vicTick(C64_t * pC64);
U8 c64_vicRead(Processor_65xx * pProcessor, mos65xx_addr address);
void c64_vicWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);
