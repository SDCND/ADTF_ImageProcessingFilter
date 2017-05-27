#include "utilities.h"

int utilities::MatType(tInt16 bitsPerPixel)
{
	switch(bitsPerPixel)
	{	
		case(24):
			return CV_8UC3; //rgb
		case(16):
			return CV_16UC1; //depth image
		case(8):
			return CV_8UC1; //gray
		default:
			return 0;
	}
}
cv::Mat utilities::ExtractImageFromMediaSample(IMediaSample *mediaSample, const tBitmapFormat &inputFormat)
{
		int depth;
		int channel;
		int Mtype = MatType(inputFormat.nBitsPerPixel);
	switch(Mtype)
	{
		case(CV_8UC3):
			depth = IPL_DEPTH_8U;
			channel = 3;
			break;
		case(CV_16UC1):
			depth= IPL_DEPTH_16U;
			channel = 1;
			break;
		case(CV_8UC1):
			depth = IPL_DEPTH_8U;
			channel =1;
			break;
		default:
			 return cv::Mat();
	}
	const tVoid* buffer; //create buffer
	if(IS_OK(mediaSample->Lock(&buffer)))
	{
		IplImage *imageBuffer = cvCreateImageHeader(cvSize(inputFormat.nWidth,inputFormat.nHeight),depth,channel);
		imageBuffer->imageData =(char*) buffer;
		cv::Mat image(cv::cvarrToMat(imageBuffer));
		cvReleaseImage(&imageBuffer);
		mediaSample->Unlock(buffer);
		return image;
	}
	return cv::Mat();
}
