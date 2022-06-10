#include<iostream>
#include "PathSearch.h"
using namespace std;
int main() {
	wcout<<L"Enter file name to search on C:\\ drive: ";
	wstring FileName;
	wcin>>FileName;
	wstring result = PathSearch::GetPathToFile(FileName);
	wcout<<L"Result path is "<<result<<L"\n";
	cin.get();
	cin.get();
	return 0;
}