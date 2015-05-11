#include <windows.h>
#include <stdio.h>

namespace rs
{
	int clip(int val, int min, int max)
	{
		if (val < min)
			return min;
		if (val > max)
			return max;
		return val;
	}


	int get_screen_width()
	{
		return GetSystemMetrics(SM_CXSCREEN);
	}

	int get_screen_height()
	{
		return GetSystemMetrics(SM_CYSCREEN);
	}

	//最终f的内存布局为rgb格式
	void ScreenCap(void* buf, int* w, int* h)
	{

		HWND hDesk = GetDesktopWindow();
		HDC hScreen = GetDC(hDesk);
		int width = GetDeviceCaps(hScreen, HORZRES);
		int height = GetDeviceCaps(hScreen, VERTRES);

		if (w != 0)
			*w = width;
		if (h != 0)
			*h = height;

		HDC hdcMem = CreateCompatibleDC(hScreen);
		HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);

		BITMAPINFOHEADER bmi = { 0 };
		bmi.biSize = sizeof(BITMAPINFOHEADER);
		bmi.biPlanes = 1;
		bmi.biBitCount = 32;
		bmi.biWidth = width;
		bmi.biHeight = -height;
		bmi.biCompression = BI_RGB;
		bmi.biSizeImage = width*height;

		SelectObject(hdcMem, hBitmap);
		BitBlt(hdcMem, 0, 0, width, height, hScreen, 0, 0, SRCCOPY);

		GetDIBits(hdcMem, hBitmap, 0, height, buf, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

		DeleteDC(hdcMem);
		ReleaseDC(hDesk, hScreen);
		CloseWindow(hDesk);
		DeleteObject(hBitmap);
	}
}

