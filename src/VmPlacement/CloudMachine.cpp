#include "VmPlacement/CloudMachine.h"
#include "VmPlacement/VirtualMachine.h"
#include "Vertex.h"
#include "listlib.h"
#include <iostream>

CloudMachine::CloudMachine(int mid, int memory, int disk, int mips) : Vertex(mid)
{
    vms = new List<VirtualMachine>(2000);
    mem = memory;
    hd = disk;
    id = mid;
    power = mips;
}

CloudMachine::~CloudMachine()
{

}


void CloudMachine::addVm(VirtualMachine* vm)
{
    vms->add(vm);
    mem -= vm->getMem();
    //hd -= vm->getHd();
    power -= vm->getPower();
    vm->setAlloc(true);
    vm->setHost(this);
}

bool CloudMachine::fit(VirtualMachine* vm){
    if(mem >= vm->getMem())
        //if(hd >= vm->getHd())
            if(power >= vm->getPower())
                return true;

    return false;
}

void CloudMachine::clear(){
    while(vms->size() > 0){
        mem += vms->get(0)->getMem();
        hd += vms->get(0)->getHd();
        power += vms->get(0)->getPower();
        vms->get(0)->setAlloc(false);
        //vms->get(0)->setHost(NULL);
        vms->remove(0);
    }
}

void CloudMachine::removeVm(VirtualMachine* vm){
    int pos = vms->contains(vm);
    mem += vms->get(pos)->getMem();
    hd += vms->get(pos)->getHd();
    power += vms->get(pos)->getPower();
    //vms->get(pos)->setHost(NULL);
    vms->remove(pos)->setAlloc(false);
}
