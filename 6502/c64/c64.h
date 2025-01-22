/*******************************************************************************
* Commodore 64 emulator main header file.
*******************************************************************************/

#define C64BASIC_START	0xA000U
#define C64BASIC_END	0xBFFFU

#define C64CHAR_START	0xD000U
#define C64CHAR_END		0xDFFFU

#define C64KERNAL_START	0xE000U
#define C64KERNAL_END	0xFFFFU

void c64_init(Processor_65xx * pProcessor);
void c64_run(Processor_65xx * pProcessor);
