#include "PathSearch.h"

namespace fs = std::filesystem;
std::wstring PathSearch::GetPathToFile(std::wstring& fileNameToSet)
{
	SearchHelper searchHelper;
	searchHelper.FileName = fileNameToSet;
	return searchHelper.SearchPath();
}

std::wstring PathSearch::SearchHelper::SearchPath()
{
	fs::path directory = L"C:\\";
	//iterate all files in root directory
	for (const auto& entry : std::filesystem::directory_iterator(directory, fs::directory_options::skip_permission_denied)) {
		try {
			if (fs::is_directory(entry)) {
				rootSubdirectories.push_back(entry);
			}
			else {
				if (entry.path().filename() == FileName) {
					return entry.path();
				}
			}
		}
		catch (...) {
			continue;
		}
	}
	if (rootSubdirectories.size() < MAX_ALLOWED_THREADS) {
		lastGrabbedSubdirectoryIndex = rootSubdirectories.size() - 1;
	}
	else {
		lastGrabbedSubdirectoryIndex = MAX_ALLOWED_THREADS - 1;
	}
	for (int i = 0; i < MAX_ALLOWED_THREADS && i < rootSubdirectories.size(); ++i) {
		std::thread* threadDirectoryProcessing = new std::thread(&PathSearch::SearchHelper::directoryProcessing, this, rootSubdirectories.at(i));
		threadsSubdirectories.push_back(threadDirectoryProcessing);
	}
	for (std::thread* t : threadsSubdirectories) {
		t->join();
	}
	return searchResult;
}

PathSearch::SearchHelper::~SearchHelper()
{
	for (std::thread* t : threadsSubdirectories) {
		delete t;
	}
}

void PathSearch::SearchHelper::grabNextDirectory()
{
	subdirectoryIndexMutex.lock();
	int index = ++lastGrabbedSubdirectoryIndex;
	subdirectoryIndexMutex.unlock();
	if (rootSubdirectories.size() <= index)
		return;

	directoryProcessing(rootSubdirectories.at(index));
}

void PathSearch::SearchHelper::directoryProcessing(fs::directory_entry dir)
{
	try {
		for (const auto& entry : fs::recursive_directory_iterator(dir, fs::directory_options::skip_permission_denied)) {
			try {
				if (searchResult.empty() == false) {
					return;
				}

				if (fs::is_regular_file(entry)) {
					if (entry.path().filename() == FileName) {
						searchResult = entry.path();
						coutMutex.lock();
						std::wcout << L"Path to file is found: " << searchResult << L"\n";
						coutMutex.unlock();
						return;
					}
				}
			}
			catch (...) {
				continue;
			}
		}
		if (searchResult.empty()) {
			grabNextDirectory();
		}
	}
	catch (...) {
		coutMutex.lock();
		std::wcout << L"An error occured while iterating files in directory "<<dir<<L". Thread was stopped.\n";
		coutMutex.unlock();
	}


}