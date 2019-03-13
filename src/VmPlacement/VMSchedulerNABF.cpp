#include<VmPlacement/VMSchedulerNABF.h>
#include<VmPlacement/VMSchedulerKLFlat.h>
#include <iostream>


VMSchedulerNABF::VMSchedulerNABF(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph* pmsTopology, Graph* vmsTraffic): VMScheduler(vmlist, pmlist, pmsTopology, vmsTraffic){
    int maxId = 0;
    for(int i = 0 ; i < pms->size(); i++){
        if(pms->get(i)->getId() > maxId)
            maxId = pms->get(i)->getId();
    }

    activesHosts = new List<CloudMachine>(pms->size());
    inactivesHosts = new List<CloudMachine>(pms->size());
    for(int i = 0; i < pmlist->size(); i++){
        if(pmlist->get(i)->getVms()->size() > 0)
            activesHosts->add(pmlist->get(i));
        else
            inactivesHosts->add(pmlist->get(i));
    }
    lastpm = NULL;
    firstpm = new bool[maxId + 1];
    restartNaVector();
}

void VMSchedulerNABF::restartNaVector(){
    int maxId = 0;
    for(int i = 0 ; i < pms->size(); i++){
        if(pms->get(i)->getId() > maxId)
            maxId = pms->get(i)->getId();
    }

    for(int i = 0; i < maxId; i++){
        firstpm[i] = false;
    }
}

VMSchedulerNABF::~VMSchedulerNABF(){
    /*delete activesHosts;
    delete inactivesHosts;
    while(pmsToCluster->size() > 0)
        delete pmsToCluster->get(0);

    delete pmsToCluster;*/
}

void VMSchedulerNABF::run(){
    sortClusters();
    List<VirtualMachine> *cluster;
    lastpm = NULL;

    for(int i = 0 ; i < clusters->size(); i++){
        for(int j = 0; j < clusters->get(i)->size(); j++){
            clusters->get(i)->get(j)->setGroup(i);
        }
    }

    pmsToCluster = new List<List<CloudMachine>>(clusters->size());
    for(int i = 0 ; i < clusters->size(); i++)
        pmsToCluster->add(new List<CloudMachine>(pms->size()));
/*
    while(clusters->size() > 0){
        for(int i = clusters->size() - 1; i > - 1; i--){
            if(clusters->get(i)->size() > 1){
                naFit(clusters->remove(i));
            }
        }

        bfApm(clusters->remove(0)->get(0));
    }*/

    for(int i = 0 ; i < clusters->size(); i++){
        cluster = clusters->get(i);

        if(cluster->size() > 1){
            //sortVmsByEdge(cluster);
            std::cout << "size: " << clusters->size();
            naFit(cluster);
        }else{
            bfApm(cluster->get(0));
        }
    }
}

void VMSchedulerNABF::naFit(List<VirtualMachine> *list){
    int maxhop = 0;
    CloudMachine *selectedHost = NULL;
    int selectedIndex;

    //First placement
    if(lastpm == NULL){
        lastpm = pms->get(0);
        selectedHost = pms->get(0);
        firstpm[0] = true;
    }else{
        //select next pod
        for(int i = 0; i < inactivesHosts->size(); i++){
            if(!firstpm[i]){
                if(maxhop < pmsgraph->getEdge(lastpm, inactivesHosts->get(i))){
                    maxhop = pmsgraph->getEdge(lastpm, inactivesHosts->get(i));
                    selectedHost = inactivesHosts->get(i);
                    selectedIndex = inactivesHosts->get(i)->getId();
                }
            }
        }
        if(selectedHost == NULL){
            restartNaVector();
            selectedHost = pms->get(0);
            firstpm[0] = true;
        }else{
            firstpm[selectedIndex] = true;
        }
    }
    //lastpm = selectedHost;
    List<VirtualMachine>* vmstoplace = list->copyStatic();
    List<VirtualMachine>* success = new List<VirtualMachine>(list->size());
    VirtualMachine *vm;
    vm = vmstoplace->remove(0);

    List<CloudMachine>* assignedpms = pmsToCluster->get(vm->getGroup());

    if(!selectedHost->fit(vm)){
       selectedHost = nextPm(vm, selectedHost);
    }

    if(selectedHost == NULL){
        std::cout <<"erro";
        return;
    }

    placeVm(vm, selectedHost);
    success->add(vm);

    assignedpms->add(selectedHost);

    //Best-fit phase
    int npms = 0;
    while(vmstoplace->size() > 0){
        vm = vmstoplace->remove(0);
        selectedHost = bestFit(vm, assignedpms);

        if(selectedHost == NULL)
            selectedHost = expandCluster(vm);

        if(selectedHost == NULL){
            continue;
            return;
        }

        if(assignedpms->contains(selectedHost) == -1){
            assignedpms->add(selectedHost);
            npms++;
        }

        placeVm(vm, selectedHost);
        success->add(vm);
    }

    if(assignedpms->size() > 1){
         //corte de arestas
        //Graph* minig = tiny(vmsgraph, success, npms);
        //std::cout << "v = " << minig->getEdge(0,0) << "\n";
        VMSchedulerKLFlat klsort(success, pms, pmsgraph, vmsgraph);
        //VMSchedulerKLFlat klsort(success, pms, pmsgraph, minig);
        std::cout << "startkl";
        klsort.run();
        //delete minig;
        List<CloudMachine> *expanded = klsort.getExpanded();
        int x;

        for(int i = 0; i < expanded->size(); i++){
            if(activesHosts->contains(expanded->get(i)) == -1)
                activesHosts->add(expanded->get(i));

            inactivesHosts->remove (expanded->get(i));
            assignedpms->add(expanded->get(i));
            selectedHost = expanded->get(i);
            lastpm = expanded->get(i);
        }

    }else
        std::cout << "quitkl";

    delete vmstoplace;
    delete success;
}

Graph *VMSchedulerNABF::tiny(Graph* big, List<VirtualMachine>* vmlist, int n){
    Graph* g = new Graph(big->getNumberOfVertices());
    g->setVertexList(big->getVertexList());
    int c = vmlist->size();
    double v = 0;
    for(int i = 0; i < vmlist->size(); i++){
        for(int j = 0; j < i; j++){
            if(big->getEdge(vmlist->get(i)->getId(), vmlist->get(j)->getId()) != 0){
                c++;
                v = v + big->getEdge(vmlist->get(i)->getId(), vmlist->get(j)->getId());
            }
        }
    }

    for(int i = 0; i < big->getNumberOfVertices(); i++){
        for(int j = 0; j < big->getNumberOfVertices(); j++){
            g->insertArc(i, j, big->getEdge(i, j));
        }
    }

    int k = c - c/n;

    std::cout << "k = " << k;

    int ci = -1, cj = -1;
    float w = 0;
    v = v/c;
    std::cout << "v = " << v;

    /*for(int l = 0; l < vmlist->size(); l++)
        for(int j = 0; j < l; j++)
            if(g->getEdge(vmlist->get(l)->getId(), vmlist->get(j)->getId()) < v){
                g->insertArc(vmlist->get(l)->getId(), vmlist->get(j)->getId(), 0);
            }*/

    for(int i = 0; i < k; i++){
        std::cout << "i = " << i << "\n";
        for(int l = 0; l < vmlist->size(); l++){
            for(int j = 0; j < l; j++){
                if(ci == -1 && g->getEdge(vmlist->get(l)->getId(), vmlist->get(j)->getId()) > 0){
                   w = g->getEdge(vmlist->get(l)->getId(), vmlist->get(j)->getId());
                   ci = vmlist->get(j)->getId();
                   cj = vmlist->get(l)->getId();
                   continue;
                }
                if(g->getEdge(vmlist->get(l)->getId(), vmlist->get(j)->getId()) < w
                && g->getEdge(vmlist->get(l)->getId(), vmlist->get(j)->getId()) > 0){
                    w = g->getEdge(vmlist->get(l)->getId(), vmlist->get(j)->getId());
                    ci = vmlist->get(j)->getId();
                    cj = vmlist->get(l)->getId();
                }

            }
        }
        if(ci == -1)
            continue;

        g->insertArc(ci, cj, 0);
        ci = -1;
    }
    std::cout << "corte ok";
    return g;

}

CloudMachine *VMSchedulerNABF::nextPm(VirtualMachine *vm, CloudMachine *start){
    int hop = -1;
    CloudMachine *selectedHost = NULL;

    for(int i = 0; i < inactivesHosts->size(); i++){
            if(!inactivesHosts->get(i)->fit(vm))
                continue;

            if(hop != -1){
                if(hop > pmsgraph->getEdge(start, inactivesHosts->get(i))){
                    hop = pmsgraph->getEdge(start, inactivesHosts->get(i));
                    selectedHost = inactivesHosts->get(i);
                }
            }else{
                hop = pmsgraph->getEdge(start, inactivesHosts->get(i));
                selectedHost = inactivesHosts->get(i);
            }
        }

    if(selectedHost == NULL){
        hop = -1;
        for(int i = 0; i < activesHosts->size(); i++){
            if(!activesHosts->get(i)->fit(vm))
                continue;

            if(hop != -1){
                if(hop > pmsgraph->getEdge(start, activesHosts->get(i))){
                    hop = pmsgraph->getEdge(start, activesHosts->get(i));
                    selectedHost = activesHosts->get(i);
                }
            }else{
                hop = pmsgraph->getEdge(start, activesHosts->get(i));
                selectedHost = activesHosts->get(i);
            }
        }
    }

    return selectedHost;
}

void VMSchedulerNABF::bfApm(VirtualMachine *vm){
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

CloudMachine *VMSchedulerNABF::firstFitDecreasing(VirtualMachine* vm, List<CloudMachine> *target){
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

CloudMachine *VMSchedulerNABF::bestFit(VirtualMachine* vm, List<CloudMachine>* target){
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

CloudMachine *VMSchedulerNABF::expandCluster(VirtualMachine* vm){
    CloudMachine* pm;
    CloudMachine* target;
    double cost;
    double mincost;
    List<Vertex>* adj;

    target = NULL;
    mincost = -1;

    for(int j = 0; j < pms->size(); j++){
        cost = 0;
        pm = pms->get(j);
        if(pm->fit(vm)){
            adj = vmsgraph->getVertexAdj(vm->getId());
            for(int k = 0; k < adj->size(); k++){
                if(((VirtualMachine*)adj->get(k))->isAlloc())
                    cost += vmsgraph->getEdge(vm->getId(), adj->get(k)->getId()) * pmsgraph->getEdge(pm->getId(), ((VirtualMachine*)adj->get(k))->getHost()->getId());
            }
            delete adj;
            if(mincost == -1){
                target = pm;
                mincost = cost;
            }else{
                if(mincost > cost){
                    target = pm;
                    mincost = cost;
                }
            }
        }
    }

    lastpm = target;
    return target;
}

void VMSchedulerNABF::placeVm(VirtualMachine* vm, CloudMachine* pm){
    success++;
    if(pm->getVms()->size() == 0){
        activesHosts->add(pm);
        inactivesHosts->remove(pm);
    }

    pm->addVm(vm);
    vm->setHost(pm);
}
