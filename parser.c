/* AUTHOR:				Naveen Kumar Molleti
 * EMAIL:				nerd.naveen@gmail.com
 * GIT Read-only URL:	git://github.com/NOLFXceptMe/pcmanfm2k10.git
 *
 * Parser module for .desktop files
 * To be integrated into PCManFM
 * Work as a part of GSoC 2010
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"

#define LINESZ	255
#define STRSZ	255
#define KEYSZ	STRSZ
#define VALSZ	STRSZ

#define KEYEXTRACTION 1
#define VALEXTRACTION 2

char inputLine[LINESZ] = {'\0'};

/* TODO: change type of fileName to FmPath */
int parse(char *fileName)
{
	int fd = open(fileName, O_RDONLY);

	char buf;
	size_t index = 0, lineCount = 0;
	memset(inputLine, '\0', LINESZ);

	while(read(fd, &buf, 1) != 0){
		//printf("%c", buf);
		inputLine[index++] = (char )buf;
		if(buf == '\n'){
			parseLine();
			//printf("%s", inputLine);
			index = 0;
			memset(inputLine, 0, LINESZ);
			++lineCount;
		}
	}

	return lineCount;
}

int parseLine()
{
	//printf("Parsing line %s", inputLine);
	size_t lineIndex = 0, keyIndex = 0, valIndex = 0;
	char key[KEYSZ], value[VALSZ];
	char buf;
	memset(key, 0, KEYSZ);
	memset(value, 0, VALSZ);
		
	size_t stateLineParser = KEYEXTRACTION;

	if((inputLine[0] == '#')||(inputLine[0] == '[')||(inputLine[0] == ' ')){
		//printf("Skipping line starting with %c\n", inputLine[0]);
		return -1;
	}

	while((buf = inputLine[lineIndex++])!= '\n'){
		if(buf == '=') stateLineParser = VALEXTRACTION;
		else if(stateLineParser == KEYEXTRACTION)
			key[keyIndex++] = buf;
		else if(stateLineParser == VALEXTRACTION)
			value[valIndex++] = buf;
	}

	printf("Extracted key-value pair %s = %s\n", key, value);

	return 0;
}

/*
int main()
{
	size_t lineCount = parse("test.desktop");
	printf("Line count = %d\n", lineCount);
	return 0;
}
*/
