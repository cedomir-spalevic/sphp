/**
 * Command Line App that converts PHP version
 * 
 * @author Cedomir Spalevic
 */
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_PATH_LENGTH 32760
#define PHP_56_PATH ""
#define PHP_70_PATH ""
#define PHP_71_PATH ""
#define PHP_72_PATH ""
#define PHP_73_PATH ""

using namespace std;

void displayHelp() {
    cout << "\nUsage: sphp [VERSION]\n" << endl;
    cout << "Versions:" << endl;
    cout << "\t56\t\tChange to PHP version 5.6" << endl;
    cout << "\t70\t\tChange to PHP version 7.0" << endl;
    cout << "\t71\t\tChange to PHP version 7.1" << endl;
    cout << "\t72\t\tChange to PHP version 7.2" << endl;
    cout << "\t73\t\tChange to PHP version 7.3\n" << endl;
}

int findSubstring(const char* source, int sourceLength, const char* sub, int subLength) {
    char builder[subLength] = {};
    int foundIndex = -1;
    for(int i = 0; i < sourceLength; i++) {
        int temp = (foundIndex == -1 ? 0 : (i - foundIndex));
        if(source[i] == sub[temp]) {
            if(temp == (subLength-1)) break;
            else if(foundIndex == -1) foundIndex = i;
        }
        else foundIndex = -1;
    }
    return foundIndex;
}

char* doubleArray(char* arr, int length, int cap) {
    int newCap = cap*2;
    char* tmp = new char[newCap] {'\0'};
    strcmp(tmp, arr);
    delete [] arr;
    arr = NULL;
    return tmp;
}

char* getCurrentDir() {
    char buffer[128] = {'\0'};
    char* result = new char[128] {'\0'};

    // '2>&1' is to get stdout and stderr
    auto pipe = _popen("WHERE PHP 2>&1", "r");
    if(!pipe) {
        cout << "popen() failed!" << endl;
        delete [] result;
        result = NULL;
        exit(EXIT_FAILURE);
    }

    while(!feof(pipe)) {
        auto n = fgets(buffer, sizeof(buffer), pipe);
        if(n != nullptr) {
            if((strlen(result) + strlen(buffer)) > sizeof(result)) result = doubleArray(result, strlen(result), sizeof(result));
            strcat(result, buffer);
        }
    }

    auto rc = _pclose(pipe);
    if(rc != 0) {
        delete [] result;
        result = NULL;
    }

    return result;
}

void accessReg(const char* currentDir, int currentDirLength, const char* newDir, int newDirLength) {
    //Open the Registry
    HKEY hKey = 0;
    if(RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Environment"), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
        //Get PATH
        DWORD lpType = REG_EXPAND_SZ;
        TCHAR lpData[MAX_PATH_LENGTH] = {'\0'};
        DWORD lpcbData = sizeof(lpData);
        auto n = RegQueryValueEx(hKey, TEXT("Path"), 0, &lpType, (LPBYTE) &lpData, &lpcbData);
        if(n != ERROR_SUCCESS) {
            cout << "Registry failed to query value with error " << n << endl;
            exit(EXIT_FAILURE);
        }

        //Get variables
        int currentPathLength = strlen(lpData);

        //Make sure that adding new path does not exceed PATH character limit
        if( (currentDirLength == -1 && (currentPathLength + newDirLength) > MAX_PATH_LENGTH) ||
            (currentDirLength != -1 && (currentPathLength - currentDirLength + newDirLength) > MAX_PATH_LENGTH)) {
                cout << "Changing the PATH variable will exceed the character limit allowed for this Registry value " << endl;
                exit(EXIT_FAILURE);
        }

        //Change PATH
        TCHAR newLpData[MAX_PATH_LENGTH];
        if(currentDirLength == -1) {
            strcpy(newLpData, lpData);
            strcat(newLpData, newDir);
        }
        else {
            int indexOf = findSubstring(lpData, currentPathLength, currentDir, currentDirLength);
            int subLength = currentPathLength - currentDirLength;

            for(int i = 0; i < indexOf; i++) newLpData[i] = lpData[i];
            for(int i = indexOf; i < subLength; i++) newLpData[i] = lpData[i + currentDirLength];
            strcat(newLpData, newDir);
        }

        //Update PATH
        n = RegSetValueEx(hKey, TEXT("Path"), 0, lpType, (BYTE*) &newLpData, strlen(newLpData));
        if(n != ERROR_SUCCESS) {
            cout << "Registry failed to update value with error " << n << endl;
            exit(EXIT_FAILURE);
        }

        //Broadcast Setting Change Message
        SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) "Environment");

        //Create temp file to update current cmd process with the changes
        //TODO: Try to update current cmd process to have the changes
        // char* temp = strcat(getenv("TEMP"), "\\t.cmd");
        // ofstream tempFile(temp);
        // if(currentDirLength == -1) tempFile << "SET PATH=\"%PATH:" << currentDir << "=%\"" << endl;
        // tempFile << "SET PATH=%PATH%;" << newDir << endl;
        // tempFile.close();
        // system("CALL %TEMP%\\t.cmd");
        // system("DEL %TEMP%\\t.cmd");

        //Close Registry
        RegCloseKey(hKey);

        cout << "Done!" << endl;
    }
    else {
        cout << "Registry failed to open with error " << hKey << endl;
        exit(EXIT_FAILURE);
    }
}

int switchVersion(const char* version, const char* newDir) {
    cout << "Switching to PHP " << version << "...\n" << endl;

    //Find where PHP is currently pointing to
    char* currentDir = getCurrentDir();
    int currentDirLength = -1, newDirLength = strlen(newDir);
    if(currentDir != NULL) {
        currentDirLength = strlen(currentDir);
        int eol = currentDirLength - 9;
        for(int i = currentDirLength; i > eol; i--) currentDir[i] = '\0';
        currentDir[eol] = ';';
        currentDirLength = eol + 1;
    }

    //Edit Registry
    accessReg(currentDir, currentDirLength, newDir, newDirLength);

    //Free memory - if needed
    if(currentDir != NULL) {
        delete [] currentDir;
        currentDir = NULL;
    }
}

int main(int argc, char* argv[]) {

    if(argc == 1) {
        displayHelp();
        return 0;
    }

    if(!strcmp(argv[1], "56")) switchVersion("5.6", PHP_56_PATH);
    else if(!strcmp(argv[1], "70")) switchVersion("7.0", PHP_70_PATH);
    else if(!strcmp(argv[1], "71")) switchVersion("7.1", PHP_71_PATH);
    else if(!strcmp(argv[1], "72")) switchVersion("7.2", PHP_72_PATH);
    else if(!strcmp(argv[1], "73")) switchVersion("7.3", PHP_73_PATH);
    else displayHelp();

    return 0;
}