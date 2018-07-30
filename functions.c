#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "functions.h"

int preview(char input[], char output[], struct imagesize *img)
{
	int quit = 0;
	SDL_Event event;
	 
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_JPG);
	 
	SDL_Window * window = SDL_CreateWindow("Input/Output Images",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (img->width * 2) + 5, img->height, 0);

	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface * leftimage = IMG_Load(input); 
	SDL_Texture * lefttexture = SDL_CreateTextureFromSurface(renderer, leftimage);

	SDL_Surface * rightimage = IMG_Load(output); 
	SDL_Texture * righttexture = SDL_CreateTextureFromSurface(renderer, rightimage);

	while (!quit)
	{
		SDL_WaitEvent(&event);
	 
		switch (event.type)
		{
		    case SDL_QUIT:
		        quit = 1;
		        break;
		}
		SDL_RenderClear(renderer);
		SDL_Rect leftrect = { 0, 0, img->width, img->height };
		SDL_Rect rightrect = { img->width + 5, 0, img->width, img->height };
    		SDL_RenderCopy(renderer, lefttexture, NULL, &leftrect);
		SDL_RenderCopy(renderer, righttexture, NULL, &rightrect);
        	SDL_RenderPresent(renderer);
	}
	 
	SDL_DestroyTexture(lefttexture);
	SDL_FreeSurface(leftimage);
	SDL_DestroyTexture(righttexture);
	SDL_FreeSurface(rightimage);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	IMG_Quit();
 
	SDL_Quit();
	 
	return 0;
}

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

