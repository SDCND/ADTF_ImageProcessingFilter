
#include "stdafx.h"
#include "grayimage.h"

ADTF_FILTER_PLUGIN(FILTER_NAME, OID_ADTF_FILTER, image)


image::image(const tChar *__info)
	: cFilter(__info)
{
}

image::~image(void)
{}

tResult image::Init(tInitStage eStage, __exception)
{
	RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));

	if (eStage == StageFirst)
	{

		RETURN_IF_FAILED(createInputPin("videoInput", this->videoInput));
		RETURN_IF_FAILED(createVideoOutputPin("cuttedVideo", this->VideoOutputPin));

	}
	else if (eStage == StageNormal) {}

	else if (eStage == StageGraphReady) {
		cObjectPtr<IMediaType> mediaType;
		RETURN_IF_FAILED(videoInput.GetMediaType(&mediaType));
		cObjectPtr<IMediaTypeVideo> videoType;
		RETURN_IF_FAILED(mediaType->GetInterface(IID_ADTF_MEDIA_TYPE_VIDEO, (tVoid**)&videoType));
		videoInputInfo = *(videoType->GetFormat());
		//Set format for outputPin
		videoOutputInfo.nWidth = videoInputInfo.nWidth;
		videoOutputInfo.nHeight = videoInputInfo.nHeight;
		videoOutputInfo.nBitsPerPixel = 8;
		videoOutputInfo.nPixelFormat = cImage::PF_GREYSCALE_8;
		videoOutputInfo.nBytesPerLine = videoInputInfo.nWidth;
		videoOutputInfo.nSize = videoOutputInfo.nBytesPerLine*videoInputInfo.nHeight;
		videoOutputInfo.nPaletteSize = 0;
		VideoOutputPin.SetFormat(&videoOutputInfo, NULL);
	}
		
	RETURN_NOERROR;
}

tResult image::createInputPin(const tChar *pinName, cVideoPin &pin)
{
	pin.Create(pinName, IPin::PD_Input, static_cast<IPinEventSink*>(this));
	RETURN_IF_FAILED(RegisterPin(&pin));

	RETURN_NOERROR;
}
tResult image::createVideoOutputPin(const tChar *pinName, cVideoPin &pin)
{
	pin.Create(pinName, IPin::PD_Output, static_cast<IPinEventSink*>(this));
	RETURN_IF_FAILED(RegisterPin(&pin));

	RETURN_NOERROR;
}

tResult image::Start(__exception)
{
	RETURN_IF_FAILED(cFilter::Start(__exception_ptr));

	RETURN_NOERROR;
}

tResult image::Stop(__exception)
{
	RETURN_IF_FAILED(cFilter::Stop(__exception_ptr));

	RETURN_NOERROR;
}

tResult image::Shutdown(tInitStage eStage, __exception)
{
	RETURN_IF_FAILED(cFilter::Shutdown(eStage, __exception_ptr));

	RETURN_NOERROR;
}

tResult image::OnPinEvent(IPin *psource, tInt eventCore, tInt param1, tInt param2, IMediaSample *mediaSample)
{
	RETURN_IF_POINTER_NULL(mediaSample);

	if (eventCore == IPinEventSink::PE_MediaSampleReceived)
	{
		if (psource == &this->videoInput)
		{
			utilities::ExtractImageFromMediaSample(mediaSample, videoInputInfo).copyTo(outputImage);
			RETURN_IF_POINTER_NULL(outputImage.data);
			int mat = utilities::MatType(videoInputInfo.nBitsPerPixel);
			switch (mat)
			{
			case(CV_8UC3):
				cv::cvtColor(outputImage, outputImage, CV_BGR2GRAY);
				//cv::threshold(outputImage, outputImage, 128, 255, 1);
				break;
			case(CV_16UC1):
				outputImage = outputImage / 255;
				outputImage.convertTo(outputImage, CV_8UC1);
				//cv::threshold(outputImage, outputImage, 128, 255, 1);
				break;
			case(CV_8UC1):
				//cv::threshold(outputImage, outputImage, 128, 255, 1);
				break;
			}
			TransmitOutput(mediaSample->GetTime());
			}		
		else
		{
			RETURN_ERROR(ERR_NOT_SUPPORTED);
		}
		}
	

	RETURN_NOERROR;
}

tResult image::TransmitOutput(const tTimeStamp &timeStamp)
{
	cObjectPtr <IMediaSample> mediaSample;
	RETURN_IF_FAILED(_runtime->CreateInstance(OID_ADTF_MEDIA_SAMPLE, IID_ADTF_MEDIA_SAMPLE, (tVoid * *)& mediaSample));
	RETURN_IF_FAILED(mediaSample->AllocBuffer(videoOutputInfo.nSize));
	mediaSample->Update(timeStamp, outputImage.data, videoOutputInfo.nSize, 0);

	VideoOutputPin.Transmit(mediaSample);

	RETURN_NOERROR;
}
