#ifndef RS_UTILS
#define RS_UTILS

namespace rs
{
	int clip(int val, int min, int max);

	int get_screen_width();
	int get_screen_height();

	void ScreenCap(void* buf, int* width = 0, int* height = 0);
}

#endif