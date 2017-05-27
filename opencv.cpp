
#include "stdafx.h"
#include "opencv.h"
#include "adtf2Opencv.h"
ADTF_FILTER_PLUGIN(FILTER_NAME, OID_ADTF_FILTER, opencv)
opencv::opencv(const tChar *__info)
	: cFilter(__info), logger(FILTER_NAME, 20)
{
	cMemoryBlock::MemSet(&videoInputInfo, 0, sizeof(videoInputInfo));
	cMemoryBlock::MemSet(&videoOutputInfo, 0, sizeof(videoOutputInfo));
}
opencv::~opencv()
{
}
tResult opencv::Init(tInitStage eStage, __exception)
{
	RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));
	if (eStage == StageFirst)
	{
		RETURN_IF_FAILED(createInputPin("videoInput", this->videoInput));
		RETURN_IF_FAILED(createVideoOutputPin("OutputVideo", this->videoOutput));
	}
	else if (eStage == StageNormal)
	{
	}
	else if (eStage == StageGraphReady)
	{
		cObjectPtr<IMediaType> pType;
		RETURN_IF_FAILED(videoInput.GetMediaType(&pType));
		cObjectPtr<IMediaTypeVideo> pTypeVideo;
		RETURN_IF_FAILED(pType->GetInterface(IID_ADTF_MEDIA_TYPE_VIDEO, (tVoid**)&pTypeVideo));
		
		const tBitmapFormat* pFormat = pTypeVideo->GetFormat();
		if (pFormat == NULL)
		{
			LOG_ERROR("No BItmap");
			RETURN_ERROR(ERR_NOT_SUPPORTED);
		}
		videoInputInfo = *pFormat;
		videoOutputInfo = *pFormat;
	
		videoOutput.SetFormat(&videoOutputInfo, NULL);
		logger.log(cString::Format("Input format: %d x %d @ %d Bit", videoInputInfo.nWidth, videoInputInfo.nHeight,
			videoInputInfo.nBitsPerPixel).GetPtr(), false);
		logger.log(cString::Format("Output format: %d x %d @ %d Bit", videoOutputInfo.nWidth, videoOutputInfo.nHeight,
			videoOutputInfo.nBitsPerPixel).GetPtr(), false);
	}
	RETURN_NOERROR;
}
tResult opencv::createInputPin(const tChar *pinName, cVideoPin &pin)
{
	pin.Create(pinName, IPin::PD_Input, static_cast<IPinEventSink*>(this));
	RETURN_IF_FAILED(RegisterPin(&pin));
	RETURN_NOERROR;
}
tResult opencv::createVideoOutputPin(const tChar *pinName, cVideoPin &pin)
{
	pin.Create(pinName, IPin::PD_Output, static_cast<IPinEventSink*>(this));
	RETURN_IF_FAILED(RegisterPin(&pin));
	RETURN_NOERROR;
}
tResult opencv::Start(__exception)
{
	RETURN_IF_FAILED(cFilter::Start(__exception_ptr));

	RETURN_NOERROR;
}
tResult opencv::Stop(__exception)
{
	RETURN_IF_FAILED(cFilter::Stop(__exception_ptr));

	RETURN_NOERROR;
}
tResult opencv::Shutdown(tInitStage eStage, __exception)
{
	RETURN_IF_FAILED(cFilter::Shutdown(eStage, __exception_ptr));
	RETURN_NOERROR;
	
}

tResult opencv::OnPinEvent(adtf::IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample)
{
	if (pSource == &videoInput && nEventCode == IPinEventSink::PE_MediaSampleReceived)
	{	

		RETURN_IF_POINTER_NULL(pMediaSample);
		const tVoid* l_pSrcBuffer;
		tVoid* l_pDestBuffer;
		cv::Mat img,gray;
		cImage iImage;
		cScopedSampleReadLock rl1(pMediaSample, &l_pSrcBuffer);
		RETURN_IF_FAILED(iImage.Attach((tUInt8*)l_pSrcBuffer, &videoInputInfo, NULL));
		img = cImageToCV(iImage);
		cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
		iImage = cvToCImage(img);
		cObjectPtr<IMediaSample> pNewsample;
		RETURN_IF_FAILED(_runtime->CreateInstance(OID_ADTF_MEDIA_SAMPLE, IID_ADTF_MEDIA_SAMPLE, (tVoid**)&pNewsample));
		RETURN_IF_FAILED(pNewsample->AllocBuffer(videoOutputInfo.nSize));
		if (IS_OK(pNewsample->WriteLock(&l_pDestBuffer)))
		{
			if (videoOutputInfo.nBitsPerPixel == videoInputInfo.nBitsPerPixel)
			{
				cImage::StretchBlit((tUInt8*)iImage.GetBitmap(), &videoInputInfo,
					(tUInt8*)l_pDestBuffer, &videoOutputInfo, 0);
			}
			pNewsample->Unlock(l_pDestBuffer);
		}
		pNewsample->SetTime(pMediaSample->GetTime());
		videoOutput.Transmit(pNewsample);

}
}
/*
tResult opencv::transmitVideoOutput(cv::Mat &image, cVideoPin &pin)
{
	if (!pin.IsConnected())
	{
		RETURN_NOERROR;
	}
	// Create new IMediaSample
	cObjectPtr<IMediaSample> sample;

	if (IS_OK(AllocMediaSample(&sample)))
	{
		sample->Update(_clock->GetTime(), image.data, this->videoOutputInfo.nSize, adtf::IMediaSample::MSF_None);
		RETURN_IF_FAILED(pin.Transmit(sample));
	}
	RETURN_NOERROR;
}
*/


