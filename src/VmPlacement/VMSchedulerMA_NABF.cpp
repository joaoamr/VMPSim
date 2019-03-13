#include<VmPlacement/VMSchedulerMA_NABF.h>
#include<VmPlacement/VMSchedulerNABF.h>
#include<VmPlacement/VMScheduler.h>
#include<VmPlacement/VMSchedulerKLFlat.h>
#include <iostream>


VMSchedulerMA_NABF::VMSchedulerMA_NABF(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph* pmsTopology, Graph* vmsTraffic): VMScheduler(vmlist, pmlist, pmsTopology, vmsTraffic){


}

VMSchedulerMA_NABF::~VMSchedulerMA_NABF(){

}

int VMSchedulerMA_NABF::setK(){

    for(int i = 0 ; i < vms->size(); i++){
        for(int j = 0; j < pms->size(); j++){
            if(pms->get(j)->fit(vms->get(i))){
                vms->get(i)->setHost(pms->get(j));
                pms->get(j)->addVm(vms->get(i));
                break;
            }
        }
    }

    int k = 0;

    for(int i = 0 ; i < pms->size(); i++)
        if(pms->get(i)->getVms()->size() > 0)
            k++;

    for(int i = 0; i < vms->size(); i++){
        if(vms->get(i)->isAlloc())
            vms->get(i)->getHost()->removeVm(vms->get(i));
    }

    return k;
}

void VMSchedulerMA_NABF::run(){
    List<VirtualMachine> *tempvms = vms->copyStatic();
    List<VirtualMachine> *relloc = new List<VirtualMachine>(vms->size());
    List<CloudMachine> *total = pms->copyStatic();
    List<CloudMachine>* targets = new List<CloudMachine>(pms->size());

    int n = vms->size();
    //int n = (16/2)*(16/2);


    n = setK();
    std::cout << "n = " << n;
    //n = total->size();
    for(int i = 0; i < n && total->size() > 0; i++){
        targets->add(total->remove(0));
    }

    VMSchedulerNABF* vmsc = new VMSchedulerNABF(tempvms, targets, pmsgraph, vmsgraph);

    vmsc->run();
    delete vmsc;
    for(int i = 0; i < tempvms->size(); i++){
        if(tempvms->get(i)->isAlloc()){
            tempvms->remove(i);
            i--;
        }
    }
    while(targets->size() > 0)
        targets->remove(0);

    for(int i = 0; i < pms->size(); i++)
        for(int j = 0; j < pms->get(i)->getVms()->size(); j++)
            pms->get(i)->getVms()->get(j)->setHost(pms->get(i));

    for(int i = 0; i < vms->size(); i++)
        if(!vms->get(i)->isAlloc())
            relloc->add(vms->get(i));


    std::cout << "\n " << activesPms() << " " << relloc->size() << "\n";

    retryNA(relloc);

    //VMSchedulerKLFlat klsort(vms, pms, pmsgraph, vmsgraph);
    //klsort.run();


    delete tempvms;
    delete targets;
    delete total;
    delete relloc;
}

void VMSchedulerMA_NABF::retryNA(List<VirtualMachine>* tempvms){
    VirtualMachine* vm;
    CloudMachine* pm;
    CloudMachine* target;
    double cost;
    double mincost;
    List<Vertex>* adj;

    List<CloudMachine>* fullpms = pms->copyStatic();

    for(int i = 0; i < tempvms->size(); i++){
        vm = (VirtualMachine*)tempvms->get(i);
        target = NULL;
        mincost = -1;

        for(int j = 0; j < fullpms->size(); j++){
            cost = 0;
            pm = fullpms->get(j);
            if(pm->fit(vm)){
                adj = vmsgraph->getVertexAdj(vm->getId());
                for(int k = 0; k < adj->size(); k++){
                    if(((VirtualMachine*)adj->get(k))->isAlloc())
                        cost += vmsgraph->getEdge(vm->getId(), adj->get(k)->getId()) * pmsgraph->getEdge(pm->getId(), ((VirtualMachine*)adj->get(k))->getHost()->getId());
                }
                delete adj;

                cost = pm->getMem()*pm->getHd()*pm->getPower() - vm->getMem()*vm->getHd()*vm->getPower();

                if(mincost == -1){
                    target = pm;
                    mincost = cost;
                }else{
                    if(mincost > cost && pm->getVms()->size() > 0){
                        target = pm;
                        mincost = cost;
                    }else{
                        if(pm->getVms()->size() > 0 && target->getVms()->size() == 0){
                            target = pm;
                            mincost = cost;
                        }
                    }
                }
            }
        }

        if(target == NULL)
            continue;

        target->addVm(vm);
        vm->setHost(target);
        tempvms->remove(i);
        i--;
    }

}
