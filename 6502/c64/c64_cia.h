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

/*******************************************************************************
* Functions
*******************************************************************************/
void c64_ciaInit(void);
U8 c64_ciaCheckIrq(void);
void c64_ciaTick(U64 tickAdvance);
U8 c64_ciaRead(Processor_65xx * pProcessor, mos65xx_addr address);
void c64_ciaWrite(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);
