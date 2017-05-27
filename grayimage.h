

#ifndef IMAGE
#define IMAGE


#define FILTER_NAME "ImageProcessingFilterTemplate"
#define OID_ADTF_FILTER "de.tuchemnitz.adtf.template.imageprocessingfilter"



#include "stdafx.h"
#include "utilities.h"
class image : public cFilter
{
	ADTF_FILTER(OID_ADTF_FILTER, FILTER_NAME, OBJCAT_Generic)

private:
	// pins
	cVideoPin videoInput;
	cVideoPin VideoOutputPin;

	// video stuff
	tBitmapFormat videoInputInfo;
	tBitmapFormat videoOutputInfo;
	//
	cv::Mat outputImage;
	cv::Mat mask;

public:
	image(const tChar *__info);
	virtual ~image(void);

public:
	tResult Init(tInitStage eStage, __exception = NULL);
	tResult Start(__exception = NULL);
	tResult Stop(__exception = NULL);
	tResult Shutdown(tInitStage eStage, __exception = NULL);

	tResult OnPinEvent(adtf::IPin*, tInt, tInt, tInt, IMediaSample*);

private:
	tResult createInputPin(const tChar*, cVideoPin&);
	tResult createVideoOutputPin(const tChar*, cVideoPin&);

	//cv::Mat extractROI(const cv::Mat& inImage);

	tResult TransmitOutput(const tTimeStamp &timeStamp);
};

#endif 