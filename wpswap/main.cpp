// main.cpp

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "WallpaperManager.h"


int main() {
	std::cout << "Hello World" << std::endl;

	std::string wallpaperCollectionPath;
	std::cout << "Please provide Wallpaper Collection Path: ";
	std::cin >> wallpaperCollectionPath;
	WallpaperManager wallpaperManager(wallpaperCollectionPath);

	return 0;
}