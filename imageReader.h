#ifndef READER
#define READER

#define FILTER_NAME "IMAGEREADER"
#define OID_ADTF_FILTER "IMAGEREADER.FILTER"

#include "C:\Users\mhoa\Downloads\ADTF_Templates\project2\stdafx.h"
class imageReader : public cFilter
{
	ADTF_FILTER(OID_ADTF_FILTER, FILTER_NAME, OBJCAT_Generic)
private:
	//cVideoPin videoInput;
	cVideoPin videoOutput;
	tBitmapFormat videoOutputInfo;//Get type of output
	cv::Mat outputImage;
	cv::Mat processedImage;
public:
	imageReader(const tChar *__info);
	virtual ~imageReader();
public:
	tResult Init(tInitStage eStage, __exception = NULL);
	tResult Start(__exception = NULL);
	tResult Stop(__exception = NULL);
	tResult Shutdown(tInitStage eStage, __exception = NULL);
private:
	//tResult createInputPin(const tChar*, cVideoPin&);
	tResult createOutputPin(const tChar*, cVideoPin&);
	tResult ReadData();  //transmit image to output pin
	cv::Mat ProcessImage(const cv::Mat& img);
};

#endif //READER