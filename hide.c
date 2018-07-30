#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "functions.h"


void convertToBin(char c, unsigned int binarray[])
{
	int counter = 0; //counter for marking each bit being converted
	for (int i = 7; i >= 0; --i)
	{	
		binarray[counter] = ((c & (1 << i)) ? '1' : '0'); 
		//putchar(binmsg[counter]);
		counter += 1;
	}
	//putchar('\n');
}

int compareLSB(FILE *fp, FILE *ofp, char c, unsigned int binmsg[], int byte[], int LSB)
{	
	for (int i = 0; i < 8; i++)
	{
		/*Error handling for the case in which there are no more bytes to read from the image but the message isn't completely hidden yet*/
		if (fread(byte, 1, 1, fp))
		{
			LSB = *byte & 1;

			/*If the least significant bit of the read-in byte is not the same as the message bit, and is equal to 0 then change it to 1*/
			if ((LSB != binmsg[i] - 48) && LSB == 0)
			{
				byte[0] = *byte | 1;
			}
			/*If the least significant bit of the read-in byte is not the same as the message bit, and is equal to 1 then change it to 0*/
			else if ((LSB != binmsg[i] - 48) && LSB == 1)
			{
				byte[0] = *byte & ~1;
			}
			/*If the least significant bit of the read-in byte is the same as the message bit then leave it as is*/
			else
			{
				//NLSB = *byte & 1;
			}
			fwrite(byte, 1, 1, ofp);
		}
		
		/*Error message when byte cannot be read*/
		else
		{
			return 1;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	/*Check to see whether program has two parameters; the program requires the name of an input file that must be in PPM format, and an output file.*/
	if (argc < 3)
	{
		printf("Program requires two parameters, the name of an input file in PPM format and an output file.\n");
		return 1;
	}
	/*Check for -p flag*/
	if (strcmp(argv[1], "-p") == 0)
	{
		/*Check if -p functionality was given the correct number of parameters*/
		if (argc != 3)
		{
			printf("The -p flag requires only one additional parameter; the file containing the input information.\n");
			return 1;
		}	
		
		FILE *fp;
		fp = fopen(argv[2], "r");
		char message[15], input[15], output[15]; //storage for arguments in each line of file
		if (fp)
		{
			int r = fscanf(fp, "%s %s %s", message, input, output);
			//while there are still lines in the file to read
			while (r != EOF)
			{
				//Create child process
				pid_t pid = fork();
				//fork() error
				if (pid < 0)
				{
					printf("Fork() error.\n");
					return 1;
				}
				//Child process continues here
				else if (pid == 0)
				{
					/*Open the file in the parameter for reading and writing*/
					FILE *fp; 
					fp = fopen(input, "rb");

					/*If a NULL pointer is returned then input file failed to open*/
					if (!fp)
					{
						printf("Could not open input file.\n");
						exit(1);
					}
					else
					{
						//printf("File opened.\n");
					}

					/*Call function to check whether header is valid for PPM format*/
					if (checkIfPPM(fp) == 1)
					{
						return 1;
					}

					/*Call function to skip through comments*/
					checkComments(fp);
					struct imagesize img = { .width = 0, .height = 0, .maxvalue = 0 };
					int image; 

					/*Call function to read size of image in header and store in struct*/
					image = readSize(fp, &img);
					if (image == 1)
					{
						printf("Invalid width or height value in header.\n");		
						return 1;
					}

					/*Call function to read max value of pixel in header and store in struct*/
					image = readMaxValue(fp, &img);
					if (image == 1)
					{
						printf("Invalid maximum value for pixel in header.\n");	
						//printf("Value: %d\n", img.maxvalue);	
						return 1;
					}
	
					/*Open output file and write header information to it*/
					FILE *ofp;
					ofp = fopen(output, "wb");

					/*If a NULL pointer is returned then output file failed to open*/
					if (!ofp)
					{
						printf("Could not open output file.\n");
						exit(1);
					}
					else
					{
						/*Call function to write the header information to the output file*/
						outputHeader(ofp, &img);
					}

					/*Read in message from text file one character at a time*/
					char c; //variable to store character being read
					unsigned int binmsg[8]; //array to store the character's binary representation
					
					FILE *mfp;
					mfp = fopen(message, "r");
	
					int byte[1];	//array for storing each byte from image as they're read
					int LSB = 0;	//Least significant bit
					
					while ((c = getc(mfp)) != '\n' && c != EOF)
					{		
						/*If there is whitespace in the message, represent it as an underscore for binary conversion*/
						if (c == ' ')
							c = '_';
	
						/*Convert the current character from stdin to binary*/
						convertToBin(c, binmsg);
		
						/*Grab byte from file, compare least significant bit to binary representation of current character. If they are the same then leave it, if different then change LSB to be equal to current bit of current character*/
						if (compareLSB(fp, ofp, c, binmsg, byte, LSB) == 1)
						{
							printf("The message is too long to be stored in the image.\n");
							return 1;
						}
		
					}
					/*Add a last byte of 00000000 to signify the end of the message for unhide*/
					c = 0;
	
					/*Convert the current character from stdin to binary using bitwise operators and store it in an array of size 8, room for 8 bits*/
					convertToBin(c, binmsg);	

					if (compareLSB(fp, ofp, c, binmsg, byte, LSB) == 1)
					{
						printf("The message is too long to be stored in the image.\n");
						return 1;
					}

					/*Continue reading and writing the remaining bytes of the image to the new file until there are no more bytes to read*/
					while (fread(byte, 1, 1, fp))
					{
						fwrite(byte, 1, 1, ofp);
					}	
		
					fclose(fp);
					fclose(ofp);
					fclose(mfp);
		
					exit(-1);
				}
				
				r = fscanf(fp, "%s %s %s", message, input, output);
			}
		}
		else
		{
			printf("Could not open input file.\n");
			return 1;
		}
	}


	/*Check for -m flag*/
	else if (strcmp(argv[1], "-m") == 0)
	{
		/*Check if -m functionality was given the correct number of parameters*/
		if (argc != 5)
		{
			printf("The -m flag requires three parameters; the number of files, the input file basename and the output file basename, in that order.\n");
			return 1;
		}		

		/*Check how many files to open and output, assign to variable*/
		int numfiles; //holds number of files given in arguments
		numfiles = atoi(argv[2]);
		//printf("No. of files: %d\n", numfiles);

		struct dirent **namelist;
   		int n;
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
				/*If file has same basename as argument, store it in a sorted array. This is done with string comparison, partially comparing filename in d_name with the basename in the arguments given to the program.*/
				if (strncmp(namelist[m]->d_name, argv[3], strlen(argv[3])) == 0)
				{
       					//printf("%s\n", namelist[m]->d_name);
					filearray[i] = strdup(namelist[m]->d_name);
					//printf("filearray[%d]: %s\n", i, filearray[i]);
					i++;
				}
					
       				free(namelist[m]);
				m++;
       			}
       			free(namelist);
		}

		int end = 0;
		int filectr = 0; //file counter, to iterate from zero to number stored in numfiles
		while (!end)
		{	
			/*Open the file in the parameter for reading and writing*/
			FILE *fp; 
			fp = fopen(filearray[filectr], "rb");

			/*If a NULL pointer is returned then input file failed to open*/
			if (!fp)
			{
				printf("Could not open input file.\n");
				exit(1);
			}
			else
			{
				//printf("File opened.\n");
			}

			/*Call function to check whether header is valid for PPM format*/
			if (checkIfPPM(fp) == 1)
			{
				return 1;
			}
			
			/*Call function to skip through comments*/
			checkComments(fp);
			struct imagesize img = { .width = 0, .height = 0, .maxvalue = 0 };
			int image; 

			/*Call function to read size of image in header and store in struct*/
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
				//printf("Value: %d\n", img.maxvalue);	
				return 1;
			}
	
			/*Open output file and write header information to it*/
			char buffer[32];
			snprintf(buffer, sizeof(char) * 32, "%s-%d", argv[4], filectr);

			FILE *ofp;
			ofp = fopen(buffer, "wb");

			/*If a NULL pointer is returned then output file failed to open*/
			if (!ofp)
			{
				printf("Could not open output file.\n");
				exit(1);
			}
			else
			{
				/*Call function to write the header information to the output file*/
				outputHeader(ofp, &img);
			}

			char c; //variable to store character being read
			unsigned int binmsg[8];	//array to store the character's binary representation
			int byte[1];	//array for storing each byte from image as they're read
			int LSB = 0;	//Least significant bit
			int reset = 0;	//variable to control whether to return to start of while loop

			/*Read in message from stdin one character at a time*/
			while ((c = getchar()) != EOF)
			{		
				/*If there is whitespace in the message, represent it as an underscore for binary conversion*/
				if (c == ' ')
					c = '_';
	
				/*Convert the current character from stdin to binary*/
				convertToBin(c, binmsg);
		
				/*Grab byte from file, compare least significant bit to binary representation of current character. If they are the same then leave it, if different then change LSB to be equal to current bit of current character*/

				int compare = compareLSB(fp, ofp, c, binmsg, byte, LSB);
				if (compare == 1 && filectr < numfiles)
				{
					filectr++;
					fclose(fp);
					fclose(ofp);
					printf("It works now.\n");
					reset = 1;
					break;	
				}
				else if (compare == 1)
				{	
					printf("(1)The message is too long to be stored in the image/s.\n");
					return 1;
				}
		
			}
			if (reset == 1)
				continue;

			/*Add a last byte of 00000000 to signify the end of the message for unhide*/
			c = 0;
	
			/*Convert the current character from stdin to binary using bitwise operators and store it in an array of size 8, room for 8 bits*/
			convertToBin(c, binmsg);	

			if (compareLSB(fp, ofp, c, binmsg, byte, LSB) == 1 && filectr < numfiles)
			{
				filectr++;
				fclose(fp);
				fclose(ofp);
				continue;				
			}
			else if (compareLSB(fp, ofp, c, binmsg, byte, LSB) == 1)
			{
				printf("(2)The message is too long to be stored in the image/s.\n");
				return 1;
			}

			/*Continue reading and writing the remaining bytes of the image to the new file until there are no more bytes to read*/
			while (fread(byte, 1, 1, fp))
			{
				fwrite(byte, 1, 1, ofp);
			}	
		
			fclose(fp);
			fclose(ofp);
			end = 1;
		}

		puts("Done! Message encoded.");
		return 0;
   	}

	/*Continue with either basic hide functionality or -s image preview*/
	else
	{
		int sdl = 0;
		if (strcmp(argv[1], "-s") == 0)
		{
			sdl = 1;
	
			/*Check if -s functionality was given the correct number of parameters*/
			if (argc != 4)
			{
				printf("The -s flag requires only two additional parameters; the input file and output file.\n");
				return 1;
			}	
		}

		/*Open the file in the parameter for reading and writing*/
		FILE *fp;
		if (sdl == 1)
		{
			fp = fopen(argv[2], "rb");
		}
		else
		{
			fp = fopen(argv[1], "rb");
		}

		/*If a NULL pointer is returned then input file failed to open*/
		if (!fp)
		{
			printf("?Could not open input file.\n");
			exit(1);
		}
		else
		{
			//printf("File opened.\n");
		}

		/*Call function to check whether header is valid for PPM format*/
		if (checkIfPPM(fp) == 1)
		{
			return 1;
		}
		
		/*Call function to skip through comments*/
		checkComments(fp);
		struct imagesize img = { .width = 0, .height = 0, .maxvalue = 0 };
		int image; 

		/*Call function to read size of image in header and store in struct*/
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
			//printf("Value: %d\n", img.maxvalue);	
			return 1;
		}
	
		/*Open output file and write header information to it*/
		FILE *ofp;
		if (sdl == 1)
		{
			ofp = fopen(argv[3], "wb");
		}
		else
		{
			ofp = fopen(argv[2], "wb");
		}

		/*If a NULL pointer is returned then output file failed to open*/
		if (!ofp)
		{
			printf("Could not open output file.\n");
			exit(1);
		}
		else
		{
			/*Call function to write the header information to the output file*/
			outputHeader(ofp, &img);
		}

		/*Read in message from stdin one character at a time*/
		char c; //variable to store character being read
		unsigned int binmsg[8];	//array to store the character's binary representation
		printf("\nPlease enter the message that you want to hide: ");
	
		int byte[1];	//array for storing each byte from image as they're read
		int LSB = 0;	//Least significant bit
		//int NLSB;
		while ((c = getchar()) != '\n' && c != EOF)
		{		
			/*If there is whitespace in the message, represent it as an underscore for binary conversion*/
			if (c == ' ')
				c = '_';
	
			/*Convert the current character from stdin to binary*/
			convertToBin(c, binmsg);
		
			/*Grab byte from file, compare least significant bit to binary representation of current character. If they are the same then leave it, if different then change LSB to be equal to current bit of current character*/
			if (compareLSB(fp, ofp, c, binmsg, byte, LSB) == 1)
			{
				printf("The message is too long to be stored in the image.\n");
				return 1;
			}
		
		}
		/*Add a last byte of 00000000 to signify the end of the message for unhide*/
		c = 0;
	
		/*Convert the current character from stdin to binary using bitwise operators and store it in an array of size 8, room for 8 bits*/
		convertToBin(c, binmsg);	

		if (compareLSB(fp, ofp, c, binmsg, byte, LSB) == 1)
		{
			printf("The message is too long to be stored in the image.\n");
			return 1;
		}

		/*Continue reading and writing the remaining bytes of the image to the new file until there are no more bytes to read*/
		while (fread(byte, 1, 1, fp))
		{
			fwrite(byte, 1, 1, ofp);
		}	
		
		fclose(fp);
		fclose(ofp);

		if (sdl == 1)
		{
			preview(argv[2], argv[3], &img);
		}
		
		puts("Done! Message encoded.");
		return 0;
	}

}






















