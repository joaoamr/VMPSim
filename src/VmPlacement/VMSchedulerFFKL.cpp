#include <VmPlacement/VMSchedulerFFKL.h>
#include <VmPlacement/VMSchedulerKLFlat.h>
#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <iostream>


VMSchedulerFFKL::VMSchedulerFFKL(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph* pmsTopology, Graph* vmsTraffic): VMScheduler(vmlist, pmlist, pmsTopology, vmsTraffic){

}

VMSchedulerFFKL::~VMSchedulerFFKL(){

}

void VMSchedulerFFKL::volumeSort(){
    List<CloudMachine>* sort = pms->copyStatic();
    pms->clear();

    while(sort->size() > 0){
        int index = 0;
        int vol = sort->get(0)->getHd() * sort->get(0)->getMem() * sort->get(0)->getPower();
        for(int i = 1; i < sort->size(); i++){
            if(vol < sort->get(i)->getHd() * sort->get(i)->getMem() * sort->get(i)->getPower()){
                index = i;
                vol = sort->get(i)->getHd() * sort->get(i)->getMem() * sort->get(i)->getPower();
            }
        }
        pms->add(sort->remove(index));

    }
    delete sort;
}

void VMSchedulerFFKL::networkSort(List<CloudMachine>* pms){
    if(pms->size() == 0)
        return;

    List<CloudMachine>* sorted = pms->copyStatic();
    pms->clear();
    int minhop, minIndex;
    CloudMachine* pm = sorted->remove(0);
    pms->add(pm);

    while(sorted->size() > 0){
        minhop = pmsgraph->getEdge(pm->getId(), sorted->get(0)->getId());
        minIndex = 0;

        for(int i = 1; i < sorted->size(); i++){
            if(pmsgraph->getEdge(pm->getId(), sorted->get(i)->getId()) < minhop){
                minhop = pmsgraph->getEdge(pm->getId(), sorted->get(i)->getId());
                minIndex = i;
            }
        }
        pm = sorted->remove(minIndex);
        pms->add(pm);

    }

    delete sorted;
}

void VMSchedulerFFKL::run(){
    List<VirtualMachine>* vms = VMScheduler::getVms();
    List<CloudMachine>* pms = VMScheduler::getPms();

    int n = setK();
    List<CloudMachine>* targets = new List<CloudMachine>(pms->size());
    List<CloudMachine>* rest = new List<CloudMachine>(pms->size());


    for(int i = 0; i < n; i++)
        targets->add(pms->get(i));

    for(int i = n; i < pms->size(); i++)
        rest->add(pms->get(i));

    inactivesHosts = targets->copyStatic();
    activesHosts = new List<CloudMachine>(pms->size());

    sortClusters();

    for(int i = 0 ; i < clusters->size(); i++){
        List<VirtualMachine>* cluster = clusters->get(i);
        List<CloudMachine>* selectedHosts = new List<CloudMachine>(pms->size());
        targets->clear();

        for(int i = 0 ; i < inactivesHosts->size(); i++)
            targets->add(inactivesHosts->get(i));

        for(int i = 0 ; i < activesHosts->size(); i++)
            targets->add(activesHosts->get(i));

        for(int i = 0 ; i < rest->size(); i++)
            targets->add(rest->get(i));

        if(cluster->size() > 0){
            std::cout << cluster->size() << "\n";
            for(int i = 0 ; i < cluster->size(); i++){
                for(int j = 0; j < targets->size(); j++){
                    if(targets->get(j)->fit(cluster->get(i))){
                        cluster->get(i)->setHost(targets->get(j));
                        placeVm(cluster->get(i), targets->get(j));
                        selectedHosts->add(targets->get(j));
                        break;
                    }
                }
            }
            if(selectedHosts->size() > 1){
                VMSchedulerKLFlat klsort(cluster, selectedHosts, pmsgraph, vmsgraph);
                klsort.run();
            }
            delete selectedHosts;

        }else{
            bfApm(cluster->get(0));
        }
    }

}

void VMSchedulerFFKL::bfApm(VirtualMachine *vm){
    CloudMachine* selectedhost;

    selectedhost = bestFit(vm, activesHosts);

    if(selectedhost != NULL){
        placeVm(vm, selectedhost);
        return;
    }

    selectedhost = firstFitDecreasing(vm, inactivesHosts);
    if(selectedhost != NULL){
        placeVm(vm, selectedhost);
    }
}

CloudMachine *VMSchedulerFFKL::firstFitDecreasing(VirtualMachine* vm, List<CloudMachine> *target){
    int d;
    bool dset = false;
    CloudMachine *pm, *selectedhost = NULL;

    for(int i = 0 ; i < target->size(); i++){
        pm = target->get(i);
        if(pm->fit(vm)){
            if(!dset){
                d = pm->getMem()*pm->getPower() - vm->getMem()*vm->getHd()*vm->getPower();
                selectedhost = pm;
                dset = true;
                continue;
            }

            if(pm->getMem()*pm->getPower() - vm->getMem()*vm->getPower() > d){
                d = pm->getMem()*pm->getPower() - vm->getMem()*vm->getPower();
                selectedhost = pm;
            }

        }
    }

    return selectedhost;
}

CloudMachine *VMSchedulerFFKL::bestFit(VirtualMachine* vm, List<CloudMachine>* target){
    int d;
    bool dset = false;
    CloudMachine *pm, *selectedhost = NULL;

    for(int i = 0 ; i < target->size(); i++){
        pm = target->get(i);
        if(pm->fit(vm)){
            if(!dset){
                d = pm->getMem()*pm->getPower() - vm->getMem()*vm->getPower();
                selectedhost = pm;
                dset = true;
                continue;
            }

            if(pm->getMem()*pm->getPower() - vm->getMem()*vm->getPower() < d){
                d = pm->getMem()*pm->getPower() - vm->getMem()*vm->getPower();
                selectedhost = pm;
            }

        }
    }

    return selectedhost;
}

void VMSchedulerFFKL::placeVm(VirtualMachine* vm, CloudMachine* pm){
    if(pm->getVms()->size() == 0){
        activesHosts->add(pm);
        inactivesHosts->remove(pm);
    }

    pm->addVm(vm);
    vm->setHost(pm);
}

int VMSchedulerFFKL::setK(){

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

