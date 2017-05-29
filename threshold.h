#ifndef THRESHOLD
#define THRESHOLD

#define FILTER_NAME "THRESHOLD"
#define OID_ADTF_FILTER "THRESHOLD.FILTER"

#include "stdafx.h"

class threshold : public cFilter
{
	ADTF_FILTER(OID_ADTF_FILTER,FILTER_NAME,OBJCAT_Generic)
private:
	cVideoPin videoInput;
	cVideoPin videoOutput;
	
	tBitmapFormat videoInputInfo;
	tBitmapFormat videoOutputInfo;
	
	cv::Mat outputImage;
public:
	threshold(const tChar *__infor);
virtual ~threshold();
public:
	tResult Init(tInitStage eStage,__exception=NULL);
	tResult Start(__exception=NULL);
	tResult Stop(__exception=NULL);
	tResult Shutdown(tInitStage eStage,__exception=NULL);
	
	tResult OnPinEvent(IPin*, tInt, tInt, tInt, IMediaSample*);

private:
	tResult createInputPin(const tChar*,cVideoPin&);
	tResult createOutputPin(const tChar*,cVideoPin&);
	tResult TransmitOutput(const tTimeStamp &timeStamp);
};


#endif