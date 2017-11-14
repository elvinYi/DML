
#ifndef AFP_SYNCHRONIZATOIN_H
#define AFP_SYNCHRONIZATOIN_H

typedef struct _BRay3DAudioSample
{
	short *sampleData;
	int length;
	double fs;
} BRay3DAudioSample;

int BRay3DAFPSync(BRay3DAudioSample *leftChannel, BRay3DAudioSample *rightChannel, double fps);

#endif /* AFP_SYNCHRONIZATOIN_H */

