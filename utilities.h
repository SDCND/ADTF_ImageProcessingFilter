

#include "stdafx.h"

using namespace std;

class utilities
{
public:
	static int MatType(tInt16 bitsPerPixel);
	static cv::Mat ExtractImageFromMediaSample(IMediaSample *mediaSample, const tBitmapFormat &inputFormat);

};