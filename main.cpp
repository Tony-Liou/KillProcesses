#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <tchar.h>
#include <windows.h>
#include <psapi.h>

std::tuple<unsigned long, std::string> Get64bitProcessIDAndName(DWORD processID) {
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (nullptr != hProcess) {
        HMODULE hMod;
        const auto moduleSize = sizeof(hMod);
        DWORD cbNeeded;

        if (EnumProcessModulesEx(hProcess, &hMod, moduleSize, &cbNeeded, LIST_MODULES_ALL)) {
            GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
        }

        if (cbNeeded > moduleSize) {
            std::unique_ptr<HMODULE[]> biggerModule(new HMODULE[cbNeeded / moduleSize]);
            if (EnumProcessModulesEx(hProcess, biggerModule.get(), sizeof(biggerModule), &cbNeeded, LIST_MODULES_ALL)) {
                if (!GetModuleBaseName(hProcess, *biggerModule.get(), szProcessName,
                                       sizeof(szProcessName) / sizeof(TCHAR))) {
                    std::cout << "Get process name failed.\n";
                }
            }
        }
    }
    CloseHandle(hProcess);

    return {processID, szProcessName};
}

void KillProcess(const DWORD processID) {
    const auto hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
    TerminateProcess(hProcess, 1);
    CloseHandle(hProcess);
}

int main(int argc, char *argv[]) {
    std::string targetName;
    if (argc > 1) {
        targetName = argv[1];
    } else {
        std::cout << "Enter the target name: ";
        std::cin >> targetName;
    }

    std::cout << "Searching [" << targetName << "]\n";

    DWORD aProcesses[1 << 12], cbNeeded, cProcesses;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return 1;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    std::cout << "Total process count: " << cProcesses << '\n';

    for (unsigned int i = 0; i < cProcesses; ++i) {
        if (aProcesses[i] != 0) {
            const auto [pid, pName] = Get64bitProcessIDAndName(aProcesses[i]);
            std::cout << pid << ", " << pName << '\n';
            if (pName.contains(targetName)) {
                std::cout << "* Found a process: " << pName << '\n';
                KillProcess(pid);
                std::cout << "Process terminated\n";
            }
        }
    }

    return 0;
}
