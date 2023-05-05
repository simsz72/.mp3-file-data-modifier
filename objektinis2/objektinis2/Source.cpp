#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <windows.h>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        return 1;
    }

    const char* filename = argv[1];

    HANDLE hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //funkcija skirta atidaryti failui.. "GENERIC_READ | GENERIC_WRITE" reiškia
    if (hFile == INVALID_HANDLE_VALUE)
    {
        cout << "Error opening file" << filename << endl;
        return 1;
    }

    DWORD fileSize = GetFileSize(hFile, NULL); //tikrinama ar failas siekia 128, jei ne, metamas klaidos pranešimas, kadangi vadinasi, jog failas neturi ID3 TAG
    if (fileSize < 128)
    {
        cout << "File is too small to contain ID3 tags" << endl;
        CloseHandle(hFile);
        return 1;
    }

    unique_ptr<char[]> lpBuffer(new char[fileSize]); //sukuriamas unikalus dinaminis char masyvas su fileSize dydžiu. unique_ptr reiškia, jog pointerio valdomas objektas bus pašalintas, kai pointeris bus sunaikintas, ar bus priskirtas kitam objektui.

    DWORD bytesRead;
    if (!ReadFile(hFile, lpBuffer.get(), fileSize, &bytesRead, NULL))
    {
        cout << "Error reading file" << endl;
        CloseHandle(hFile);
        return 1;
    }

    char* buffer = lpBuffer.get();
    if (buffer[fileSize - 128] == 'T' && buffer[fileSize - 127] == 'A' && buffer[fileSize - 126] == 'G') //ieškoma TAG'o pirmuose failo bituose
    {
        cout << "Current title: ";
        for (int i = 3; i < 33; i++)
        {   
            cout << buffer[fileSize - 128 + i]; //perskaitomi dainos duomenys atitinkamuose bituose
        }
        cout << endl << "Current artist: ";
        for (int i = 33; i < 63; i++)
        {
            cout << buffer[fileSize - 128 + i]; //perskaitomi dainos duomenys atitinkamuose bituose
        }
        cout << endl << "Current album: ";
        for (int i = 63; i < 93; i++)
        {
            cout << buffer[fileSize - 128 + i]; //perskaitomi dainos duomenys atitinkamuose bituose
        }
        int choice; //pasirinkimas
        int byteStart; //priklausomai nuo pasirinkimo, laikomas skaičius, nuo kurių bitų reiks keisti duomenis
        int byteEndsAt; //priklausomai nuo pasirinkimo, laikomas skaičius, iki kurių bitų reiks keisti duomenis
        string temp;
        cout << "\n1 - Change data\n0 - Exit\n";
        cin >> choice;

        while (choice != 0)
        {
            cout << "\n1 - Change song name\n2 - Change artist\n3 - Change album\n0 - Exit\n";
            cin >> choice;
            switch (choice) {
            case 1:
                byteStart = 3;
                byteEndsAt = 33;
                cout << "Enter song name: ";
                break;

            case 2:
                byteStart = 33;
                byteEndsAt = 63;
                cout << "Enter artist: ";
                break;

            case 3:
                byteStart = 63;
                byteEndsAt = 93;
                cout << "Enter album name: ";
                break;

            default:
                break;
            }
            if (choice != 0)
            {
                getline(cin >> ws, temp); //ivedami norimi pakeisti duomenys
                for (int i = 0; i < temp.size(); i++) //duomenys yra pakeičiami įvestais duomenimis
                {
                    buffer[fileSize - 128 + byteStart] = temp.at(i);
                    byteStart++;
                }
                for (int i = byteStart + temp.size(); i < byteEndsAt; i++) //like bitai užpildomi tarpais
                {
                    buffer[fileSize - 128 + i] = ' ';
                }
            }
        }
        DWORD dwPos = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        if (dwPos == INVALID_SET_FILE_POINTER) {
            cout << "Error setting file pointer" << endl;
            CloseHandle(hFile);
            return 1;
        }
        DWORD bytesWritten;
        if (!WriteFile(hFile, lpBuffer.get(), fileSize, &bytesWritten, NULL))
        {
            cout << "Error writing file" << endl;
            CloseHandle(hFile);
            return 1;
        }
    }
    else
    {
        cout << "File does not contain ID3 tag" << endl; //jei pirmi trys bitai nėra "TAG", failas TAG neturi
    }
    CloseHandle(hFile); //uždaromas handle

    return 0;
}