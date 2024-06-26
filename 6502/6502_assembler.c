/*******************************************************************************
* assembler.c
*
* 6502 assembler.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "6502_types.h"

U8 fileGetLine(FILE * file, char * dstString);
U8 rowExploder(char * pRow, char ** pDstElements);
char * skipWhitespace(char * pStr);
char * getElement(char * pStr, char ** pDstList);

extern opCode_st * mos6502_opCodes;

/*******************************************************************************
* Assembles given file in to the memory.
*******************************************************************************/
void mos6502_assemble(char * asmFilePath, U8 * memory) {

	FILE * asmFile = fopen(asmFilePath, "r");
	char * nextLine = calloc(257, sizeof(U8));
	U8 errCode;

	while (errCode = fileGetLine(asmFile, nextLine)) {
		char ** rowContents = NULL;
		U8 elementCount;
		
		elementCount = rowExploder(nextLine, rowContents);

		for (elementCount; elementCount > 0; elementCount--) {
			free(rowContents[elementCount]);
		}
	}

	fclose(asmFile);
}

/*******************************************************************************
* Reads line from a file.
*******************************************************************************/
U8 fileGetLine(FILE * file, char * dstString) {

	U16 bufferIndex = 0;
	int readByte;
	U8 retVal = 1;
	U8 commentStart = 0;

	do {

		readByte = fgetc(file);

		if (readByte == ';' || readByte == '@') {
			commentStart  = 1;
		}
		
		if (!commentStart && readByte != '\r') {
			dstString[bufferIndex++] = (char)readByte;
		} else {
			dstString[bufferIndex++] = '\0';
		}

		if (bufferIndex > 0x100) {
			retVal = -1;
			break;
		}

	} while (readByte != EOF && readByte != '\n');

	if (readByte == EOF) {
		retVal = 0;
	}

	dstString[bufferIndex - 1] = '\0';

	return retVal;
}

/*******************************************************************************
* Turn row of a file to list of whitespace separated elements.
*******************************************************************************/
U8 rowExploder(char * pRow, char ** pDstElements) {

	U8 notFinished = 1;
	U8 elementCount = 0;

	while (notFinished) {

		pRow = skipWhitespace(pRow);

		if (*pRow != '\0') {

			char * tempElement = NULL;

			elementCount++;
			*pDstElements = (char *)realloc(*pDstElements, sizeof(char *) * elementCount);

			pRow = getElement(pRow, &tempElement);
			pDstElements[elementCount - 1] = tempElement;

		} else {
			notFinished = 0;
		}
	}
	return elementCount;
}

/*******************************************************************************
* Move string pointer past whitespace.
*******************************************************************************/
char * skipWhitespace(char * pStr) {

	while ((*pStr == ' ' || *pStr == '\t') && *pStr != '\0') {
		pStr++;
	}

	return pStr;
}

/*******************************************************************************
* Reads next element from the string.
*******************************************************************************/
char * getElement(char * pStr, char ** pDstList) {
	U8 buffIndex = 0;
	char * dstStr = calloc(256, sizeof(char));

	while (*pStr != ' ' && *pStr != '\n' && *pStr != '\0') {
		dstStr[buffIndex++] = *pStr++;
	}

	dstStr[buffIndex] = '\0';

	dstStr = (char *)realloc(dstStr, sizeof(char) * (buffIndex + 1));

	*pDstList = dstStr;

	return pStr;
}