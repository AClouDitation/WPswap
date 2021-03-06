#pragma once
#ifndef __WALLPAPERMANAGER_H__
#define __WALLPAPERMANAGER_H__
#include <windows.h>
#include <shobjidl.h>
#include <string>
#include <set>

class CoUninitializeOnExit {
public:
    CoUninitializeOnExit() {}
    ~CoUninitializeOnExit() { CoUninitialize(); }
};

class ReleaseOnExit {
public:
    ReleaseOnExit(IUnknown *p) : m_p(p) {}
    ~ReleaseOnExit() { if (nullptr != m_p) { m_p->Release(); } }
private:
    IUnknown *m_p;
};

class WallpaperManager
{
	std::string wallpaperCollectionPath;
	std::set<std::string> wallpaperPaths;
	int monitorCnt;

	int getMonitorCnt();
	int changeWallpaper(uint8_t, std::string);
public:
	WallpaperManager(std::string path);
	~WallpaperManager();

	void nextWallpaper();

};

#endif
