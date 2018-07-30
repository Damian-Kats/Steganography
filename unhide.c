#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "functions.h"

int main(int argc, char *argv[])
{
	/*Check to see whether program has at least two parameters*/
	if (argc < 2)
	{
		printf("Program requires one command line parameter, the name of an input file in PPM format.\n");
		return 1;
	}
	/*Check for -m flag*/
	if (strcmp(argv[1], "-m") == 0)
	{
		struct dirent **namelist;
   		int n;
		int numfiles = 0;
		int m = 0;
	
		/*Initialise array for storing input filenames*/
		char **filearray = malloc(15);

		/*Scan directory and save number of sorted files in variable n*/
   		n = scandir(".", &namelist, 0, alphasort);
   		if (n < 0)
       			perror("scandir");
   		else 
		{
			int i = 0;
       			while (m < n) 
			{
				/*If file has same basename as argument, store it in a sorted array*/
				if (strncmp(namelist[m]->d_name, argv[2], strlen(argv[2])) == 0)
				{
       					//printf("%s\n", namelist[m]->d_name);
					filearray[i] = strdup(namelist[m]->d_name);
					//printf("filearray[%d]: %s\n", i, filearray[i]);
					i++;
					numfiles++;
				}
       				free(namelist[m]);
				m++;
       			}
       			free(namelist);
		}
		
		/*Iterate through each of the files that share the basename specified in the arguments and stored in the sorted list filearray. */
		for (int i = 0; i < numfiles; i++)
		{
			/*Open the file in the parameter for reading and writing*/
			FILE *fp; 
			fp = fopen(filearray[i], "rb");

			/*If a NULL pointer is returned then file failed to open*/
			if (!fp)
			{
				printf("Could not open file.\n");
				exit(1);
			}
	
			if (checkIfPPM(fp) == 1)
			{
				return 1;
			}

			checkComments(fp);
			
			/*Initialise struct for values of the PPM image*/
			struct imagesize img = { .width = 0, .height = 0, .maxvalue = 0 };
			int image; 
			image = readSize(fp, &img);
			if (image == 1)
			{
				/*Error handling for invalid width or height in PPM header*/
				printf("Invalid width or height value in header.\n");		
				return 1;
			}

			image = readMaxValue(fp, &img);
			if (image == 1)
			{
				/*Error handling for invalid maximum pixel value in PPM header*/
				printf("Invalid maximum value for pixel in header.\n");		
				return 1;
			}

			/*Grab byte from file and remember least significant bit, every 8 bits convert to ascii*/
			int byte[1];	//array for storing each byte from image as they're read
			int bits[8] = {2};	//array for storing each bit of a character in message
		 	int LSB;	//Least significant bit
			int done = 0;	//control for while loop below
			int cont = 0;	
			if (i == 0)
				printf("\nSecret message: ");
			while (done != 1)
			{
				int dec = 0;
				/*Read first 8 bytes*/
				for (int i = 0; i < 8; i++)
				{
					/*Continue read byte and processing LSB until out of bytes*/
					if (fread(byte, 1, 1, fp))
					{
						LSB = *byte & 1;
						bits[i] = LSB;
					}
					else
					{
						cont = 1;
						continue;
					}
				}
				/*break out of while loop and start uncoding message in next file*/
				if (cont == 1)
					break;

				/*Convert to decimal*/
				for (int i = 0; i < 8; i++)
				{
					dec = dec * 2 + bits[i];
					if (dec == '_')
						dec = ' ';
				}
				
		
				/*End when 00000000 is reached, marks end of message*/
				if (dec == 0)
					done = 1;
				else
					printf("%c", dec);
			}
	
			fclose(fp);
		}
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
	
	if (checkIfPPM(fp) == 1)
	{
		return 1;
	}
	checkComments(fp);
	struct imagesize img = { .width = 0, .height = 0, .maxvalue = 0 };
	int image; 
	image = readSize(fp, &img);
	if (image == 1)
	{
		printf("Invalid width or height value in header.\n");		
		return 1;
	}
	image = readMaxValue(fp, &img);
	if (image == 1)
	{
		printf("Invalid maximum value for pixel in header.\n");		
		return 1;
	}

	/*Grab byte from file and remember least significant bit, every 8 bits convert to ascii*/
	/*8 bits representing 0 means end of secret message*/
	int byte[1];	//array for storing each byte from image as they're read
	int bits[8] = {2};	//array for storing each bit of a character in message
	int LSB;	//Least significant bit
	int done = 0;	//control for while loop below
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
			if (dec == '_')
				dec = ' ';
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
	
	fclose(fp);
	
	return 0;
}
	
	
