# Virtual Machine and Hypervisor Detection

This project contains code that detects whether it's running inside a Virtual Machine (VM) or on a physical host machine. If running on a host, it further checks for the presence of hypervisors. This code uses various techniques to determine if it’s in a virtualized environment, leveraging aspects like CPUID, BIOS information, disk enumeration, environment variables, MAC addresses, and running processes.

## Overview

The code is written in C++ and relies on Windows-specific APIs to inspect various system attributes. Here’s a breakdown of what each function does:

- **CheckForVirtualCPU**: Uses the CPUID instruction to detect if a hypervisor is present. This checks the 31st bit of the `ECX` register to see if a hypervisor is detected, which is an indicator of virtualization.

- **CheckForVirtualBIOS**: Inspects the system BIOS manufacturer string in the registry to see if it contains keywords associated with virtual environments, such as "VMware," "VirtualBox," "QEMU," "Microsoft Corporation" (for Hyper-V), and "Parallels."

- **CheckForVirtualDisk**: Examines registry entries related to disk hardware to look for strings that suggest a virtual disk, like "VMware," "VBOX," and "QEMU."

- **CheckEnvironmentVariables**: Checks environment variables for values commonly set by virtual environments, such as "VBOX" or "VMWARE".

- **CheckMACAddress**: Queries the system’s network adapters to inspect MAC addresses that are commonly associated with virtual machines, like those starting with `00:05:69` (VMware) or `08:00:27` (VirtualBox).

- **CheckVMProcesses**: Looks for specific processes that are usually associated with virtual machines, such as `vmware.exe`, `vboxservice.exe`, and `qemu-ga.exe`.

## Usage

The program first determines if it’s running inside a VM. If it detects that it’s on a host machine, it checks for hypervisor indicators.

### Example Output

The output of this program will indicate whether it’s running inside a virtual machine, or if it detects the presence of a hypervisor on a host system. Example outputs may include:

- `"Running inside a Virtual Machine!"`
- `"Running on the Host Machine. Checking for hypervisors and existing VMs..."`

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
