#include <VmPlacement/VMSchedulerFF.h>
#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <iostream>


VMSchedulerFF::VMSchedulerFF(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph* pmsTopology, Graph* vmsTraffic): VMScheduler(vmlist, pmlist, pmsTopology, vmsTraffic){

}

VMSchedulerFF::~VMSchedulerFF(){

}

void VMSchedulerFF::run(){
    List<VirtualMachine>* vms = VMScheduler::getVms();
    List<CloudMachine>* pms = VMScheduler::getPms();

    for(int i = 0 ; i < vms->size(); i++){
        for(int j = 0; j < pms->size(); j++){
            if(pms->get(j)->fit(vms->get(i))){
                vms->get(i)->setHost(pms->get(j));
                pms->get(j)->addVm(vms->get(i));
                break;
            }
        }
    }
}
