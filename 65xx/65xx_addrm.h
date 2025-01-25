/*******************************************************************************
* MOS 65xx emulator addressing mode handler header.
*******************************************************************************/
#include "65xx_types.h"

/*******************************************************************************
* Function prototypes.
*******************************************************************************/
mos65xx_addr addrm_IMM(Processor_65xx * processor);
mos65xx_addr addrm_REL(Processor_65xx * processor);
mos65xx_addr addrm_ABS(Processor_65xx * processor);
mos65xx_addr addrm_ABX(Processor_65xx * processor);
mos65xx_addr addrm_ABY(Processor_65xx * processor);
mos65xx_addr addrm_ZPG(Processor_65xx * processor);
mos65xx_addr addrm_ZPX(Processor_65xx * processor);
mos65xx_addr addrm_ZPY(Processor_65xx * processor);
mos65xx_addr addrm_IND(Processor_65xx * processor);
mos65xx_addr addrm_INX(Processor_65xx * processor);
mos65xx_addr addrm_INY(Processor_65xx * processor);

U8 addrm_stackPop8(Processor_65xx * processor);
U16 addrm_stackPop16(Processor_65xx * processor);
void addrm_stackPush8(Processor_65xx * processor, U8 value);
void addrm_stackPush16(Processor_65xx * processor, U16 value);

U8 addrm_read8(Processor_65xx * pProcessor, mos65xx_addr address);
U16 addrm_read16(Processor_65xx * pProcessor, mos65xx_addr address);
void addrm_write8(Processor_65xx * pProcessor, mos65xx_addr address, U8 value);
void addrm_write16(Processor_65xx * pProcessor, mos65xx_addr address, U16 value);