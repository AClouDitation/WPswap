// main.cpp

#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>
#include "WallpaperManager.h"


int main() {
	std::cout << "Hello World" << std::endl;

	std::string wallpaperCollectionPath;
	std::cout << "Please provide Wallpaper Collection Path: ";
	std::cin >> wallpaperCollectionPath;
	WallpaperManager wallpaperManager(wallpaperCollectionPath);

	while (true) {
		wallpaperManager.nextWallpaper();
		Sleep(5000);
	}
	return 0;
}