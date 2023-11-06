#pragma once
#include "File.h"

string Int2String(int n);
wstring read_offset_wide_string(string offset, int n, const BYTE* sectors);
void printInformation(const BYTE sector[512]);
string read_offset_raw(string offset, int n, const BYTE* sector);
wstring read_short_name(string offset, int n, const BYTE* sector);
string read_offset(string offset, int n, const BYTE sector[512]);
wstring clean_wstring(wstring s);
int ReadSector(LPCWSTR  drive, int readPoint, BYTE sector[512]);
int Hex2Dec(string hex);
wstring get_status(BYTE*);


class FAT
{
private:
    BYTE BootSector[512];
    LPCWSTR  drive;
    int bytes_per_sector;
    int sectors_per_cluster;
    int sectors_BootSector;
    int quantity_FAT;
    int first_RDET_cluster;
    int sectors_per_fat;
    int total_sectors_on_disk;
    int first_data_sector;
    int first_RDET_sector;

public:
    FAT();
    FAT(LPCWSTR  drive);
    void display_FAT32_information();
    void display_directory_tree();
    string FAT_type();
    int first_sector_of_cluster(int cluster_number);
    vector<int> clusters_holding(int first_cluster);
    vector<BYTE*> get_entries(int first_cluster);
    File* read_entries(vector<BYTE*> entries);
    string read_sector_content(int sector_index);
    string read_file(wstring file_name);
    File* root_directory();
    vector<int> sectors_on_disk(int first_cluster);

};



FAT::FAT()
{
    drive = L"\\\\.\\D:";
    ReadSector(drive, 0, BootSector);

    bytes_per_sector = Hex2Dec(read_offset("B", 2, BootSector));
    sectors_per_cluster = Hex2Dec(read_offset("D", 1, BootSector));
    sectors_BootSector = Hex2Dec(read_offset("E", 2, BootSector));
    quantity_FAT = Hex2Dec(read_offset("10", 1, BootSector));
    first_RDET_cluster = Hex2Dec(read_offset("2C", 4, BootSector));
    sectors_per_fat = Hex2Dec(read_offset("24", 4, BootSector));
    total_sectors_on_disk = Hex2Dec(read_offset("20", 4, BootSector));
    first_data_sector = sectors_BootSector + quantity_FAT * sectors_per_fat;
    first_RDET_sector = first_sector_of_cluster(first_RDET_cluster);

}

FAT::FAT(LPCWSTR  drive)
{
    this->drive = drive;
    ReadSector(drive, 0, BootSector);

    bytes_per_sector = Hex2Dec(read_offset("B", 2, BootSector));
    sectors_per_cluster = Hex2Dec(read_offset("D", 1, BootSector));
    sectors_BootSector = Hex2Dec(read_offset("E", 2, BootSector));
    quantity_FAT = Hex2Dec(read_offset("10", 1, BootSector));
    first_RDET_cluster = Hex2Dec(read_offset("2C", 4, BootSector));
    sectors_per_fat = Hex2Dec(read_offset("24", 4, BootSector));
    total_sectors_on_disk = Hex2Dec(read_offset("20", 4, BootSector));
    first_data_sector = sectors_BootSector + quantity_FAT * sectors_per_fat;
    first_RDET_sector = first_sector_of_cluster(first_RDET_cluster);
    first_RDET_sector = first_sector_of_cluster(first_RDET_cluster);


}

void FAT::display_FAT32_information()

{
    cout << "1. FAT type:                   " << this->FAT_type() << endl;
    cout << "2. Bytes / 1 sector:           " << dec << bytes_per_sector << endl;
    cout << "3. Sectors / 1 cluster (sC):   " << dec << sectors_per_cluster << endl;
    cout << "4. Sectors in BootSector (sB): " << dec << sectors_BootSector << endl;
    cout << "5. Quantity of FAT (nF):       " << dec << quantity_FAT << endl;
    cout << "6. RDET sectors:               " << dec << clusters_holding(first_RDET_cluster).size() * sectors_per_cluster << endl;
    cout << "7. Total sectors on disk:      " << dec << total_sectors_on_disk << endl;
    cout << "8. Sectors / 1 FAT (sF):       " << dec << sectors_per_fat << endl;
    cout << "9. First sector of FAT 1:      " << dec << sectors_BootSector << endl;
    cout << "10. First sector of RDET:      " << dec << first_RDET_sector << endl;
    cout << "11. First sector of Data:      " << dec << first_data_sector << endl;
}
void FAT::display_directory_tree()
{
    File* root = root_directory();
    root->printTree();
}
string FAT::FAT_type()
{
    // Offset 52, doc 8 byte
    string offset = "52";
    int n = 8;

    stringstream builder;
    int start_index = stoi(offset, 0, 16);

    for (int i = 0; i < n; i++) {
        builder << BootSector[start_index + i];
    }

    return builder.str();
}

int FAT::first_sector_of_cluster(int cluster_number)
{
    BYTE sector[512];
    ReadSector(drive, 0, sector);

    int result = first_data_sector + (cluster_number - 2) * sectors_per_cluster;

    return result;
}

vector<int> FAT::clusters_holding(int first_cluster)
{
    vector<int> clusters;
    string data = "";

    int current = first_cluster;
    int fat_sector = first_cluster / 128;

    while (data != "0fffffff") {

        BYTE fat[512];
        ReadSector(drive, (sectors_BootSector + fat_sector) * 512, fat);
        clusters.push_back(current);

        string offset = Int2String((current % 128) * 4);
        data = read_offset(offset, 4, fat);


        if (data != "0fffffff") {
            current = stoi(data, 0, 16);
        }
    }
    return clusters;
}





vector<BYTE*> FAT::get_entries(int first_cluster)
{
    // initial

    vector<int> clusters = clusters_holding(first_cluster);
    vector<BYTE*> entries;


    for (auto cluster : clusters) {

        int first_sector_of_cluster = first_data_sector + (cluster - 2) * sectors_per_cluster;

        for (int j = 0; j < sectors_per_cluster; j++) {

            BYTE sector[512];

            ReadSector(drive, (first_sector_of_cluster + j) * 512, sector);


            for (int k = 0; k < 512; k = k + 32) {

                BYTE* new_entry = new BYTE[32];

                for (int h = 0; h < 32; h++) {
                    new_entry[h] = sector[k + h];
                }

                entries.push_back(new_entry);
            }
        }
    }

    return entries;
}



File* FAT::read_entries(vector<BYTE*> entries)
{

    File* root = new File(true, L"Root directory", 0, 0, L" ", first_RDET_sector);
    stack<BYTE*> sub_entries;
    bool isFolder = false;
    wstring name = L"";
    wstring ext = L"";
    int first_cluster;
    int size = 0;
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    bool have_sub_entries = false;

    for (int i = 2; i < entries.size(); i++) {

        if (entries[i][0] == stoi("E5", 0, 16) || entries[i][0] == 0) {
            continue;
        }
        // sub_entry
        else if (entries[i][11] == 0x0F) {
            sub_entries.push(entries[i]);
        }
        else if (entries[i][11] == 0x10 || entries[i][11] == 0x20) {

            while (!sub_entries.empty()) {

                have_sub_entries = true;
                BYTE* sub_entry = sub_entries.top();
                sub_entries.pop();

                name += read_offset_wide_string("1", 10, sub_entry);
                name += read_offset_wide_string("E", 12, sub_entry);
                name += read_offset_wide_string("1C", 4, sub_entry);
            }

            if (entries[i][11] == 0x10) {
                isFolder = true;
                ext = L"";

                if (!have_sub_entries) {
                    name = read_short_name("0", 8, entries[i]);
                    name = clean_wstring(name);
                }

                wstring status = get_status(entries[i]);
                string cluster_bytes = read_offset("14", 2, entries[i]) + read_offset("1A", 2, entries[i]);
                first_cluster = stoi(cluster_bytes, 0, 16);

                name = clean_wstring(name);
                File* f = new File(isFolder, name, size, first_cluster, status, first_sector_of_cluster(first_cluster));
                vector<BYTE*> folder_entries = get_entries(first_cluster);
                File* folder = read_entries(folder_entries);
                folder->setName(name);
                folder->setFirstCluster(first_cluster);
                folder->setStatus(status);
                folder->setFirstSector(first_sector_of_cluster(first_cluster));
                root->add(folder);

            }
            else {
                if (!have_sub_entries) {
                    name = read_short_name("0", 8, entries[i]);
                    ext = converter.from_bytes(read_offset_raw("8", 3, entries[i]));
                    name += L"." + ext;
                    name = clean_wstring(name);

                }

                wstring status = get_status(entries[i]);
                name = clean_wstring(name);
                string cluster_bytes = read_offset("14", 2, entries[i]) + read_offset("1A", 2, entries[i]);
                first_cluster = stoi(cluster_bytes, 0, 16);
                int first_sector = first_sector_of_cluster(first_cluster);
                vector<int> sectors = sectors_on_disk(first_cluster);

                string size_bytes = read_offset("1C", 4, entries[i]);
                size = stoi(size_bytes, 0, 16);

                File* f = new File(isFolder, name, size, first_cluster, status, first_sector);
                root->add(f);
            }

            name = L"";
            ext = L"";
            isFolder = false;
            have_sub_entries = false;
        }
        else {
            while (!sub_entries.empty())
                sub_entries.pop();
        }
    }
    return root;
}

File* FAT::root_directory()
{
    File* root = read_entries(get_entries(first_RDET_cluster));
    return root;
}

string FAT::read_sector_content(int sector_index)
{
    BYTE sector[512];
    ReadSector(drive, sector_index * 512, sector);
    stringstream builder;

    for (int i = 0; i < 512; i++) {
        if (sector[i] == 0)
            break;
        builder << sector[i];
    }
    return builder.str();
}

string FAT::read_file(wstring file_name)
{
    File* root = root_directory();
    string result = "";
    vector<int> clusters;
    File* files = root->findChild(file_name);

    clusters = clusters_holding(files->first_cluster());


    vector<int> sectors;
    for (auto cluster : clusters)
    {
        int first_sector = first_sector_of_cluster(cluster);
        for (int i = 0; i < sectors_per_cluster; i++)
        {
            sectors.push_back(first_sector);
            first_sector++;
        }
    }


    for (auto sector : sectors)
    {
        result += read_sector_content(sector);
    }
    return result;
}





string Int2String(int n) {
    stringstream builder;
    builder << hex << n;
    return builder.str();
}

int Hex2Dec(string hex) {

    int res = 0;
    int base = 1;

    for (int i = hex.size() - 1; i >= 0; i--) {

        if (hex[i] >= '0' && hex[i] <= '9') {

            res += (int(hex[i]) - 48) * base;
            base *= 16;
        }
        else {

            res += (int(hex[i]) - 87) * base;
            base *= 16;
        }
    }
    return res;
}


int ReadSector(LPCWSTR  drive, int readPoint, BYTE sector[512])
{
    int retCode = 0;
    DWORD bytesRead;
    HANDLE device = nullptr;

    device = CreateFileW(drive,    // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        nullptr,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        nullptr);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        printf("CreateFile: %u\n", GetLastError());
        return 1;
    }

    SetFilePointer(device, readPoint, nullptr, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, 512, &bytesRead, nullptr))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
    else
    {
        //printf("Success!\n");
    }
}

wstring clean_wstring(wstring s) {

    wstring result;
    for (auto character : s) {
        if (character != L'\0')
            result.push_back(character);

    }
    return result;
}

string read_offset(string offset, int n, const BYTE sector[512]) {

    stringstream builder;

    int start_index = stoi(offset, 0, 16);
    int end_index = start_index + (n - 1);


    for (int i = 0; i < n; i++) {

        builder << hex << setw(2) << setfill('0') << int(sector[end_index - i]);
    }

    return builder.str();
}

string read_offset_raw(string offset, int n, const BYTE* sector) {

    stringstream builder;

    for (int i = 0; i < n; i++) {

        builder << sector[stoi(offset, 0, 16) + i];
    }

    return builder.str();
}

wstring read_short_name(string offset, int n, const BYTE* sector) {

    wstring result;
    for (int i = 0; i < n; i++) {
        if (sector[stoi(offset, 0, 16) + i] != 0x20) {

            wchar_t wideChar = static_cast<wchar_t>(sector[stoi(offset, 0, 16) + i]);
            result += wideChar;
        }
            
    }
    return result;

}

void printInformation(const BYTE sector[512]) {

    for (int i = 0; i < 512; i++) {

        cout << hex << setw(2) << setfill('0') << int(sector[i]) << " ";

        if ((i + 1) % 4 == 0)
            cout << " ";
        if ((i + 1) % 16 == 0)
            cout << endl;
    }
}

wstring read_offset_wide_string(string offset, int n, const BYTE* sectors) {

    wstring result;


    for (int i = 0; i < n; i++) {
        if (int((sectors[stoi(offset, 0, 16) + i])) == 255)
            break;

        wchar_t wideChar = static_cast<wchar_t>(sectors[stoi(offset, 0, 16) + i]);
        result += wideChar;
    }


    return clean_wstring(result);
}


wstring get_status(BYTE* entries) {

    wstring status = L"";
    BYTE status_byte = entries[0xB];

    if ((status_byte & 1)== 1)
        status.append(L"R");
    status_byte = status_byte >> 1;

    if ((status_byte & 1) == 1)
        status.append(L"H");
    status_byte = status_byte >> 1;

    if ((status_byte & 1) == 1)
        status.append(L"S");
    status_byte = status_byte >> 1;

    if ((status_byte & 1) == 1)
        status.append(L"V");
    status_byte = status_byte >> 1;

    if ((status_byte & 1) == 1)
        status.append(L"D");
    status_byte = status_byte >> 1;

    if ((status_byte & 1) == 1)
        status.append(L"A");

    return status;
}


vector<int> FAT::sectors_on_disk(int first_cluster) {

    vector<int> sectors;
    vector<int> clusters = clusters_holding(first_cluster);

    for (auto cluster : clusters) {
        int n = sectors_per_cluster;
        int first_sector = first_sector_of_cluster(cluster);

        for (int i = 0; i < n; i++)
            sectors.push_back(first_sector + i);

    }
    return sectors;

}