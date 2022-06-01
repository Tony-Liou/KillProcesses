#include <iostream>
#include <cstdio>
#include <tchar.h>
#include <windows.h>
#include <psapi.h>

void Print64bitProcessAndID(DWORD processID) {
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // Get a handle to the process.
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

    // Get the process name.
    if (nullptr != hProcess) {
        HMODULE hMod;
        auto moduleSize = sizeof(hMod);
        DWORD cbNeeded;

        if (EnumProcessModulesEx(hProcess, &hMod, moduleSize, &cbNeeded, LIST_MODULES_ALL)) {
            GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
        }

        if (cbNeeded > moduleSize) {
            //std::cout << "hModule is too small. Obtained: " << moduleSize << ", needed: " << cbNeeded << std::endl;

            auto *biggerModule = new HMODULE[cbNeeded / moduleSize];
            if (EnumProcessModulesEx(hProcess, biggerModule, sizeof(biggerModule), &cbNeeded, LIST_MODULES_ALL)) {
                GetModuleBaseName(hProcess, *biggerModule, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
            }

            delete[] biggerModule;
        }
    }

    // Print the process name and identifier.
    _tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);

    // Release the handle to the process.
    CloseHandle(hProcess);
}

int main() {
    // Get the list of process identifiers.
    DWORD aProcesses[1024], cbNeeded, cProcesses;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return 1;
    }


    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.
    for (unsigned int i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            Print64bitProcessAndID(aProcesses[i]);
        }
    }

    return 0;
}
