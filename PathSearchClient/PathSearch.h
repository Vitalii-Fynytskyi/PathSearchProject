#pragma once

#include<string>
#include<iostream>
#include<thread>
#include<mutex>
#include <filesystem>

namespace fs = std::filesystem;
class PathSearch
{
public:

	static std::wstring GetPathToFile(std::wstring& fileNameToSet);
private:
	class SearchHelper {
	public:
		std::wstring FileName;
		std::wstring SearchPath();
		~SearchHelper();

	private:
		const int MAX_ALLOWED_THREADS = 8;
		void directoryProcessing(fs::directory_entry dir);
		void grabNextDirectory();
		std::vector<fs::directory_entry> rootSubdirectories;
		std::vector<std::thread*> threadsSubdirectories;
		int lastGrabbedSubdirectoryIndex = -1;
		std::wstring searchResult;

		std::mutex subdirectoryIndexMutex;
		std::mutex coutMutex;

	};
};

