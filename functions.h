#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

struct imagesize {
	int width;
	int height;
	int maxvalue;
};

int preview(char input[], char output[], struct imagesize *img);

int checkIfPPM(FILE *fp);

void checkComments(FILE *fp);

int readSize(FILE *fp, struct imagesize *img);

int readMaxValue(FILE *fp, struct imagesize *img);

void outputHeader(FILE *ofp, struct imagesize *img);

#endif
