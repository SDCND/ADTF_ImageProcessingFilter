#ifndef test
#define test

#define FILTER_NAME "Test"
#define OID_ADTF_FILTER "adtf.imageprocessingfilter"
#include "stdafx.h"
#include "logger.h"
class opencv : public cFilter
{
	ADTF_FILTER(OID_ADTF_FILTER, FILTER_NAME, OBJCAT_Generic)
private:
	cVideoPin videoInput;
	cVideoPin videoOutput;
	//
	logger logger;
	tBitmapFormat videoInputInfo;
	tBitmapFormat videoOutputInfo;
public:
	opencv(const tChar *__info);
	virtual ~opencv();
public:
	tResult Init(tInitStage eStage, __exception = NULL);
	tResult Start(__exception = NULL);
	tResult Stop(__exception = NULL);
	tResult Shutdown(tInitStage eStage, __exception = NULL);
	
	tResult OnPinEvent(adtf::IPin*, tInt, tInt, tInt, IMediaSample*);
private:
	//tResult transmitVideoOutput(cv::Mat&, cVideoPin&);
	//tResult process(IMediaSample*);
	tResult createInputPin(const tChar*, cVideoPin&);
	tResult createVideoOutputPin(const tChar*, cVideoPin&);


};



#endif