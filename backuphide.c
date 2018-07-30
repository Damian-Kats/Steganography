#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	/*Check to see whether program has two parameters*/
	if (argc != 3)
	{
		printf("Program requires two parameters, the name of an input file in PPM 				format and an output file.\n");
		return 0;
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
		printf("File opened.\n");
	}

	/*Check if file is in PPM format*/
	char a, b;
	char arr[20];
	fgets(arr, sizeof(arr), fp);
	sscanf(arr, "%c %c", &a, &b);
	printf("%c%c\n", a, b);
	if (a == 'P' && b == '6')
	{
		printf("This file is a PPM file.\n");
	}
	else
	{
		printf("This file is not a PPM file.\n");
		exit(1);
	}

	/*Check for comments*/
	int c;
	c = getc(fp);
    	while (c == '#') 
	{
    		while (getc(fp) != '\n');
         		c = getc(fp);
    	}

    	ungetc(c, fp);

	/*Check width and height information*/
	int width, height;
	char line[20];
	fgets(line, sizeof(line), fp);
	sscanf(line, "%d %d", &width, &height);
	printf("Width is %d and height is %d.\n", width, height);

	/*Check maximum value*/
	int max;
	char value[20];
	fgets(value, sizeof(value), fp);
	sscanf(value, "%d", &max);
	printf("Max value is %d.\n", max);

	/*Read in message from stdin*/
	char message[25] = {0};
	int i;
	int msglen = 0;
	printf("Please enter the message that you want to hide: ");
	fgets(message, sizeof(message), stdin);
	for (i = 0; i < sizeof(message); i++)
	{
		if (message[i] != '\n' && message[i] != 0)
		{
			printf("%c", message[i]);
			msglen += 1;
		}
	}
	printf("\n");
	printf("Message length is %d characters.\n", msglen);
	int binmsglen = 8 * msglen;
	
	/*Convert message to binary*/
	unsigned int binmsg[100] = {2};
	int k = 0;
	int counter = 0;
	while (message[k] != '\n') 
	{
		for (i = 7; i >= 0; --i)
		{	
			binmsg[counter] = ((message[k] & (1 << i)) ? '1' : '0'); 
			putchar(binmsg[counter]);
			counter += 1;
		}
		putchar('\n');
		k++;
	}

	/*Print header information to output file*/
	FILE *ofp;
	ofp = fopen(argv[2], "wb");
	fprintf(ofp, "P6\n");
	fprintf(ofp, "%d %d\n", width, height);
	fprintf(ofp, "%d\n", max);

	/*Grab byte from file, compare least significant bit to current bit of message*/
	int byte[1];
	int LSB;
	int NLSB;
	int j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < 3 * width; j++)
		{
			//byte[0] = fgetc(fp);
			fread(byte, 1, 1, fp);
			LSB = *byte & 1;
			if ((i == 0 && (LSB && j < binmsglen)))
			{
				printf("I: %d\n", i);
				printf("J: %d\n", j);
				printf("LSB: %d\n", LSB); 
				printf("Message bit: %d\n", binmsg[j] - 48);
				if (LSB != binmsg[j] - 48)
				{
					byte[0]  = (*byte & 0xfe) | (binmsg[7 + 8*j] & 0xfe);
					NLSB = *byte & 1;
					printf("NLSB: %d\n", NLSB); 
					printf("\n");
				}
				else
				{
					NLSB = *byte & 1;
					printf("NLSB: %d\n", NLSB);
					printf("\n");
				}
			}
			fwrite(byte, 1, 1, ofp); 
		}
	}
			
	fclose(fp);
	fclose(ofp);
	return 1;
}
	























