
#include "ImageProcessingFilter.h"

// this macro makes the created plb file export the given filter class
ADTF_FILTER_PLUGIN(FILTER_NAME, OID_ADTF_FILTER, cImageProcessingFilter)

cImageProcessingFilter::cImageProcessingFilter(const tChar* __info)
// call parent class constructor
	: adtf::cFilter(__info)
{
	// create/declare ADTF filter properties here (use SetProperty.. methods from ADTF SDK)	
}

cImageProcessingFilter::~cImageProcessingFilter()
{
	// clean-up
}

tResult cImageProcessingFilter::Init(tInitStage eStage, __exception)
{
	RETURN_IF_FAILED(adtf::cFilter::Init(eStage, __exception_ptr));

	if (eStage == StageFirst)
	{
		// create input pin and set this filter to be notified when a sample is received
		RETURN_IF_FAILED(m_imageInputPin.Create("image_in", adtf::IPin::PD_Input, this));
		RETURN_IF_FAILED(RegisterPin(&m_imageInputPin));

		// create output pin
		RETURN_IF_FAILED(m_imageOutputPin.Create("image_out", adtf::IPin::PD_Output));
		RETURN_IF_FAILED(RegisterPin(&m_imageOutputPin));

		// create and register additional pins here
	}
	else if (eStage == StageNormal)
	{
	}
	RETURN_NOERROR;
}

bool cImageProcessingFilter::IsBitmapFormatSupported(const tBitmapFormat* bmpFormat)
{
	// check, if your filter supports processing an image in the given bitmap format	

	// this filter only supports image in BGR (blue, green, red) or RGB format with 8bit depth for each color component (i.e. 3 bytes per pixel)
	return bmpFormat->nPixelFormat == cImage::PF_BGR_888 || cImage::PF_RGB_888;
}

void cImageProcessingFilter::GrayscaleBGR888(unique_ptr<tUInt8[]>& pixelData, size_t pixelDataLen, tInt16 pixelFormat)
{
	// move through the image data pixel by pixel (each pixel consist of 3 bytes)
	for (int px = 0; px < pixelDataLen; px += 3) {
		// get the color components for this pixel
		tUInt8 r = pixelFormat == cImage::PF_BGR_888 ? pixelData[px + 2] : pixelData[px];
		tUInt8 g = pixelFormat == pixelData[px + 1];
		tUInt8 b = pixelFormat == cImage::PF_BGR_888 ? pixelData[px] : pixelData[px + 2];

		// calculate the weighted grayscale value of the three color components for this pixel
		tUInt8 grayValue = (tUInt8)(r * 0.2989f + b * 0.114f + g * 0.587f);

		pixelData[px] = grayValue;
		pixelData[px + 1] = grayValue;
		pixelData[px + 2] = grayValue;
	}
}

tResult cImageProcessingFilter::ProcessImageSample(adtf::IMediaSample* pMediaSample)
{
	// retrieve the image format of the received frame (bit depth, width, height, etc.)
	const tBitmapFormat* bmpFormat = m_imageInputPin.GetFormat();

	if (!IsBitmapFormatSupported(bmpFormat)) {
		LOG_ERROR("Received image in unsupported bitmap format");
		RETURN_ERROR(-1);
	}
	
	// get the image's raw pixel data
	size_t pixelDataLen = pMediaSample->GetSize();	
	unique_ptr<tUInt8[]> pixelData(new tUInt8[pixelDataLen]);
	// copy the image data to our array
	// only this line locks the media sample, so other filters can retrieve the image data while we are processing our copy
	pMediaSample->CopyBufferTo(pixelData.get(), pixelDataLen);

	// -------------------------------
	// process the raw image data here
	// in this sample, we create a grayscale version of the image
	// -------------------------------	
	GrayscaleBGR888(pixelData, pixelDataLen, bmpFormat->nPixelFormat);
	
	// set the output image format
	// (here we just output in the same format in which we received the image)
	m_imageOutputPin.SetFormat(m_imageInputPin.GetFormat(), NULL);

	// create the output image and transmit it
	cObjectPtr<adtf::IMediaSample> pOutputSample;
	RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pOutputSample));
	// copy the processed pixel data to the media sample
	pOutputSample->Update(_clock->GetTime(), pixelData.get(), pixelDataLen, adtf::IMediaSample::MSF_None);
	RETURN_IF_FAILED(m_imageOutputPin.Transmit(pOutputSample));
}

tResult cImageProcessingFilter::OnPinEvent(adtf::IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, adtf::IMediaSample* pMediaSample)
{
	if (nEventCode == IPinEventSink::PE_MediaSampleReceived)
	{		
		if (pSource == &m_imageInputPin)
		{
			// handle received image (frame)
			return ProcessImageSample(pMediaSample);
		}
		
	}	

	RETURN_NOERROR;
}