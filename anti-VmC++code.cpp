#include <iostream>
#include <windows.h>
#include <iphlpapi.h>
#include <string>
#include <vector>
#include <intrin.h>       // For __cpuid function
#pragma comment(lib, "Iphlpapi.lib")
bool CheckForVirtualCPU() {
    int CPUInfo[4] = { 0 };
    __cpuid(CPUInfo, 1);
    // Check hypervisor presence (bit 31 of ECX).
    return (CPUInfo[2] & (1 << 31)) != 0;
}

bool CheckForVirtualBIOS() {
    char buffer[256];
    DWORD bufferSize = sizeof(buffer);
    LSTATUS result = RegGetValueA(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DESCRIPTION\\System\\BIOS",
        "SystemManufacturer",
        RRF_RT_REG_SZ,
        nullptr,
        buffer,
        &bufferSize);

    if (result == ERROR_SUCCESS) {
        std::string manufacturer(buffer);
        return manufacturer.find("VMware") != std::string::npos ||
            manufacturer.find("VirtualBox") != std::string::npos ||
            manufacturer.find("QEMU") != std::string::npos ||
            manufacturer.find("Microsoft Corporation") != std::string::npos ||  // For Hyper-V
            manufacturer.find("Parallels") != std::string::npos;
    }
    return false;
}

bool CheckForVirtualDisk() {
    DWORD bufferSize = 0;
    LSTATUS result = RegGetValueA(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Services\\disk\\Enum",
        "0",
        RRF_RT_REG_SZ,
        nullptr,
        nullptr,
        &bufferSize);

    if (result == ERROR_MORE_DATA) {
        std::vector<char> buffer(bufferSize);
        result = RegGetValueA(HKEY_LOCAL_MACHINE,
            "SYSTEM\\CurrentControlSet\\Services\\disk\\Enum",
            "0",
            RRF_RT_REG_SZ,
            nullptr,
            buffer.data(),
            &bufferSize);
        if (result == ERROR_SUCCESS) {
            std::string diskInfo(buffer.data());
            return diskInfo.find("VMware") != std::string::npos ||
                diskInfo.find("VBOX") != std::string::npos ||
                diskInfo.find("QEMU") != std::string::npos;
        }
    }
    return false;
}

bool CheckEnvironmentVariables() {
    const char* vmEnvVars[] = { "VBOX", "VMWARE", "PARALLELS" };
    for (const char* var : vmEnvVars) {
        char* value = nullptr;
        size_t valueSize = 0;

        // Use _dupenv_s to safely get the environment variable
        if (_dupenv_s(&value, &valueSize, var) == 0 && value != nullptr) {
            free(value); // Free the allocated memory
            return true;
        }
    }
    return false;
}

bool CheckMACAddress() {
    IP_ADAPTER_INFO AdapterInfo[16];
    DWORD dwBufLen = sizeof(AdapterInfo);

    if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
        PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
        while (pAdapterInfo) {
            if ((pAdapterInfo->Address[0] == 0x00 && pAdapterInfo->Address[1] == 0x05 && pAdapterInfo->Address[2] == 0x69) || // VMware
                (pAdapterInfo->Address[0] == 0x08 && pAdapterInfo->Address[1] == 0x00 && pAdapterInfo->Address[2] == 0x27)) { // VirtualBox
                return true;
            }
            pAdapterInfo = pAdapterInfo->Next;
        }
    }
    return false;
}

bool CheckVMProcesses() {
    const std::string vmProcesses[] = { "vmware.exe", "vboxservice.exe", "qemu-ga.exe" };

    for (const std::string& processName : vmProcesses) {
        if (system(("tasklist | findstr " + processName).c_str()) == 0) {
            return true;
        }
    }
    return false;
}
//CheckForVirtualCPU()
bool IsRunningOnVM() {
    return  CheckForVirtualBIOS() || CheckForVirtualDisk() ||
        CheckEnvironmentVariables() || CheckMACAddress() || CheckVMProcesses();
}

void CheckForHypervisorsAndVMs() {
    std::cout << "Checking for hypervisors and existing VMs...\n";

    if (CheckForVirtualCPU()) {
        std::cout << "Hypervisor detected via CPUID." << std::endl;
    }

    if (CheckForVirtualBIOS()) {
        std::cout << "Virtual BIOS detected." << std::endl;
    }

    if (CheckForVirtualDisk()) {
        std::cout << "Virtual Disk detected." << std::endl;
    }

    if (CheckEnvironmentVariables()) {
        std::cout << "Environment variable indicating VM detected." << std::endl;
    }

    if (CheckMACAddress()) {
        std::cout << "VM MAC address detected." << std::endl;
    }

    if (CheckVMProcesses()) {
        std::cout << "VM process detected." << std::endl;
    }
}

int main() {
    if (IsRunningOnVM()) {
        std::cout << "Running inside a Virtual Machine!" << std::endl;
    }
    else {
        std::cout << "Running on the Host Machine. Checking for hypervisors and existing VMs...\n";
        CheckForHypervisorsAndVMs();
    }

    return 0;
}
