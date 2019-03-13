#include <VmPlacement/VMSchedulerFFD.h>
#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <iostream>


VMSchedulerFFD::VMSchedulerFFD(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph* pmsTopology, Graph* vmsTraffic): VMScheduler(vmlist, pmlist, pmsTopology, vmsTraffic){

}

VMSchedulerFFD::~VMSchedulerFFD(){

}

void VMSchedulerFFD::run(){
    List<VirtualMachine>* vms = VMScheduler::getVms();
    List<CloudMachine>* pms = VMScheduler::getPms();

    List<VirtualMachine>* sort = vms->copyStatic();
    vms->clear();

    while(sort->size() > 0){
        int index = 0;
        int vol = sort->get(0)->getHd() * sort->get(0)->getMem();
        for(int i = 1; i < sort->size(); i++){
            if(vol < sort->get(i)->getHd() * sort->get(i)->getMem()){
                index = i;
                vol = sort->get(i)->getHd() * sort->get(i)->getMem();
            }
        }
        vms->add(sort->remove(index));

    }
    delete sort;

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
