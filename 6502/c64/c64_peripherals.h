/*******************************************************************************
* Commodore 64 peripheral header file.
*******************************************************************************/

#include <65xx.h>


U8 c64_checkPinIRQ(void);
U8 c64_ioRead(Processor_65xx * pProcessor, mos65xx_addr address);
void c64_ioWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);

/*******************************************************************************
* VIC-II chip definitions
*******************************************************************************/
#define VIC_II_START_ADDR 0xD000
#define VIC_II ((VicII_t *)&c64_IO[VIC_II_START_ADDR])

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
} VicII_t;

void vicII_init(void);
void vicII_tick(U64 tickAdvance);

/*******************************************************************************
* SID chip definitions
*******************************************************************************/
#define SID_START_ADDR 0xD400
#define SID ((Sid_t *)&c64_IO[SID_START_ADDR])

typedef struct {
	U8 D400_voice1FreqLSB; // Write-only
	U8 D401_voice1FreqMSB; // Write-only
	U8 D402_voice1PulseWidthLSB; // Write-only
	U8 D403_voice1PulseWidthMSB; // Write-only
	/*
	 * 0: Voice off, Release cycle / Voice on Attack-Decay-Sustain cycle
	 * 1: Synchronization enabled
	 * 2: Ring modulation enabled
	 * 3: Disable voice, reset noise generator
	 * 4: Triangle waveform enabled
	 * 5: Saw waveform enabled
	 * 6: Rectangle waveform enabled
	 * 7: Noise enabled
	 */
	U8 D404_voice1CtrlReg;
	U8 D405_voice1AttackDecayLen;
	U8 D406_voice1SustainReleaseLen;
	U8 D407_voice2FreqLSB; // Write-only
	U8 D408_voice2FreqMSB; // Write-only
	U8 D409_voice2PulseWidthLSB; // Write-only
	U8 D40A_voice2PulseWidthMSB; // Write-only
	U8 D40B_voice2CtrlReg;
	U8 D40C_voice2AttackDecayLen;
	U8 D40D_voice2SustainReleaseLen;
	U8 D40E_voice3FreqLSB; // Write-only
	U8 D40F_voice3FreqMSB; // Write-only
	U8 D410_voice3PulseWidthLSB; // Write-only
	U8 D411_voice3PulseWidthMSB; // Write-only
	U8 D412_voice3CtrlReg;
	U8 D413_voice3AttackDecayLen;
	U8 D414_voice3SustainReleaseLen;
	U8 D415_filterCutOffFreq1; // Bits 0-2
	U8 D416_filterCutOffFreq2; // Bits 3-10
	U8 D417_filterCtrl;
	U8 D418_volumeFilterModes;
	U8 D419_selPaddleX;
	U8 D41A_selPaddleY;
	U8 D41B_voice3WaveformOut;
	U8 D41C_voice3ADSROut;
	U8 D41D_unused;
	U8 D41E_unused;
	U8 D41F_unused;
} Sid_t;

/*******************************************************************************
* Color RAM definitions
*******************************************************************************/
#define COLOR_RAM_START_ADDR 0xD800

/*******************************************************************************
* CIA chip definitions
*******************************************************************************/
#define CIA1_START_ADDR 0xDC00
#define CIA1 ((Cia1_t *)&c64_IO[CIA1_START_ADDR])

#define CIA2_START_ADDR 0xDD00
#define CIA2 ((Cia2_t *)&c64_IO[CIA2_START_ADDR])

#define CIA_IRQ_EN_MASK (	\
	CIA_IRQ_CS_TIM_A    	\
|	CIA_IRQ_CS_TIM_B    	\
|	CIA_IRQ_CS_ALARM    	\
|	CIA_IRQ_CS_SERIAL   	\
|	CIA_IRQ_CS_FLAG			\
)

enum {
	CIA_IRQ_CS_TIM_A	= (1 << 0),
	CIA_IRQ_CS_TIM_B	= (1 << 1),
	CIA_IRQ_CS_ALARM	= (1 << 2),
	CIA_IRQ_CS_SERIAL	= (1 << 3),
	CIA_IRQ_CS_FLAG		= (1 << 4),
	CIA_IRQ_CS_FILL		= (1 << 7),
};

enum {
	TIM_START		= (1 << 0),
	TIM_PORTB_UFLOW	= (1 << 1),
	TIM_PORTB_FN	= (1 << 2),
	TIM_STOP_UFLOW	= (1 << 3),
	TIM_RELOAD		= (1 << 4),
	TIM_CNT_SYSCYC	= (1 << 5),
	TIM_SFT_DIR_W	= (1 << 6),
	TIM_TOD_50HZ	= (1 << 7),
};

typedef struct {
	U8 DC00_portAKbdColJStick2;
	U8 DC01_portBKbdRowJStick1;
	U8 DC02_portADataDir;
	U8 DC03_portBDataDir;
	U8 DC04_timerAValueLSB; // RW
	U8 DC05_timerAValueMSB; // RW
	U8 DC06_timerBValueLSB; // RW
	U8 DC07_timerBValueMSB; // RW
	U8 DC08_timeOfDayTenths; // BDC
	U8 DC09_timeOfDaySecs; // BDC
	U8 DC0A_timeOfDayMins; // BDC
	U8 DC0B_timeOfDayHours; // BDC
	U8 DC0C_serialShiftReg;
	U8 DC0D_intCtrlStatusReg;
	U8 DC0E_timerACtrl;
	U8 DC0F_timerBCtrl;
} Cia1_t;

typedef struct {
	U8 DD00_portASerial;
	U8 DD01_portBRS232;
	U8 DD02_portADataDir;
	U8 DD03_portBDataDir;
	U8 DD04_timerAValueLSB; // RW
	U8 DD05_timerAValueMSB; // RW
	U8 DD06_timerBValueLSB; // RW
	U8 DD07_timerBValueMSB; // RW
	U8 DD08_timeOfDayTenths; // BDC
	U8 DD09_timeOfDaySecs; // BDC
	U8 DD0A_timeOfDayMins; // BDC
	U8 DD0B_timeOfDayHours; // BDC
	U8 DD0C_serialShiftReg;
	U8 DD0D_intCtrlStatusReg;
	U8 DD0E_timerACtrl;
	U8 DD0F_timerBCtrl;
} Cia2_t;

void CIA_tick(U64 tickAdvance);
