#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PPM_header_read.h"

int checkIfPPM(FILE *fp)
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
		return 0;
	}
	else
	{
		printf("This file is not a PPM file.\n");
		return 1; 
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
		{
         		c = getc(fp);
		}
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
	if (img->width == 0 || img->height == 0)
	{
		//printf("NOT WORKING\n");
		return 1;
	}
	else
	{
		//printf("WORKING\n");
		return 0;
	}
}

int readMaxValue(FILE *fp, struct imagesize *img)
{
	/*Check maximum value*/
	char value[20];
	fgets(value, sizeof(value), fp);
	sscanf(value, "%d", &img->maxvalue);
	//printf("Max value is %d.\n", img->maxvalue);
	if (img->maxvalue == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void outputHeader(FILE *ofp, struct imagesize *img)
{
	fprintf(ofp, "P6\n");
	fprintf(ofp, "%d %d\n", img->width, img->height);
	fprintf(ofp, "%d\n", img->maxvalue);
}

