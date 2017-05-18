#pragma once

// include ADTF SDK
#define ADTF_A_UTILS_LEGACY_NAMES
#include <adtf_platform_inc.h>
#include <adtf_plugin_sdk.h>

// change namespace here
#define OID_ADTF_FILTER "de.tuchemnitz.adtf.template.imageprocessingfilter"

// change filter name here
#define FILTER_NAME "ImageProcessingFilterTemplate"

// the filter class (change the name accordingly)
class cImageProcessingFilter : public adtf::cFilter
{
	// you may change the filter category (the node under which it appears in the component tree here)
	ADTF_FILTER(OID_ADTF_FILTER, FILTER_NAME, adtf::OBJCAT_Generic)	

public:
	cImageProcessingFilter(const tChar* __info);
	virtual ~cImageProcessingFilter();

private:
	adtf::cVideoPin					m_imageInputPin;
	adtf::cVideoPin					m_imageOutputPin;

	// put your additional pin and field declarations here

private:
	// filter internal methods
	bool IsBitmapFormatSupported(const tBitmapFormat* bmpFormat);
	void GrayscaleBGR888(unique_ptr<tUInt8[]>& pixelData, size_t pixelDataLen, tInt16 pixelFormat);
	tResult ProcessImageSample(adtf::IMediaSample* pMediaSample);

public:
	// put additional overrides for cFilter methods here (e.g. Start, Stop, Shutdown);
	tResult Init(tInitStage eStage, __exception = NULL);

public: // implements IPinEventSink
	tResult OnPinEvent(adtf::IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, adtf::IMediaSample* pMediaSample);
};