#include <VmPlacement/VMSchedulerBFDM.h>
#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <iostream>


VMSchedulerBFDM::VMSchedulerBFDM(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph* pmsTopology, Graph* vmsTraffic): VMScheduler(vmlist, pmlist, pmsTopology, vmsTraffic){

}

VMSchedulerBFDM::~VMSchedulerBFDM(){

}

void VMSchedulerBFDM::run(){
    int d;
    bool dset;
    CloudMachine *pm, *selectedhost = NULL;
    VirtualMachine *vm;
    List<VirtualMachine> *vmsc = vms->copyStatic();

    for(int j = 0; j < vmsc->size(); j++){
        selectedhost = NULL;
        vm = vmsc->get(j);
        dset = false;

        for(int i = 0 ; i < pms->size(); i++){
            pm = pms->get(i);
            if(pm->fit(vm) && pm->getVms()->size() > 0){
                if(!dset){
                    d = pm->getMem()*pm->getHd() - vm->getMem()*vm->getHd();
                    selectedhost = pm;
                    dset = true;
                    continue;
                }

                if(pm->getMem()*pm->getHd() - vm->getMem()*vm->getHd() < d){
                    d = pm->getMem()*pm->getHd() - vm->getMem()*vm->getHd();
                    selectedhost = pm;
                }

            }
        }
        if(selectedhost == NULL){
            int maxvol = pms->get(0)->getMem()*pms->get(0)->getHd();
            int index = 0;
            for(int i = 1 ; i < pms->size(); i++){
                if(maxvol < pms->get(i)->getMem()*pms->get(i)->getHd()){
                    maxvol = pms->get(i)->getMem()*pms->get(i)->getHd();
                    index = i;
                }
            }
            selectedhost = pms->get(index);
        }

        vm->setHost(selectedhost);
        selectedhost->addVm(vm);
    }

    delete vmsc;
}
