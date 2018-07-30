#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hide.h"

void checkIfPPM(FILE *fp)
{
	/*Check if file is in PPM format*/
	char a, b;
	char arr[20];
	fgets(arr, sizeof(arr), fp);
	sscanf(arr, "%c %c", &a, &b);
	//printf("%c%c\n", a, b);
	if (a == 'P' && b == '6')
	{
		//printf("This file is a PPM file.\n");
	}
	else
	{
		printf("This file is not a PPM file.\n");
	}
}

void checkComments(FILE *fp)
{
	/*Check for comments*/
	int c;
	c = getc(fp);
    	while (c == '#') 
	{
    		while (getc(fp) != '\n');
         		c = getc(fp);
    	}

    	ungetc(c, fp);
}

int readSize(FILE *fp, struct imagesize *img)
{
	/*Check width and height information*/
	char line[20];
	fgets(line, sizeof(line), fp);
	sscanf(line, "%d %d", &img->width, &img->height);
	//printf("Width is %d and height is %d.\n", img->width, img->height);
	return 0;
}

int readMaxValue(FILE *fp, struct imagesize *img)
{
	/*Check maximum value*/
	char value[20];
	fgets(value, sizeof(value), fp);
	sscanf(value, "%d", &img->maxvalue);
	//printf("Max value is %d.\n", img->maxvalue);
	return 0;
}

int main(int argc, char *argv[])
{
	/*Check to see whether program has two parameters*/
	if (argc != 2)
	{
		printf("Program requires one command line parameter, the name of an input file in PPM 				format.\n");
		return 1;
	}
	
	/*Open the file in the parameter for reading and writing*/
	FILE *fp; 
	fp = fopen(argv[1], "rb");

	/*If a NULL pointer is returned then file failed to open*/
	if (!fp)
	{
		printf("Could not open file.\n");
		exit(1);
	}
	else
	{
		//printf("File opened.\n");
	}
	
	checkIfPPM(fp);
	checkComments(fp);
	struct imagesize img;
	int image; 
	image = readSize(fp, &img);
	image = readMaxValue(fp, &img);

	/*Grab byte from file and remember least significant bit, every 8 bits convert to ascii*/
	/*8 bits representing 0 means end of secret message*/
	int byte[1];
	int bits[8] = {2};
 	int LSB;
	char bin;
	int done = 0;
	printf("\nSecret message: ");
	while (done != 1)
	{
		int dec = 0;
		/*Read first 8 bytes*/
		for (int i = 0; i < 8; i++)
		{
			fread(byte, 1, 1, fp);
			LSB = *byte & 1;
			bits[i] = LSB;
		}
		/*Convert to decimal*/
		for (int i = 0; i < 8; i++)
		{
			dec = dec * 2 + bits[i];
		}
		//printf("Dec: %d\n", dec);
		printf("%c", dec);
		
		/*End when 00000000 is reached, marks end of message*/
		if (dec == 0)
		{
			done = 1;
		}	
	}
	printf("\n\n");
	
	
	return 1;
}
	
	
