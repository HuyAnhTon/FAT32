#include "FAT.h"

void menu() {
	cout << "\n----------------- Menu -------------------\n";

	cout << "1.  Print BootSector Information\n";
	cout << "2.  Print Directory Tree\n";
	cout << "3.  File Traversal (read content)\n";
	cout << "4.  Clear Screen\n";
	cout << "0.  End program\n";
}

void traversal_folder(FAT* fat32, File*& folder) {

	folder->printTree();
	wstring file_name;
	cout << "Input file name: ";
	wcin >> file_name;

	File* file = folder->search(file_name);

	if (file == nullptr) {
		cout << "FILE NOT FOUND!!!\n";
	}

	else if (file->ext() == L"txt")
	{
		cout << "CONTENT OF FILE:\n";
		cout << fat32->read_file(file_name);
		cout << endl;
	}
	else if (file->ext() == L"Folder")
	{
		system("cls");
		traversal_folder(fat32, file);
	}
	else {
		cout << "PLEASE USE PROPER SOFTWARE TO READ!!!\n";
	}

}


void load_menu()
{
	string choice = "";
	wstring prefix = L"\\\\.\\";
	wstring temp;
	cout << "Input Volume Name:(Uppercase) ";
	wcin >> temp;
	temp = prefix + temp + L":";
	LPCWSTR drive = temp.c_str();

	FAT* fat32 = new FAT(drive);

	while (true)
	{
		menu();

		cout << "Choice: ";
		cin >> choice;

		if (choice == "0")
		{
			break;
		}
		else if (choice == "1")
		{
			cout << "---------------BOOTSECTOR INFORMATION---------------\n";
			fat32->display_FAT32_information();
			cout << endl;
		}
		else if (choice == "2")
		{
			cout << "-------------------DIRECTORY TREE-------------------\n";
			fat32->display_directory_tree();

		}
		else if (choice == "3")
		{
			cout << "----------------------FILE TRAVERSAL----------------------\n";

			
			File* root = fat32->root_directory();
			root->printTree();

			wstring file_name;
			cout << "Input file name: ";
			wcin >> file_name;

			File* file = root->findChild(file_name);

			if (file == nullptr) {
				cout << "FILE NOT FOUND!!!\n";
			}

			else if (file->ext() == L"txt")
			{
				cout << "CONTENT OF FILE:\n";
				cout << fat32->read_file(file_name);
				cout << endl;
			}
			else if (file->ext() == L"Folder")
			{
				system("cls");
				traversal_folder(fat32, file);
			}
			else {
				cout << "PLEASE USE PROPER SOFTWARE TO READ!!!\n";
			}

		}
		else if (choice == "4")
			system("cls");
		else if (choice == "0")
		{
			cout << "END PROGRAM!!!\n";
		}
		else
		{
			cout << "WRONG INPUT, INPUT AGAIN!!!\n";
		}

	}

}

int main()
{
	load_menu();
	return 0;
}