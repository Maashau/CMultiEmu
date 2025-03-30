/*******************************************************************************
* VIC-II chip definitions
*******************************************************************************/
#define VIC_START_ADDR 0xD000

typedef enum {
	D000_sprite0x	= 0x00,
	D001_sprite0y	= 0x01,
	D002_sprite1x	= 0x02,
	D003_sprite1y	= 0x03,
	D004_sprite2x	= 0x04,
	D005_sprite2y	= 0x05,
	D006_sprite3x	= 0x06,
	D007_sprite3y	= 0x07,
	D008_sprite4x	= 0x08,
	D009_sprite4y	= 0x09,
D00A_sprite5x = 0x0A,
D00B_sprite5y = 0x0B,
D00C_sprite6x = 0x0C,
D00D_sprite6y = 0x0D,
D00E_sprite7x = 0x0E,
D00F_sprite7y = 0x0F,
D010_sprite0_7xMSB = 0x10, // Bit 8 for all sprites x coordinates
 /*
  * 0-2: Vertical raster scroll
  * 3: Screen height 0 = 24 rows / 1 = 25 rows
  * 4: 0 = Screen off
  * 5: 0 = text mode, 1 = bitmap
  * 6: Extended background on/off
  * 7: 8th bit of: Read current raster line or Write raster line to interrupt at
  * Default: 0x1B 0b00011011
  */
	D011_screenCtrl1 = 0x11,
	/*
	 * Bits 0-7 of: Read current raster line or
	 * Write raster line to interrupt at
	 */
	D012_currRasterLine = 0x12,
	D013_lightPenX = 0x13,
	D014_lightPenY = 0x14,
	D015_spriteEnable = 0x15, // Bit mask bit x = sprite x
	/*
	 * 0-2: Horizontal raster scroll
	 * 3: Screen width 0 = 38 columns, 1 = 40 columns
	 * 4: Multicolor mode on/off
	 * Default: 0xC8 0b11001000
	 */
	D016_screenCtrl2 = 0x16,
	D017_spriteDoubleHeight = 0x17, // Bit x = sprite x 2 * height
	/*
	 * 1-3: In text mode pointer to character memory
	 * 4-7: Pointer to screen memory
	 */
	D018_memSetupReg = 0x18,
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
	D019_intStatus = 0x19,
	/*
	 * 0: raster interrupt interrupt enable
	 * 1: sprite-background interrupt enable
	 * 2: sprite-sprite interrupt enable
	 * 3: light pen interrupt enable
	 */
	D01A_intCtrlReg = 0x1A,
	// Bit x = sprite x in front (0) or behind (1) screen contents
	D01B_spritePrioReg = 0x1B,
	// Bit x = sprite x single color (0) or multi color (1)
	D01C_spriteMultColorReg = 0x1C,
	D01D_spriteDoubleWidth = 0x1D, // Bit x = sprite x 2 * width
	// Read: Bit x = sprite x collided, Write: enable further detection.
	D01E_spriteSpriteCollReg = 0x1E,
	// Read: Bit x = sprite x collided, Write: enable further detection.
	D01F_spriteBackgroundCollReg = 0x1F,
	D020_borderColor = 0x20, // Bits 0-3
	D021_backgroundColor = 0x21, // Bits 0-3
	D022_extraBgColor1 = 0x22, // Bits 0-3
	D023_extraBgColor2 = 0x23, // Bits 0-3
	D024_extraBgColor3 = 0x24, // Bits 0-3
	D025_spriteExtraColor1 = 0x25, // Bits 0-3
	D026_spriteExtraColor2 = 0x26, // Bits 0-3
	D027_sprite0Color = 0x27, // Bits 0-3
	D028_sprite1Color = 0x28, // Bits 0-3
	D029_sprite2Color = 0x29, // Bits 0-3
	D02A_sprite3Color = 0x2A, // Bits 0-3
	D02B_sprite4Color = 0x2B, // Bits 0-3
	D02C_sprite5Color = 0x2C, // Bits 0-3
	D02D_sprite6Color = 0x2D, // Bits 0-3
	D02E_sprite7Color = 0x2E, // Bits 0-3
	D02F_unused = 0x2F,
	D030_unused = 0x30,
	D031_unused = 0x31,
	D032_unused = 0x32,
	D033_unused = 0x33,
	D034_unused = 0x34,
	D035_unused = 0x35,
	D036_unused = 0x36,
	D037_unused = 0x37,
	D038_unused = 0x38,
	D039_unused = 0x39,
	D03A_unused = 0x3A,
	D03B_unused = 0x3B,
	D03C_unused = 0x3C,
	D03D_unused = 0x3D,
	D03E_unused = 0x3E,
	D03F_unused = 0x3F,
	VICII_COUNT
} Vic_t;

/*******************************************************************************
* Functions
*******************************************************************************/
void c64_vicInit(C64_t * pC64);
void c64_vicTick(C64_t * pC64);
U8 c64_vicRead(Processor_65xx * pProcessor, mos65xx_addr address);
void c64_vicWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);
