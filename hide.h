#ifndef HIDE_H_
#define HIDE_H_

struct imagesize {
	int width;
	int height;
	int maxvalue;
};

void checkIfPPM(FILE *fp);

void checkComments(FILE *fp);

int readSize(FILE *fp, struct imagesize *img);

int readMaxValue(FILE *fp, struct imagesize *img);

void outputHeader(FILE *ofp, struct imagesize *img);

int hide(int argc, char *argv[]);

#endif
