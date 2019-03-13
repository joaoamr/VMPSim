#include <VmPlacement/VMSchedulerRR.h>
#include <VmPlacement/VMScheduler.h>
#include <iostream>


VMSchedulerRR::VMSchedulerRR(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph *pmsTopology, Graph *vmsTraffic): VMScheduler(vmlist, pmlist, pmsTopology, vmsTraffic){
    last = 0;
}

VMSchedulerRR::~VMSchedulerRR(){

}

void VMSchedulerRR::run(){
    std::cout << "Starting RR:\n";
    int alloc = 0;
    CloudMachine *host;
    int first;
    List<VirtualMachine> *vms = VMScheduler::getVms();
    List<CloudMachine> *pms = VMScheduler::getPms();

    for(int i = 0; i < vms->size(); i++){
        first = last;
        host = pms->get(last);
        while(!host->fit(vms->get(i))){
            last++;
            if(last == pms->size())
                last = 0;

            if(last == first){
                host = NULL;
                break;
            }

            host = pms->get(last);
        }
        if(host != NULL){
            vms->get(i)->setHost(host);
            host->addVm(vms->get(i));
            alloc++;
        }
        last++;
        if(last == pms->size())
            last = 0;
    }
}
