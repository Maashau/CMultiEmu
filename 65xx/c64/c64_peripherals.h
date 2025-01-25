/*******************************************************************************
* Commodore 64 peripheral header file.
*******************************************************************************/

/*******************************************************************************
* Includes
*******************************************************************************/
#include <65xx.h>


/*******************************************************************************
* SID chip definitions
*******************************************************************************/
#define SID_START_ADDR 0xD400

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
* Functions
*******************************************************************************/
void c64_periphInit(Processor_65xx * pProcessor);
U8 c64_periphCheckIrq(void);
void c64_periphTick(Processor_65xx * pProcessor, U8 advance);
U8 c64_periphRead(Processor_65xx * pProcessor, mos65xx_addr address);
void c64_periphWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);
