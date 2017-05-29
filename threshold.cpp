#include "stdafx.h"
#include "threshold.h"

ADTF_FILTER_PLUGIN(FILTER_NAME, OID_ADTF_FILTER, threshold)

threshold::threshold(const tChar *__info)
	:cFilter(__info)
	{
		
	}
threshold::~threshold()
{}
tResult threshold::Init(tInitStage eStage,__exception)
{
	RETURN_IF_FAILED(cFilter::Init(eStage,__exception_ptr));
	if(eStage==StageFirst)
	{
		RETURN_IF_FAILED(createInputPin("videoInput",this->videoInput));
		RETURN_IF_FAILED(createOutputPin("videoOutput",this->videoOutput));
	}
	else if(eStage==StageNormal){}
	else if(eStage==StageGraphReady){
		cObjectPtr<IMediaType> mediaType;
		RETURN_IF_FAILED(videoInput.GetMediaType(&mediaType));
		cObjectPtr<IMediaTypeVideo> videoType;
		RETURN_IF_FAILED(mediaType->GetInterface(IID_ADTF_MEDIA_TYPE_VIDEO,(tVoid**)&videoType));
		videoInputInfo = *(videoType->GetFormat());
		videoOutputInfo= *(videoType->GetFormat());
		videoOutput.SetFormat(&videoOutputInfo, NULL);

	}
	RETURN_NOERROR;
}
tResult threshold::createInputPin(const tChar *pinName,cVideoPin &pin)
{
	pin.Create(pinName,IPin::PD_Input,static_cast<IPinEventSink*>(this));
	RETURN_IF_FAILED(RegisterPin(&pin));
	RETURN_NOERROR;
	
}
tResult threshold::createOutputPin(const tChar *pinName,cVideoPin &pin)
{
	pin.Create(pinName,IPin::PD_Output,static_cast<IPinEventSink*>(this));
	RETURN_IF_FAILED(RegisterPin(&pin));
	RETURN_NOERROR;
	
}
tResult threshold::Shutdown(tInitStage eStage,__exception)
{
	RETURN_IF_FAILED(cFilter::Shutdown(eStage,__exception_ptr));
	RETURN_NOERROR;
}
tResult threshold::Start(__exception)
{
	RETURN_IF_FAILED(cFilter::Start(__exception_ptr));
	RETURN_NOERROR;
}
tResult threshold::Stop(__exception)
{
	RETURN_IF_FAILED(cFilter::Stop(__exception_ptr));
	RETURN_NOERROR;
}
tResult threshold::OnPinEvent(IPin *pSource,tInt nEventCode,tInt param1,tInt param2,IMediaSample *mediaSample)
{
	if(nEventCode==IPinEventSink::PE_MediaSampleReceived)
	{
		if(pSource==&this->videoInput)
		{
			const tVoid *buffer;
			if(IS_OK(mediaSample->Lock(&buffer)))
			{
				//Receive imagesize
				outputImage = cv::Mat (cv::Size(this->videoInputInfo.nWidth,this->videoInputInfo.nHeight),CV_8UC1,(char*)buffer);
				mediaSample->Unlock(buffer);

				cv::threshold(outputImage, outputImage, 128, 255, 1);
				TransmitOutput(mediaSample->GetTime());
			}
		}
		else{
			RETURN_ERROR(ERR_NOT_SUPPORTED);
		}
	}
	RETURN_NOERROR;
	
}
tResult threshold::TransmitOutput(const tTimeStamp &timeStamp)
{
	cObjectPtr<IMediaSample> mediaSample;
	RETURN_IF_FAILED(_runtime->CreateInstance(OID_ADTF_MEDIA_SAMPLE,IID_ADTF_MEDIA_SAMPLE,(tVoid**)&mediaSample));
	RETURN_IF_FAILED(mediaSample->AllocBuffer(videoOutputInfo.nSize));
	mediaSample->Update(timeStamp,outputImage.data,videoOutputInfo.nSize,0);
	videoOutput.Transmit(mediaSample);
	RETURN_NOERROR;
}




