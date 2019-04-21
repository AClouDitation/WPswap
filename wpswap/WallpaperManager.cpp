#include "WallpaperManager.h"
#include <iostream>
#include <stdio.h>

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
