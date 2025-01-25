/*******************************************************************************
* assembler.c
*
* 65xx assembler.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "65xx_types.h"

U8 fileGetLine(FILE * file, char * dstString);
U8 rowExploder(char * pRow, char *** pDstElements);
char * skipWhitespace(char * pStr);
char * skipElement(char * pStr);
void handleElements(char ** ppElementList, U8 elementCount);
U8 checkLabel(char * pElement);

extern opCode_st * mos65xx_opCodes;

/*******************************************************************************
* Assembles given file in to the memory.
*******************************************************************************/
void mos65xx_assemble(char * asmFilePath, U8 * memory) {

	FILE * asmFile = fopen(asmFilePath, "r");
	char * nextLine = calloc(257, sizeof(U8));
	U8 errCode;
	U8 skip = 0;

	while (errCode = fileGetLine(asmFile, nextLine)) {
		char *** pppRowContents = (char ***)calloc(sizeof(char **), 1);
		U8 elementCount;
		
		elementCount = rowExploder(nextLine, pppRowContents);

		if (*pppRowContents != NULL) {

			if (strcmp((*pppRowContents)[0], "COMMENT") == 0) {
				skip = 1;
			} else if (skip == 1 && strcmp((*pppRowContents)[0], "END_COMMENT") == 0) {
				skip = 2;
			}

			if (skip == 0) {
				handleElements(*pppRowContents, elementCount);
			} else if (skip == 2) {
				skip = 0;
			}
				
		}

		free(pppRowContents);
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
U8 rowExploder(char * pRow, char *** pDstElements) {

	U8 notFinished = 1;
	U8 elementCount = 0;

	while (notFinished) {

		pRow = skipWhitespace(pRow);

		if (*pRow != '\0') {


			elementCount++;
			*pDstElements = (char **)realloc(*pDstElements, sizeof(char *) * elementCount);

			(*pDstElements)[elementCount - 1] = pRow;

			pRow = skipElement(pRow);

		} else {
			notFinished = 0;
		}
	}
	return elementCount;
}

/*******************************************************************************
* Convert whitespaces to null and move string pointer to next element.
*******************************************************************************/
char * skipWhitespace(char * pStr) {

	while ((*pStr == ' ' || *pStr == '\t') && *pStr != '\0') {
		*pStr++ = '\0';
	}

	return pStr;
}

/*******************************************************************************
* Reads next element from the string.
*******************************************************************************/
char * skipElement(char * pStr) {
	U8 buffIndex = 0;
	char * dstStr = calloc(256, sizeof(char));

	while (*pStr != ' ' && *pStr != '\t' && *pStr != '\0') {
		pStr++;
	}

	return pStr;
}

void handleElements(char ** ppElementList, U8 elementCount) {

	if (!checkLabel(*ppElementList)) {
		printf("\t\t");
	}

	for (int elementIndex = 0; elementIndex < elementCount; elementIndex++) {

		printf("%s\t\t", ppElementList[elementIndex]);

	}

	printf("\n");
}

U8 checkLabel(char * pElement) {

	while(*(++pElement) != '\0');

	return *(pElement - 1) == ':' ? 1 : 0;
}
