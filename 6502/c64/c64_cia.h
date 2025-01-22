/*******************************************************************************
* CIA chip definitions
*******************************************************************************/
#define CIA1_START_ADDR 0xDC00
#define CIA2_START_ADDR 0xDD00

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

typedef enum {
	DC00_portAKbdColJStick2	= 0xDC00,
	DC01_portBKbdRowJStick1	= 0xDC01,
	DC02_portADataDir	= 0xDC02,
	DC03_portBDataDir	= 0xDC03,
	DC04_timerAValueLSB	= 0xDC04, // RW
	DC05_timerAValueMSB	= 0xDC05, // RW
	DC06_timerBValueLSB	= 0xDC06, // RW
	DC07_timerBValueMSB	= 0xDC07, // RW
	DC08_timeOfDayTenths	= 0xDC08, // BDC
	DC09_timeOfDaySecs	= 0xDC09, // BDC
	DC0A_timeOfDayMins	= 0xDC0A, // BDC
	DC0B_timeOfDayHours	= 0xDC0B, // BDC
	DC0C_serialShiftReg	= 0xDC0C,
	DC0D_intCtrlStatusReg	= 0xDC0D,
	DC0E_timerACtrl	= 0xDC0E,
	DC0F_timerBCtrl	= 0xDC0F,
	DD00_portASerial	= 0xDD00,
	DD01_portBRS232	= 0xDD01,
	DD02_portADataDir	= 0xDD02,
	DD03_portBDataDir	= 0xDD03,
	DD04_timerAValueLSB	= 0xDD04, // RW
	DD05_timerAValueMSB	= 0xDD05, // RW
	DD06_timerBValueLSB	= 0xDD06, // RW
	DD07_timerBValueMSB	= 0xDD07, // RW
	DD08_timeOfDayTenths	= 0xDD08, // BDC
	DD09_timeOfDaySecs	= 0xDD09, // BDC
	DD0A_timeOfDayMins	= 0xDD0A, // BDC
	DD0B_timeOfDayHours	= 0xDD0B, // BDC
	DD0C_serialShiftReg	= 0xDD0C,
	DD0D_intCtrlStatusReg	= 0xDD0D,
	DD0E_timerACtrl	= 0xDD0E,
	DD0F_timerBCtrl	= 0xDD0F
} Cia_t;

/*******************************************************************************
* Functions
*******************************************************************************/
void c64_ciaInit(Processor_65xx * pProcessor);
U8 c64_ciaCheckIrq(void);
void c64_ciaTick(Processor_65xx * pProcessor, U8 tickAdvance);
U8 c64_ciaRead(Processor_65xx * pProcessor, mos65xx_addr address);
void c64_ciaWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);
