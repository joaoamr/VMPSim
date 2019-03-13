#include "VmPlacement/VirtualMachine.h"

VirtualMachine::VirtualMachine(int vid, int memory, int disk, int mips): Vertex(vid, memory, disk, mips, false)
{
    mem = memory;
    hd = disk;
    alloc = false;
    power = mips;

}

VirtualMachine::VirtualMachine(int vid, int memory, int disk, int mips, char* lbl): Vertex(vid, memory, disk, mips, false)
{
    mem = memory;
    hd = disk;
    alloc = false;
    label = lbl;
    power = mips;
}

VirtualMachine::VirtualMachine(int vid): Vertex(vid, true)
{
    mem = 0;
    hd = 0;
    power = 0;
}

VirtualMachine::~VirtualMachine()
{

}

void VirtualMachine::setHost(CloudMachine* mac) {
    host = mac;
    alloc = true;
};

