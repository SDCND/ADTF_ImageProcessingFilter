#include "imageReader.h"
ADTF_FILTER_PLUGIN(FILTER_NAME,OID_ADTF_FILTER,imageReader)
imageReader::imageReader(const tChar* __info)
	:cFilter(__info)
{
	outputImage = cv::imread("C:\\Users\\mhoa\\Downloads\\ADTF_Templates\\project2\\im2.png", 1); //read image from a folder
	//cMemoryBlock::MemSet(&videoOutputInfo, 0, sizeof(videoOutputInfo));

}
imageReader::~imageReader()
{}
tResult imageReader::Init(tInitStage eStage, __exception)
{
	RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr));
	if (eStage == StageFirst)
	{
		RETURN_IF_FAILED(createOutputPin("imageOutput", this->videoOutput));
	}
	else if(eStage==StageNormal)
	{	
		
	}
	else if (eStage == StageGraphReady) {
		//ProcessImage
		processedImage = ProcessImage(outputImage);
		//Get type of output pin
		videoOutputInfo.nWidth = processedImage.size().width;
		videoOutputInfo.nHeight = processedImage.size().height;
		videoOutputInfo.nBitsPerPixel = 8*processedImage.channels();
		switch (videoOutputInfo.nBitsPerPixel)
		{	case(8):
				videoOutputInfo.nPixelFormat = cImage::PF_GREYSCALE_8;
				break;
			case(24):
				videoOutputInfo.nPixelFormat = cImage::PF_RGB_888;
				break;
			case(16):
				videoOutputInfo.nPixelFormat = cImage::PF_GREYSCALE_16;
				break;
			default:
				LOG_ERROR("Wrong format");
				break;
		}
		videoOutputInfo.nBytesPerLine = processedImage.size().width * processedImage.channels();;
		videoOutputInfo.nSize = videoOutputInfo.nBytesPerLine*processedImage.size().height;
		videoOutputInfo.nPaletteSize = 0;
		
		videoOutput.SetFormat(&videoOutputInfo, NULL);
		//cMemoryBlock::MemCopy(&videoOutputInfo, videoOutput.GetFormat(), sizeof(tBitmapFormat));

		ReadData();//read image
		
	}
	RETURN_NOERROR;
}

tResult imageReader::createOutputPin(const tChar *pinName, cVideoPin &pin)
{
	pin.Create(pinName, IPin::PD_Output, static_cast<IPinEventSink*>(this));
	RETURN_IF_FAILED(RegisterPin(&pin));
	RETURN_NOERROR;

}
tResult imageReader::Shutdown(tInitStage eStage, __exception)
{
	RETURN_IF_FAILED(cFilter::Shutdown(eStage, __exception_ptr));
	RETURN_NOERROR;
}
tResult imageReader::Start(__exception)
{
	RETURN_IF_FAILED(cFilter::Start(__exception_ptr));
	RETURN_NOERROR;
}
tResult imageReader::Stop(__exception)
{
	RETURN_IF_FAILED(cFilter::Stop(__exception_ptr));
	RETURN_NOERROR;
}

tResult imageReader::ReadData()
{
	if (videoOutput.IsConnected())
	{
		cObjectPtr<IMediaSample> mediaSample;
		RETURN_IF_FAILED(_runtime->CreateInstance(OID_ADTF_MEDIA_SAMPLE, IID_ADTF_MEDIA_SAMPLE, (tVoid**)&mediaSample));
		RETURN_IF_FAILED(mediaSample->AllocBuffer(videoOutputInfo.nSize));
		mediaSample->Update(_clock->GetTime(), processedImage.data, videoOutputInfo.nSize, IMediaSample::MSF_None);
		videoOutput.Transmit(mediaSample);
	}
	RETURN_NOERROR;
}
cv::Mat imageReader::ProcessImage(const cv::Mat& img)
{
	cv::Mat dst;
	//cv::cvtColor(img, dst, CV_BGR2GRAY);
	dst = img;
	return dst;
}


