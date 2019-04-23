#include "WallpaperManager.h"
#include <iostream>
#include <stdio.h>
#include <filesystem>
#include <algorithm>
#include <windows.h>
#include <objidl.h>
#pragma warning(push)
#pragma warning(disable:4458)
#include <gdiplus.h>
#pragma warning(pop)
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define LOG(format, ...) wprintf(format L"\n", __VA_ARGS__)

// ref: https://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode
std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}


WallpaperManager::WallpaperManager(std::string path) :
	wallpaperCollectionPath(path), monitorCnt(getMonitorCnt())
{
	//TODO: Check if path is valid
	if (monitorCnt < 1) exit(1);
	std::cerr << monitorCnt << " monitors dectected" << std::endl;

	namespace fs = std::filesystem;
	for (const auto & entry : fs::directory_iterator(path))
		wallpaperPaths.insert(entry.path().string());
}


WallpaperManager::~WallpaperManager()
{
}


int WallpaperManager::getMonitorCnt() {

    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        LOG(L"CoInitialize returned 0x%08x", hr);
        return -__LINE__;
    }
    CoUninitializeOnExit cuoe;

    IDesktopWallpaper *pDesktopWallpaper = nullptr;
    hr = CoCreateInstance(__uuidof(DesktopWallpaper), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pDesktopWallpaper));
    if (FAILED(hr)) {
        LOG(L"CoCreateInstance(__uuidof(DesktopWallpaper)) returned 0x%08x", hr);
        return -__LINE__;
    }
    ReleaseOnExit releaseDesktopWallpaper(pDesktopWallpaper);
	unsigned int mc;
	hr = pDesktopWallpaper->GetMonitorDevicePathCount(&mc);
    if (FAILED(hr)) {
        LOG(L"CoCreateInstance(__uuidof(DesktopWallpaper)) returned 0x%08x", hr);
        return -__LINE__;
    }

	return (int)mc;
}

int WallpaperManager::changeWallpaper(uint8_t monitorID, std::string wallpaperPath) {
    // coinitialize
    // cocreate a desktop wallpaper object
    // call IDesktopWallpaper->SetWallpaper
    
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        LOG(L"CoInitialize returned 0x%08x", hr);
        return -__LINE__;
    }
    CoUninitializeOnExit cuoe;

    IDesktopWallpaper *pDesktopWallpaper = nullptr;
    hr = CoCreateInstance(__uuidof(DesktopWallpaper), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pDesktopWallpaper));
    if (FAILED(hr)) {
        LOG(L"CoCreateInstance(__uuidof(DesktopWallpaper)) returned 0x%08x", hr);
        return -__LINE__;
    }
    ReleaseOnExit releaseDesktopWallpaper(pDesktopWallpaper);
    
	std::wstring stemp = s2ws(wallpaperPath);
    LPCWSTR wallpaper = stemp.c_str();
	LPWSTR monitorDevicePath = nullptr;

	hr = pDesktopWallpaper->GetMonitorDevicePathAt(monitorID, &monitorDevicePath);
    if (FAILED(hr)) {
        LOG(L"IDesktopWallpaper::GetMonitorDevicePathAt returned 0x%08x", hr);
        return -__LINE__;
    }

    hr = pDesktopWallpaper->SetWallpaper(monitorDevicePath, wallpaper);
    if (FAILED(hr)) {
        LOG(L"IDesktopWallpaper::SetWallpaper returned 0x%08x", hr);
        return -__LINE__;
    }

    LOG(L"Setting the desktop wallpaper to %s succeeded.\n", wallpaper);
	return 0;

}


bool isLandScape(std::string fn) {

	// Start Gdiplus 
    Gdiplus::GdiplusStartupInput gdiplusStartupInput; 
    ULONG_PTR gdiplusToken; 
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL); 
 
    // Load the image 
	std::wstring wsFn = std::wstring(fn.begin(), fn.end());
    Gdiplus::Image* image = Gdiplus::Image::FromFile(wsFn.c_str()); 
	unsigned int height = image->GetHeight();
	unsigned int width = image->GetWidth();

	delete image;
	image = 0;
	return width > height;
}

void WallpaperManager::nextWallpaper() {
	static uint8_t nextMonitor = 0;
	std::string nextWallpaperFn = "";
	int cnt = 0;
	do {
		int index = rand() % wallpaperPaths.size();
		nextWallpaperFn = *std::next(wallpaperPaths.begin(), index);
	} while ((isLandScape(nextWallpaperFn) ^ (bool)nextMonitor) && cnt++ < 10);

	changeWallpaper(nextMonitor++, nextWallpaperFn);
	nextMonitor %= monitorCnt;
}
