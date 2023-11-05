#pragma once
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <codecvt>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include <stack>
#include <vector>
#include <locale>

using namespace std;



class File {

private:
	bool _isFolder;
	wstring _name;
	float _size;
	int _first_cluster;
	vector<File*> _files;


	wstring _status;
	int _first_sectors;

public:

	bool isFolder() { return _isFolder; }
	wstring name() { return _name; }
	int first_cluster() { return _first_cluster; }
	float size() { return _size; }

	wstring status() {
		return _status;
	}


	File(bool isFolder, wstring name, float size, int first_cluster, wstring status, int first_sector) {
		_isFolder = isFolder;
		_name = name;
		_size = size;
		_first_cluster = first_cluster;
		_status = status;
		_first_sectors = first_sector;
	}

	vector<File*> files() {
		return _files;
	}

	void add(File* file) {
		_files.push_back(file);
	}


	void setName(wstring val) {
		_name = val;
	}

	void setFirstCluster(int val) {
		_first_cluster = val;
	}
	void setStatus(wstring val) {
		_status = val;
	}
	void setFirstSector(int val) {
		_first_sectors = val;
	}

	File* search(wstring file_name) {

		for (auto file : _files)
			if (file->name() == file_name)
				return file;

		return nullptr;
	}




	wstring information() {
		wstringstream builder;

		builder << left << setw(30) << _name;

		if (!_isFolder) {
			
			builder << left << setw(20) << _size;
		}

		else {
			builder << left << setw(20) << L"";
		}

		builder << left << setw(20) << _status;

		builder << left << setw(20) << _first_sectors;
		
		builder << endl;

		return builder.str();
	}

	void printTree(int level = 0) {


		wcout << left << setw(30) << L"name" << left << setw(20) << L"size (in bytes)" << left << setw(20) << L"status" << left << setw(20) << L"first sector on data" << endl << endl;
		for (auto file : _files)
			wcout << file->information();
	}

	File* findChild(wstring file_name) {


		File* result = nullptr;

		if (file_name.compare(_name) == 0) {
			return this;
		}
		else {
			for (File* child : _files) {
				result = child->findChild(file_name);
				if (result != nullptr)
					return result;
			}
			return nullptr;
		}
	}



	wstring ext() {

		wstring result = L"Folder";
		if (!_isFolder) {

			result = _name.substr(_name.find_first_of(L'.') + 1, 3);

		}
		return result;
	}

	
};