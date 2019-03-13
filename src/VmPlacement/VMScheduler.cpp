#include "VmPlacement/VmScheduler.h"
#include "VmPlacement/CloudMachine.h"
#include "VmPlacement/VirtualMachine.h"
#include "Vertex.h"
#include "Partition.h"
#include "Partitioning.h"
#include "Graph.h"
#include "listlib.h"
#include "VmPlacement/Slot.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>

VMScheduler::~VMScheduler(){
    while(clusters->size() > 0)
        delete clusters->remove(0);

    delete clusters;
}

VMScheduler::VMScheduler(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph* pmsTopology, Graph* vmsTraffic){
    pms = pmlist;
    pmsgraph = pmsTopology;
    vmsgraph = vmsTraffic;
    vms = vmlist;

    clusters = new List<List<VirtualMachine>>(vms->size());
}

double VMScheduler::computeTotalTraffic(){
    double total = 0;

    for(int i = 0; i < vms->size(); i++){
        for(int j = i; j < vms->size(); j++){
            if(!vms->get(i)->isDummy() && !vms->get(j)->isDummy() && vms->get(j)->isAlloc() && vms->get(i)->isAlloc())
                total += vmsgraph->getEdge(vms->get(i)->getId(), vms->get(j)->getId())*pmsgraph->getEdge(vms->get(i)->getHost()->getId(), vms->get(j)->getHost()->getId());
        }
    }
    return total;
}

double VMScheduler::computeTotalTraffic(List<VirtualMachine> *vmlist){
    double total = 0;

    for(int i = 0; i < vmlist->size(); i++){
        for(int j = i; j < vmlist->size(); j++){
            if(!vmlist->get(i)->isDummy() && !vmlist->get(j)->isDummy() && vmlist->get(j)->isAlloc() && vmlist->get(i)->isAlloc())
                total += vmsgraph->getEdge(vmlist->get(i)->getId(), vmlist->get(j)->getId())*pmsgraph->getEdge(vmlist->get(i)->getHost()->getId(), vmlist->get(j)->getHost()->getId());
        }
    }
    return total;
}

int VMScheduler::allocatedVms(){
    int n = 0;
    for(int i = 0; i < pms->size(); i++)
        n += pms->get(i)->getVms()->size();

    return n;
}

int VMScheduler::activesPms(){
    int val = 0;
    for(int i = 0 ; i < pms->size(); i++)
        if(pms->get(i)->getVms()->size() > 0)
            val++;

    return val;
}

void VMScheduler::sortClusters(int size){
    double s;
    while(clusters->size() > 0)
        delete clusters->remove(0);

    List<List<VirtualMachine>> tempclusters(vms->size());

    List<double> sum(vms->size());
    List<VirtualMachine>* cluster;
    List<Vertex>* adj;
    double max;
    int pos;
    int maxindex;

    while(vms->size() > 0){
        cluster = new List<VirtualMachine>(vms->size());
        cluster->add(vms->remove(0));
        for(int i = 0; i < cluster->size(); i++){
            for(int j = 0 ; j < vms->size(); j++){
                if(vmsgraph->getEdge(cluster->get(i)->getId(), vms->get(j)->getId()) != 0){
                    cluster->add(vms->remove(j));
                    j--;
                }

            }
        }

        klCut(cluster, vms, size);

        s = 0;
        for(int i = 0; i < cluster->size() - 1; i++)
            for(int j = i + 1; j < cluster->size(); j++){
                s += vmsgraph->getEdge(cluster->get(i)->getId(), cluster->get(j)->getId());
            }

        sortVmsByEdge(cluster);
        tempclusters.add(cluster);
        sum.add(new double(s));

    }

    int ss = 0;

    while(sum.size() > 0){
        maxindex = 0;
        max = *sum.get(0);

        for(int i = 1; i < sum.size(); i++){
            if(max < *sum.get(i)){
                max = *sum.get(i);
                maxindex = i;
            }
        }

        delete sum.remove(maxindex);
        cluster = tempclusters.remove(maxindex);
        clusters->add(cluster);

        ss += cluster->size();

        for(int i = 0; i < cluster->size(); i++){
            vms->add(cluster->get(i));
        }
    }

    std::cout << "vms: " << ss;
}


void VMScheduler::sortClusters(){
    double s;
    while(clusters->size() > 0)
        delete clusters->remove(0);

    List<List<VirtualMachine>> tempclusters(vms->size());

    List<double> sum(vms->size());
    List<VirtualMachine>* cluster;
    List<Vertex>* adj;
    int maxindex = 0;
    double max;
    int pos;

    while(vms->size() > 0){
        cluster = new List<VirtualMachine>(vms->size());
        cluster->add(vms->remove(0));
        for(int i = 0; i < cluster->size(); i++){
            for(int j = 0 ; j < vms->size(); j++){
                if(vmsgraph->getEdge(cluster->get(i)->getId(), vms->get(j)->getId()) != 0){
                    cluster->add(vms->remove(j));
                    j--;
                }

            }
        }
        s = 0;
        for(int i = 0; i < cluster->size() - 1; i++)
            for(int j = i + 1; j < cluster->size(); j++){
                s += vmsgraph->getEdge(cluster->get(i)->getId(), cluster->get(j)->getId());
            }

        sortVmsByEdge(cluster);
        tempclusters.add(cluster);
        //std::cout << cluster->size() << " ";
        sum.add(new double(s));

    }

    while(sum.size() > 0){
        maxindex = 0;
        max = *sum.get(0);

        for(int i = 1; i < sum.size(); i++){
            if(max < *sum.get(i)){
                max = *sum.get(i);
                maxindex = i;
            }
        }

        delete sum.remove(maxindex);
        cluster = tempclusters.remove(maxindex);
        clusters->add(cluster);

        for(int i = 0; i < cluster->size(); i++){
            vms->add(cluster->get(i));
        }
    }
}

void VMScheduler::sortClustersEx(){
    double s;
    List<List<VirtualMachine>> clusters(vms->size());
    List<double> sum(vms->size());
    List<VirtualMachine>* cluster;
    List<Vertex>* adj;
    int maxindex = 0;
    double max;
    int pos;

    while(vms->size() > 0){
        cluster = new List<VirtualMachine>(vms->size());
        cluster->add(vms->get(0));
        vms->remove(0);
        adj = vmsgraph->getVertexAdj(cluster->get(0)->getId());
        for(int j = 0; j < adj->size(); j++){
            if(cluster->contains((VirtualMachine*)adj->get(j)) == -1){
                if(vms->remove((VirtualMachine*)adj->get(j)))
                    cluster->add((VirtualMachine*)adj->get(j));
            }
        }
        delete adj;

        s = 0;
        for(int i = 0; i < cluster->size() - 1; i++)
            for(int j = i + 1; j < cluster->size(); j++)
                s += vmsgraph->getEdge(cluster->get(i)->getId(), cluster->get(j)->getId());

        clusters.add(cluster);
        sum.add(new double(s));
    }

    while(sum.size() > 0){
        maxindex = 0;
        max = *sum.get(0);

        for(int i = 1; i < sum.size(); i++){
            if(max < *sum.get(i)){
                max = *sum.get(i);
                maxindex = i;
            }
        }

        delete sum.remove(maxindex);
        cluster = clusters.remove(maxindex);

        for(int i = 0; i < cluster->size(); i++){
            vms->add(cluster->get(i));
        }
        delete cluster;
    }
}

void VMScheduler::sortVmsByEdge(List<VirtualMachine>* list){
    VirtualMachine *vm1, *vm2;
    List<VirtualMachine>* sorted = new List<VirtualMachine>(list->size());
    double maxedge = 0;
    int x;

    while(list->size() > 0){
        vm1 = list->get(0);
        vm2 = list->get(1);
        for(int i = 0; i < list->size() - 1; i++){
            for(int j = i; j < list->size(); j++){
                if(maxedge < vmsgraph->getEdge(list->get(i)->getId(), list->get(j)->getId())){
                    maxedge = vmsgraph->getEdge(list->get(i)->getId(), list->get(j)->getId());
                    vm1 = list->get(i);
                    vm2 = list->get(j);
                }
            }
        }
        if(list->size() > 1){
            list->remove(vm1);
            list->remove(vm2);
            sorted->add(vm1);
            sorted->add(vm2);
        }else
            sorted->add(list->remove(0));

    }
    while(sorted->size() > 0){
        list->add(sorted->remove(0));
    }
    delete sorted;
}

bool VMScheduler::checkMemory(){
    for(int i = 0; i < pms->size(); i++){
        if(pms->get(i)->getMem() < 0 || pms->get(i)->getPower() < 0)
            return false;
    }
    return true;
}

void VMScheduler::sortVmsByMemory(List<VirtualMachine>* list){
    List<VirtualMachine>* sorted = new List<VirtualMachine>(list->size());
    int maxmem;
    int pos;

    while(list->size() > 0){
        pos = 0;
        maxmem = 0;
        for(int i = 0; i < list->size(); i++){
            if(list->get(i)->getMem() > maxmem){
                maxmem = list->get(i)->getMem();
                pos = i;
            }
        }
        sorted->add(list->remove(pos));
    }

    for(int i = 0; i < sorted->size(); i++)
        list->add(sorted->get(i));

    delete sorted;
}

void VMScheduler::klCut(List<VirtualMachine>* cluster, List<VirtualMachine>* rest, int size){
    if(cluster->size() < size + 2)
        return;

    Partition* pA = new Partition(0, cluster->size());
    Partition* pB = new Partition(1, cluster->size());

    for(int i = 0; i < size; i++){
        pA->getVertexList()->add(cluster->get(i));
        cluster->get(i)->setPartition(pA);
    }

    for(int i = size; i < cluster->size(); i++){
        pB->getVertexList()->add(cluster->get(i));
        cluster->get(i)->setPartition(pB);
    }

    Partitioning* klPart = new Partitioning(vmsgraph, (List<Vertex>*)cluster, NULL, NULL);
    klPart->setInitialPartitioning(pA, pB);
    klPart->run();

    cluster = (List<VirtualMachine>*)klPart->getBestA()->getVertexList();
    for (int i = 0; i < klPart->getBestB()->size(); i++){
        rest->add((VirtualMachine*)klPart->getBestB()->getVertexList()->get(i));
    }

}

void VMScheduler::dumpSolution(){
    FILE *sol = fopen("placement", "w");

    for(int i = 0; i < vms->size(); i++){
        if(vms->get(i)->isAlloc()){
            fprintf(sol, "%d ", vms->get(i)->getHost()->getId());
        }else
            fprintf(sol, "-1 ");
    }

    fclose(sol);
}

void VMScheduler::volumeSort(){
    List<CloudMachine>* sort = pms->copyStatic();
    pms->clear();

    while(sort->size() > 0){
        int index = 0;
        int vol = sort->get(0)->getPower() * sort->get(0)->getMem();
        for(int i = 1; i < sort->size(); i++){
            if(vol < sort->get(i)->getPower() * sort->get(i)->getMem()){
                index = i;
                vol = sort->get(i)->getPower() * sort->get(i)->getMem();
            }
        }
        pms->add(sort->remove(index));

    }
    delete sort;
}

