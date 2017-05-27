//#define ADTF_A_UTILS_LEGACY_NAMES
#include <adtf_platform_inc.h>
#include <adtf_plugin_sdk.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>

cv::Mat cImageToCV(const cImage& img)
{
	const tInt16 format = img.GetFormat()->nPixelFormat;
	int type = CV_8UC3;
	switch(format)
	{
	case(IImage::PF_GREYSCALE_8):
		type = CV_8UC1;
		break;
	case(IImage::PF_RGB_888):
		type = CV_8UC3;
		break;
	case(IImage::PF_GREYSCALE_16):
		type = CV_16UC1;
		break;
	default:
		std::stringstream ss;
		ss << "Wrong PixelFormat in input image: " << format << " (Unsupported by cImageToCV)." << endl;;
		LOG_ERROR(ss.str().c_str());
		break;
	}
	return cv::Mat(img.GetHeight(), img.GetWidth(), type, img.GetBitmap());
}
cImage cvToCImage(const cv::Mat& img)
{
	std::stringstream ss;
	cImage resultIm;
	tInt resultFormat = IImage::PF_UNKNOWN;
	switch (img.type())
	{
	case(CV_8UC1):
		resultFormat = IImage::PF_GREYSCALE_8;
		break;
	case(CV_16UC1):
		resultFormat = IImage::PF_GREYSCALE_16;
		break;
	case(CV_8UC3):
		resultFormat = IImage::PF_RGB_888;
		break;
	default:
		std::stringstream ss;
		ss << "Wrong PixelFormat in input image: " << img.type() << " (Unsupported by cvToCImage).";
		LOG_ERROR(ss.str().c_str());
		return resultIm;
		break;
	}
	
		tResult res = resultIm.Create(img.size().width, img.size().height, img.elemSize() * 8, img.step, NULL, resultFormat);
	
	if (!res == ERR_NOERROR)
	{
		LOG_ERROR("Could not create ADTF image in cvToCImage!");
		return resultIm;
	}

	if ((size_t)resultIm.GetSize() != (size_t)img.step*img.rows)
	{
		LOG_ERROR("Something went wrong. Sizes of OpenCV image and ADTF image don't match. Cannot copy!");
		return resultIm;
	}
	tUInt8* rawPtr = resultIm.GetBitmap();
	if (rawPtr == NULL)
	{
		LOG_ERROR("Bitmap returned nullpointer when converting to ADTF format.");
	}
	else {
		memcpy(rawPtr, (char*)img.data, img.step * img.rows);
	}

	return resultIm;
}