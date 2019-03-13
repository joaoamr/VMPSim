#include <VmPlacement/VMSchedulerKLFlat.h>
#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <VmPlacement/VMSchedulerNABF.h>
#include <VmPlacement/VMSchedulerMA_NABF.h>
#include <iostream>
#include <Partition.h>
#include <Vertex.h>
#include <Partitioning.h>
#include <KLMultCluster.h>
#include <random>


VMSchedulerKLFlat::VMSchedulerKLFlat(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph* pmsTopology, Graph* vmsTraffic): VMScheduler(vmlist, pmlist, pmsTopology, vmsTraffic){
    expanded = new List<CloudMachine>(pmsgraph->getNumberOfVertices());
}

VMSchedulerKLFlat::~VMSchedulerKLFlat(){
    delete expanded;
}

void VMSchedulerKLFlat::run(){
    expanded->clear();
    totalPlacement();
}

void VMSchedulerKLFlat::totalPlacement(){
    //sortClusters();

    tempvms = (List<Vertex>*)(VMScheduler::vms->copyStatic());

    if(mode == 0)
        performKLMC();

    if(mode == 1)
        performKL2C();

    fit();

    retryNA();

    delete tempvms;
}

void VMSchedulerKLFlat::divideAndConquerPlacement(){
    sortClusters();

    while(clusters->size() > 0){
        tempvms = (List<Vertex>*)clusters->remove(0);

        performKL2C();

        fit();

        retryNA();

        delete tempvms;
    }
}

double VMSchedulerKLFlat::computeTotalCost(){
    double cost = 0;
    Graph* vmsGraph = VMScheduler::getVmsGraph();
    Graph* pmsGraph = VMScheduler::getPmsGraph();


    for(int i = 0; i < tempvms->size(); i++)
        for(int j = i; j < tempvms->size(); j++){
            //std::cout << " edge: " << vmsGraph->getEdge(tempvms->get(i), tempvms->get(j))*pmsGraph->getEdge(tempvms->get(i)->getPartitionId(), tempvms->get(j)->getPartitionId());
            cost += vmsGraph->getEdge(tempvms->get(i), tempvms->get(j))*pmsGraph->getEdge(tempvms->get(i)->getPartitionId(), tempvms->get(j)->getPartitionId());
        }
    return cost;
}

double VMSchedulerKLFlat::computeAdditionalTotalCost(List<Vertex> *vmlist, List<Vertex> *addlist){
    double cost = 0;
    Graph* vmsGraph = VMScheduler::getVmsGraph();
    Graph* pmsGraph = VMScheduler::getPmsGraph();


    for(int i = 0; i < vmlist->size(); i++)
        for(int j = 0; j < addlist->size(); j++){
            cost += vmsGraph->getEdge(vmlist->get(i), addlist->get(j))*pmsGraph->getEdge(vmlist->get(i)->getPartitionId(), addlist->get(j)->getPartitionId());
        }

    return cost;
}

void VMSchedulerKLFlat::performKL2C(){
    int n = 0;
    for(int i = 0 ; i < pms->size(); i++){
        if(pms->get(i)->getId() > n)
            n = pms->get(i)->getId();
    }
    n++;

    plist = new List<Partition>(n);

    for(int i = 0; i < n; i++)
        plist->add(new Partition(i, tempvms->size()));

    int alloc = 0;

    //initialPartNABF();
    checkInitialPart();

    /*FILE* f = fopen("part", "w");
    for(int i = 0;  i < tempvms->size(); i++){
        fprintf(f,"%d ", tempvms->get(i)->getPartitionId());
    }
    fclose(f);*/
    CloudMachine *host;

    for(int i = 0; i < tempvms->size(); i++){
        ((VirtualMachine*)tempvms->get(i))->getHost()->removeVm((VirtualMachine*)tempvms->get(i));
    }

    double globalCost, oldCost, newCost;
    int rr = 0;
    double bestcost = -1;
    bool go = true;


    globalCost = computeTotalCost();

    std::cout << "\nTotal external traffic: " << globalCost << "\n\n";

    if(globalCost == 0)
        return;

    Partitioning* klPart = new Partitioning(VMScheduler::getVmsGraph(), tempvms, NULL, NULL);

    List<Partition> *tempplist = plist->copyStatic();
    double max = 0, prevcost, actualcost;
    Partition* maxp1, *maxp2, *p1, *p2, *lastp1 = NULL, *lastp2 = NULL;
    List<Vertex> *tempp1, *tempp2;
    int iId, jId;

    for(int i = 0; i < tempplist->size(); i++)
        if(tempplist->get(i)->size() == 0){
                tempplist->remove(i);
                i--;
        }

    double maxcost, d = 0, dlast = -1;


    //while(d + dlast != 0 && d >= 0){
        maxcost = computeTotalCost();
        dlast = d;
        for(int i = 0; i < tempplist->size() - 1; i++)
            for(int j = i + 1; j < tempplist->size(); j++){
                p1 = tempplist->get(i);
                p2 = tempplist->get(j);
                klPart->setInitialPartitioning(p1, p2);
                //klPart->setHops(VMScheduler::getPmsGraph()->getEdge(p1->getId(), p2->getId()));
                klPart->setHops(1);
                //klPart->setMaxExternal(maxcost);
                klPart->setPartitionTopology(VMScheduler::getPmsGraph());
                klPart->run();
            }
            d = maxcost - computeTotalCost();
            std::cout << d << "\n";
        //}
    /*
    bool pairMap[tempplist->size()][tempplist->size()], iddle = true;

    for(int i = 0; i < tempplist->size(); i++){
        for(int j = 0; j < tempplist->size(); j++){
            pairMap[i][j] = true;
            pairMap[j][i] = true;
        }
    }

    do{
        prevcost = computeTotalCost();
        max = 0;

        for(int i = 0; i < tempplist->size() - 1; i++){
            for(int j = i + 1; j < tempplist->size(); j++){
                if(i == j)
                    continue;

                if(!pairMap[i][j])
                    continue;

                tempp1 = tempplist->get(i)->getVertexList()->copyStatic();
                tempp2 = tempplist->get(j)->getVertexList()->copyStatic();
                p1 = tempplist->get(i);
                p2 = tempplist->get(j);

                //std::cout << " i " << p1->getVertexList() << " j " << p2->getVertexList() << "\n";

                klPart->setInitialPartitioning(p1, p2);
                //klPart->setHops(VMScheduler::getPmsGraph()->getEdge(p1->getId(), p2->getId()));
                klPart->setHops(1);
                //klPart->setMaxExternal(prevcost);
                //klPart->setPartitionTopology(VMScheduler::getPmsGraph());
                klPart->run();
                actualcost = computeTotalCost();

                if(prevcost - actualcost > 0){
                        if(prevcost - actualcost > max){
                            max = prevcost - actualcost;
                            maxp1 = p1;
                            maxp2 = p2;
                            lastp1 = p1;
                            lastp2 = p2;
                            iId = i;
                            jId = j;
                        }
                }

                delete p1->getVertexList();
                delete p2->getVertexList();

                p1->setVertexList(tempp1);
                p2->setVertexList(tempp2);

                for(int i = 0; i < tempp1->size(); i++)
                    tempp1->get(i)->setPartition(p1);

                for(int i = 0; i < tempp2->size(); i++)
                    tempp2->get(i)->setPartition(p2);

            }

        }

        std::cout << "max = " << max << " " << computeTotalCost() << "\n";
        std::cout << maxp1->getId() << "  " << maxp2->getId() << "\n";

        if(max == 0){
            if(iddle){
                break;
            }
            else{
                for(int i = 0; i < tempplist->size(); i++){
                    for(int j = 0; j < tempplist->size(); j++){
                        pairMap[i][j] = true;
                        pairMap[j][i] = true;
                    }
                }
                iddle = true;
                continue;
            }
        }



        klPart->setInitialPartitioning(maxp1, maxp2);
        //klPart->setHops(VMScheduler::getPmsGraph()->getEdge(maxp1->getId(), maxp2->getId()));
        //klPart->setMaxExternal(computeTotalCost());
        klPart->setHops(1);
        //klPart->setPartitionTopology(VMScheduler::getPmsGraph());
        klPart->run();
        pairMap[iId][jId] = false;
        pairMap[jId][iId] = false;
        iddle = false;

    }while(true);*/

    delete tempplist;

    std::cout << "Total cost: " << computeTotalCost() << "\n";

    //Partition Map
    /*
    List<Vertex> *newvmlist = plist->get(0)->getVertexList()->copyStatic();
    plist->get(0)->setId(pms->get(0)->getId());
    List<Partition> *newplist = new List<Partition>(plist->size());
    newplist->add(plist->remove(0));
    int it = 1;

    while(plist->size() > 0){
        int index = 0;
        double mincost = -1;
        //std::cout << it << "\n";
        for(int j = 0; j < plist->size(); j++){
            int psize = plist->get(0)->size();
            if(plist->get(j)->size() == 0){
                delete plist->remove(j);
                j--;
                continue;
            }

            plist->get(j)->setId(pms->get(it)->getId());
            if(psize == plist->get(j)->size()){
                if(mincost == -1){
                    index = j;
                    mincost = computeAdditionalTotalCost(newvmlist, plist->get(j)->getVertexList());
                }else{
                    double c = computeAdditionalTotalCost(newvmlist, plist->get(j)->getVertexList());
                    if(c < mincost){
                        mincost = c;
                        index = j;
                    }
                }
            }
        }

        for(int l = 0; l < plist->get(index)->size(); l++)
            newvmlist->add(plist->get(index)->getVertexList()->get(l));

        newplist->add(plist->remove(index));
        std::cout << index << "\n";
        it++;
    }

    delete plist;
    delete newvmlist;
    plist = newplist;*/
}

void VMSchedulerKLFlat::checkInitialPart(){
    Partition* pid;
    VirtualMachine *vm;

    for(int i = 0; i < tempvms->size(); i++){
        vm = (VirtualMachine*)tempvms->get(i);
        if(vm->isAlloc()){
            pid = plist->get(vm->getHost()->getId());
            pid->getVertexList()->add(tempvms->get(i));
            vm->setPartition(pid);
            pid->setD1(vm->getHost()->getMem());
            pid->setD2(vm->getHost()->getHd());
            pid->setD3(vm->getHost()->getPower());
        }else{
            std::cout << "nao alocado!";
        }
    }
}

void VMSchedulerKLFlat::initialPartFF(){

    for(int i = 0 ; i < tempvms->size(); i++){
        for(int j = 0; j < pms->size(); j++){
            if(pms->get(j)->fit((VirtualMachine*)tempvms->get(i))){
                tempvms->get(i)->setPartition(plist->get(j));
                plist->get(j)->getVertexList()->add(tempvms->get(i));
                pms->get(j)->addVm((VirtualMachine*)tempvms->get(i));
                break;
            }
        }
    }
}

void VMSchedulerKLFlat::initialPartRR(){
    int r = 0;

    for(int i = 0 ; i < tempvms->size(); i++){
        for(int j = r; j < pms->size(); j++){
            if(pms->get(j)->fit((VirtualMachine*)tempvms->get(i))){
                tempvms->get(i)->setPartition(plist->get(j));
                plist->get(j)->getVertexList()->add(tempvms->get(i));
                pms->get(j)->addVm((VirtualMachine*)tempvms->get(i));
                break;
            }
        }
        r++;
        if(r > pms->size() - 1)
            r = 0;
    }
}

void VMSchedulerKLFlat::initialPartNABF(){

    List<VirtualMachine> *vmlist = (List<VirtualMachine>*)tempvms;

    VMSchedulerMA_NABF nabf(vmlist, pms, pmsgraph, vmsgraph);
    nabf.run();

    for(int i = 0; i < vmlist->size(); i++){
        vmlist->get(i)->setPartition(plist->get(vmlist->get(i)->getHost()->getId()));
        plist->get(vmlist->get(i)->getHost()->getId())->getVertexList()->add(vmlist->get(i));
    }
}


void VMSchedulerKLFlat::performKLMC(){
    int n = 0;
    for(int i = 0 ; i < pms->size(); i++){
        if(pms->get(i)->getId() > n)
            n = pms->get(i)->getId();
    }
    n++;

    plist = new List<Partition>(n);

    for(int i = 0; i < n; i++)
        plist->add(new Partition(i, tempvms->size()));

    int alloc = 0;
    checkInitialPart();
    /*FILE* f = fopen("part", "w");
    for(int i = 0;  i < tempvms->size(); i++){
        fprintf(f,"%d ", tempvms->get(i)->getPartitionId());
    }
    fclose(f);*/

    for(int i = 0; i < tempvms->size(); i++){
        ((VirtualMachine*)tempvms->get(i))->getHost()->removeVm((VirtualMachine*)tempvms->get(i));
    }

    for(int i = 0; i < plist->size(); i++)
        if(plist->get(i)->size() == 0){
            delete plist->remove(i);
            i--;
        }

    double globalCost, oldCost, newCost;
    int rr = 0;
    double bestcost = -1;
    bool go = true;

    globalCost = computeTotalCost();

    std::cout << "\nTotal external traffic: " << globalCost;
    std::cout << "\nP: " << plist->size() << "\n";

    if(plist->size() > 1)
        KLMultCluster klmc(plist, tempvms, pmsgraph, vmsgraph);

}

void VMSchedulerKLFlat::retryFF(){

    for(int i = 0 ; i < tempvms->size(); i++){
        for(int j = 0; j < pms->size(); j++){
            if(pms->get(j)->fit((VirtualMachine*)tempvms->get(i))){
                ((VirtualMachine*)tempvms->get(i))->setHost(pms->get(j));
                pms->get(j)->addVm((VirtualMachine*)tempvms->get(i));
                break;
            }
        }
    }
}

void VMSchedulerKLFlat::fit(){
    sortVms();
    CloudMachine *pm;

    for (int i = 0 ; i < tempvms->size(); i++){
        for(int j = 0; j < pms->size(); j++)
            if(pms->get(j)->getId() == tempvms->get(i)->getPartitionId()){
                pm = pms->get(j);
                break;
            }

        if(pm->fit((VirtualMachine*)tempvms->get(i))){
            pm->addVm((VirtualMachine*)tempvms->get(i));
            ((VirtualMachine*)tempvms->get(i))->setHost(pm);
            tempvms->remove(i);
            i--;
        }else{
            ((VirtualMachine*)tempvms->get(i))->setAlloc(false);
        }
    }

    delete plist;
}

void VMSchedulerKLFlat::sortVms(){
    List<double> edgessum(tempvms->size());
    List<VirtualMachine>* sort = ( List<VirtualMachine>*)tempvms->copyStatic();
    int chooseindex;
    double maxsum;

    for(int i = 0 ;  i< tempvms->size() ; i++){
        edgessum.add(new double(VMScheduler::getVmsGraph()->getEdgesSum(tempvms->get(i)->getId())));
    }

    while(tempvms->size() > 0)
        tempvms->remove(0);

    while(sort->size() > 0){
        chooseindex = 0;
        maxsum = *edgessum.get(0);
        for(int i = 1 ; i < edgessum.size(); i++){
            if(*edgessum.get(i) > maxsum){
                maxsum = *edgessum.get(i);
                chooseindex = i;
            }
        }
        tempvms->add(sort->get(chooseindex));
        sort->remove(chooseindex);
        delete edgessum.remove(chooseindex);
    }

    delete sort;
}

void VMSchedulerKLFlat::sortVmsByMemory(int low, int high){
    if (low < high) {
        double pivot = ((VirtualMachine*)tempvms->get(high))->getMem();
        int i = (low - 1);

        for (int j = low; j <= high - 1; j++){
            if(((VirtualMachine*)tempvms->get(high))->getMem() > pivot){
                i++;
                tempvms->swap(i, j);
            }
        }
        tempvms->swap(i + 1, high);
        int pi = i + 1;
        sortVmsByMemory(low, pi - 1);
        sortVmsByMemory(pi + 1, high);
    }
}

void VMSchedulerKLFlat::retryNA(){
    VirtualMachine* vm;
    CloudMachine* pm;
    CloudMachine* target;
    double cost;
    double mincost;
    List<Vertex>* adj;

    List<CloudMachine>* fullpms = (List<CloudMachine>*) pmsgraph->getVertexList();

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

                //cost = pm->getMem()*pm->getHd()*pm->getPower() - vm->getMem()*vm->getHd()*vm->getPower();

                delete adj;
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
        if(expanded->contains(target) == -1)
            expanded->add(target);
        i--;
    }

}

void VMSchedulerKLFlat::initialPart(char* file){
    FILE* fp = fopen(file, "r");
    int j;
    for(int i = 0;  i < tempvms->size(); i++){
        fscanf(fp, "%d ", &j);

        if(pms->get(j)->fit((VirtualMachine*)tempvms->get(i))){
            ((VirtualMachine*)tempvms->get(i))->setHost(pms->get(j));
            tempvms->get(i)->setPartition(plist->get(j));
            plist->get(j)->getVertexList()->add(tempvms->get(i));
            pms->get(j)->addVm((VirtualMachine*)tempvms->get(i));
        }
    }
    fclose(fp);
}

double VMSchedulerKLFlat::computePartitionCost(Partition* p1, Partition* p2){
    double d = 0;

    for(int i = 0; i < p1->size(); i++){
        for(int j = 0; j < p2->size(); j++){
            d += vmsgraph->getEdge(p1->getVertexList()->get(i), p2->getVertexList()->get(j));
        }
    }

    d *= pmsgraph->getEdge(p1->getId(), p2->getId());

    return d;
}

List<CloudMachine>* VMSchedulerKLFlat::getExpanded(){
    return expanded;
}
