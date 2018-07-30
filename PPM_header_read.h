#ifndef PPM_HEADER_READ_H_
#define PPM_HEADER_READ_H_

struct imagesize {
	int width;
	int height;
	int maxvalue;
};

int checkIfPPM(FILE *fp);

void checkComments(FILE *fp);

int readSize(FILE *fp, struct imagesize *img);

int readMaxValue(FILE *fp, struct imagesize *img);

void outputHeader(FILE *ofp, struct imagesize *img);

#endif
