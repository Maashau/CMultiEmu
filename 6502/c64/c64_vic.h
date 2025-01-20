/*******************************************************************************
* VIC-II chip definitions
*******************************************************************************/
#define VIC_START_ADDR 0xD000
#define VIC ((c64_vict *)&c64_IO[VIC_START_ADDR])

typedef struct {
	U8 D000_sprite0x;
	U8 D001_sprite0y;
	U8 D002_sprite1x;
	U8 D003_sprite1y;
	U8 D004_sprite2x;
	U8 D005_sprite2y;
	U8 D006_sprite3x;
	U8 D007_sprite3y;
	U8 D008_sprite4x;
	U8 D009_sprite4y;
	U8 D00A_sprite5x;
	U8 D00B_sprite5y;
	U8 D00C_sprite6x;
	U8 D00D_sprite6y;
	U8 D00E_sprite7x;
	U8 D00F_sprite7y;
	U8 D010_sprite0_7xMSB; // Bit 8 for all sprites x coordinates
	 /*
	  * 0-2: Vertical raster scroll
	  * 3: Screen height 0 = 24 rows / 1 = 25 rows
	  * 4: 0 = Screen off
	  * 5: 0 = text mode, 1 = bitmap
	  * 6: Extended background on/off
	  * 7: 8th bit of: Read current raster line or Write raster line to interrupt at
	  * Default: 0x1B 0b00011011
	  */
	U8 D011_screenCtrl1;
	/*
	 * Bits 0-7 of: Read current raster line or
	 * Write raster line to interrupt at
	 */
	U8 D012_currRasterLine;
	U8 D013_lightPenX;
	U8 D014_lightPenY;
	U8 D015_spriteEnable; // Bit mask bit x = sprite x
	/*
	 * 0-2: Horizontal raster scroll
	 * 3: Screen width 0 = 38 columns, 1 = 40 columns
	 * 4: Multicolor mode on/off
	 * Default: 0xC8 0b11001000
	 */
	U8 D016_screenCtrl2;
	U8 D017_spriteDoubleHeight; // Bit x = sprite x 2 * height
	/*
	 * 1-3: In text mode pointer to character memory
	 * 4-7: Pointer to screen memory
	 */
	U8 D018_memSetupReg;
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
	U8 D019_intStatus;
	/*
	 * 0: raster interrupt interrupt enable
	 * 1: sprite-background interrupt enable
	 * 2: sprite-sprite interrupt enable
	 * 3: light pen interrupt enable
	 */
	U8 D01A_intCtrlReg;
	// Bit x = sprite x in front (0) or behind (1) screen contents
	U8 D01B_spritePrioReg;
	// Bit x = sprite x single color (0) or multi color (1)
	U8 D01C_spriteMultColorReg;
	U8 D01D_spriteDoubleWidth; // Bit x = sprite x 2 * width
	// Read: Bit x = sprite x collided, Write: enable further detection.
	U8 D01E_spriteSpriteCollReg;
	// Read: Bit x = sprite x collided, Write: enable further detection.
	U8 D01F_spriteBackgroundCollReg;
	U8 D020_borderColor; // Bits 0-3
	U8 D021_backgroundColor; // Bits 0-3
	U8 D022_extraBgColor1; // Bits 0-3
	U8 D023_extraBgColor2; // Bits 0-3
	U8 D024_extraBgColor3; // Bits 0-3
	U8 D025_spriteExtraColor1; // Bits 0-3
	U8 D026_spriteExtraColor2; // Bits 0-3
	U8 D027_sprite0Color; // Bits 0-3
	U8 D028_sprite1Color; // Bits 0-3
	U8 D029_sprite2Color; // Bits 0-3
	U8 D02A_sprite3Color; // Bits 0-3
	U8 D02B_sprite4Color; // Bits 0-3
	U8 D02C_sprite5Color; // Bits 0-3
	U8 D02D_sprite6Color; // Bits 0-3
	U8 D02E_sprite7Color; // Bits 0-3
	U8 D02F_unused;
	U8 D030_unused;
	U8 D031_unused;
	U8 D032_unused;
	U8 D033_unused;
	U8 D034_unused;
	U8 D035_unused;
	U8 D036_unused;
	U8 D037_unused;
	U8 D038_unused;
	U8 D039_unused;
	U8 D03A_unused;
	U8 D03B_unused;
	U8 D03C_unused;
	U8 D03D_unused;
	U8 D03E_unused;
	U8 D03F_unused;
} c64_vict;

/*******************************************************************************
* Functions
*******************************************************************************/
void c64_vicInit(void);
U8 c64_vicCheckIrq(void);
void c64_vicTick(U64 tickAdvance);
U8 c64_vicRead(Processor_65xx * pProcessor, mos65xx_addr address);
void c64_vicWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);
