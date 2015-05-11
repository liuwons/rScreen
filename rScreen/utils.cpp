#include <windows.h>

//最终f的内存布局为rgb格式
void ScreenCap(void* buf, int& width, int& height)
{
    HWND hDesk = GetDesktopWindow();
    HDC hScreen = GetDC(hDesk);
    width = GetDeviceCaps(hScreen, HORZRES);
    height = GetDeviceCaps(hScreen, VERTRES);

    HDC hdcMem = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);

    BITMAPINFOHEADER bmi = { 0 };
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biPlanes = 1;
    bmi.biBitCount = 24;
    bmi.biWidth = width;
    bmi.biHeight = -height;
    bmi.biCompression = BI_RGB;
    bmi.biSizeImage = width*height;

    SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, width, height, hScreen, 0, 0, SRCCOPY);

    GetDIBits(hdcMem, hBitmap, 0, height, buf, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

    DeleteDC(hdcMem);
    ReleaseDC(hDesk, hScreen);
}