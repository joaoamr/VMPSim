#include <stdio.h>
#include "FatTreeGraph.h"
#include "StarGraph.h"
#include "listlib.h"
#include "VmPlacement/VirtualMachine.h"
#include "VmPlacement/CloudMachine.h"
#include "VmPlacement/VMScheduler.h"
#include "VmPlacement/VMSchedulerRR.h"
#include "VmPlacement/VMSchedulerFFKL.h"
#include "VmPlacement/VMSchedulerEAJOINT.h"
#include "VmPlacement/VMSchedulerSlottedKL.h"
#include "VmPlacement/VMSchedulerFF.h"
#include "VmPlacement/VMSchedulerKLHierarquichal.h"
#include "VmPlacement/VMSchedulerKLFlat.h"
#include "VmPlacement/VMSchedulerNABF.h"
#include "VmPlacement/VMSchedulerBFDM.h"
#include "VmPlacement/VMSchedulerMA_NABF.h"
#include "VmPlacement/VMSchedulerFFD.h"
#include "VmPlacement/VMSchedulerOpt.h"
#include <VmPlacement/VMSchedulerMA_NABF.h>
#include <StaticGraph.h>
#include <Partition.h>
#include <Partitioning.h>
#include <Vertex.h>
#include <Graph.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <random>
#include <time.h>

#define HIERARQUICHAL 0
#define FLAT 1

List<VirtualMachine>* loadVms(char *filename){
    FILE* f = fopen(filename, "r");

    List<VirtualMachine>* vms;
    int n;
    int mem;
    char s[1000];

    fscanf(f, "%d\n", &n);
    fscanf(f, "%d %d\n", &n, &mem);
    fgets(s, 1000, f);

    vms = new List<VirtualMachine>(n);

    for(int i = 0 ; i < n; i++){
        fscanf(f, "%d ", &mem);
        fgets(s, 1000, f);
        vms->add(new VirtualMachine(i, mem, 100, 1));
    }

    fclose(f);

    return vms;

}

Graph* buildNetworkTopology(int n, char* topology, int k){
    double dist = 0;
    Graph *d;
    if(strcmp(topology, "Fat-tree") == 0){
        d = new FatTreeGraph(k, n);
    }

    if(strcmp(topology, "Star") == 0){
        d = new StarGraph(k, n);
    }

    return d;
}


double computeTotalCost(List<Vertex> *vList, Graph* source){
    double d = 0;
    for(int i = 0; i < vList->size(); i++)
        for(int j = i; j < vList->size(); j++)
            if(vList->get(i)->getPartition() != vList->get(j)->getPartition())
                d += source->getEdge(vList->get(i), vList->get(j));

    return d;
}

List<Vertex>* completeVertexList(List<Vertex>* vlist, int n){
    int newsize = vlist->size();
    while(newsize % n != 0)
        newsize++;

    List<Vertex>* newList = new List<Vertex>(newsize);
    for(int i = 0; i < vlist->size(); i++)
        newList->add(vlist->get(i));

    for(int i = 0; i < newsize - vlist->size(); i++)
        newList->add(new Vertex(newList->size(), true));

    return newList;
}

void runHierarquichalKL(int n, Graph* d, int mode){
    List<Vertex>* vlist;
    int totalp = 1;
    for(int i = 1;  i < n; i++){
        if(totalp > n)
            break;

        totalp += totalp*totalp;
    }

    Partitioning** klCluster =  (Partitioning **)malloc(sizeof(Partitioning *)*totalp);
    vlist = d->getVertexList();
    //vlist = completeVertexList(d->getVertexList(), n);
    klCluster[0] = new Partitioning(d, vlist, NULL, NULL);
    klCluster[0]->setMode(mode);
    klCluster[0]->run();
    for(int i = 1; i < n - 2; i++){
        klCluster[i*2 - 1] = new Partitioning(d, klCluster[i - 1]->getBestA()->getVertexList(), NULL, NULL);
        klCluster[i*2 - 1]->setMode(mode);
        klCluster[i*2 - 1]->run();
        klCluster[i*2] = new Partitioning(d, klCluster[i - 1]->getBestB()->getVertexList(), NULL, NULL);
        klCluster[i*2]->setMode(mode);
        klCluster[i*2]->run();
    }
}

void runFlatKL(int n, Graph* d){
    double globalCost, oldCost, newCost;
    List<Vertex>* vlist = d->getVertexList();
    List<Partition> plist(n);
    Partitioning* klCluster = new Partitioning(d, vlist, NULL, NULL);
    int rr = 0;
    double bestcost = -1;
    bool go = true;
    for(int i = 0; i < n; i++)
        plist.add(new Partition(i, vlist->size()/n));

    //Round Robin
    for(int i = 0; i < vlist->size(); i++){
        if(rr == n)
            rr = 0;

        plist.get(rr)->getVertexList()->add(vlist->get(i));
        vlist->get(i)->setPartition(plist.get(rr));
        rr++;
    }

    globalCost = computeTotalCost(vlist, d);

    std::cout << "\nTotal external traffic: " << globalCost << "\n\n";

    do{
        oldCost = computeTotalCost(vlist, d);
        for(int i = 0; i < plist.size() - 1; i++){
            for(int j = i+1; j < plist.size(); j++){
                newCost = computeTotalCost(vlist, d);
                klCluster->setInitialPartitioning(plist.get(i), plist.get(j));
                klCluster->setMaxExternal(newCost);
                klCluster->run();
            }
        }
        if(n == 2)
            break;
        newCost = computeTotalCost(vlist, d);
        if(newCost == oldCost)
            go = false;

        std::cout << "\nGlobal Cost " << newCost;

    }while(go);

}

void dumpGraph(List<VirtualMachine>* vms, Graph* g){
    FILE *f = fopen("vms.grf", "w");
    List<Vertex>* adj;
    int d, degree;

    for(int i = 0; i < vms->size(); i++){
        degree = 1;
        adj = g->getVertexAdj(vms->get(i)->getId());
        if(adj->size() == 0)
            degree = 0;

        fprintf(f, "%d 1 %d", vms->get(i)->getMem(), degree);
        for(int j =  0; j < adj->size(); j++){
            d = (int)g->getEdge(vms->get(i), adj->get(j));
            fprintf(f, " %d %d", d, adj->get(j)->getId());
        }
        fprintf(f, "\n");
    }
    fclose(f);

}

void makeWorkLoad(List<VirtualMachine>* vms, Graph* traffic, int n, int clustersize){
    int id = 0, mem, hd, swapid, j;
    double d;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> swap(0, n - 1);
    std::uniform_int_distribution<int> lowtraffic(0, 100);
    std::uniform_int_distribution<int> hightraffic(100, 200);
    std::uniform_int_distribution<int> binfactor(0, 1);
    std::uniform_int_distribution<int> e(0, 8);
    std::normal_distribution<double> flow(800, 500);

    std::uniform_int_distribution<int> ram(2, 16);
    std::uniform_int_distribution<int> mips(500, 2000);

    List<VirtualMachine> group(clustersize);

    for(int i = 0; i < n; i++){
        int mem = ram(generator);
        int power = mips(generator);
        vms->add(new VirtualMachine(i, mem, power, 1));
    }

    List<VirtualMachine>* total = vms->copyStatic();

    while(total->size() > 0){
        int p = 0;
        for(int i = 0; i < clustersize; i++){
            if(total->size() == 0)
                break;

            group.add(total->remove(0));
            p++;
        }

        for(int i = 0; i < p - 1; i++){
            for(int j = i+1; j < p; j++){
                int d = flow(generator);
                traffic->insertArc(group.get(i)->getId(), group.get(j)->getId(), d);
            }
        }
        group.clear();
    }

    delete total;
    dumpGraph(vms, traffic);

    traffic->setVertexList((List<Vertex>*)vms->copyStatic());

    for(int i = 0 ; i < vms->size(); i++)
        vms->swap(i, swap(generator));
}

void makeOptmalBP(List<CloudMachine>* pms, List<VirtualMachine>* vms){
    List<CloudMachine>* pmsc = pms->copyStatic();
    List<VirtualMachine>* vmsc = vms->copyStatic();
    List<CloudMachine> sorted (pms->size());

    std::default_random_engine generator;
    std::uniform_int_distribution<int> swap(0, vms->size() - 1);
/*
    for(int i = 0 ; i < vms->size(); i++)
        vmsc->swap(i, swap(generator));*/

    while(pmsc->size() > 0){
        int vol = pmsc->get(0)->getMem() * pmsc->get(0)->getHd();
        int index = 0;
        for(int i = 1; i < pmsc->size(); i++){
            if(vol < pmsc->get(i)->getMem() * pmsc->get(i)->getHd()){
                vol = pmsc->get(i)->getMem() * pmsc->get(i)->getHd();
                index = i;
            }
        }
        sorted.add(pmsc->remove(index));
    }

    while(vmsc->size() > 0){
        VirtualMachine *vm = vmsc->get(0);
        CloudMachine *pm = sorted.get(0);
        if(pm->fit(vm)){
            pm->addVm(vm);
        }else{
            vm->setMem(pm->getMem());
            vm->setHd(pm->getHd());
            pm->addVm(vm);
            sorted.remove(0);
        }
        vmsc->remove(0);
    }

    int n = 0;

    for(int i = 0; i < vms->size(); i++)
        if(!vms->get(i)->isAlloc())
            std::cout << "Fail \n";

    for(int i = 0; i < pms->size(); i++){
        if(pms->get(i)->getVms()->size() > 0){
            pms->get(i)->clear();
            n++;
        }
    }


    std::cout << "Optmal BP: " << n << "\n";

    delete pmsc;
    delete vmsc;
}

int main(int argc, char *argv[]){
    FILE* vmlist, *pmlist;
    vmlist = fopen("vms", "r");
    char[100] algorithm = "First-Fit";
    bool volume = false, networksort = false;

    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i], "-v") == 0)
            volume = true;

        if(strcmp(argv[i], "-nv") == 0){
            volume = true;
            networksort = true;
        }
    }

    //LOAD VMLIST
    int nVms;

    fscanf(vmlist, "%d\n", &nVms);

    Graph* d = new Graph(nVms);
    List<VirtualMachine>* vms = new List<VirtualMachine>(nVms);

    for(int i = 0; i < nVms; i++){
        int mem, hd, mips;
        float c;

        fscanf(vmlist, "%d %d %d ", &mem, &hd, &mips);

        vms->add(new VirtualMachine(i, mem, hd, mips));
        for(int j = 0; j < nVms; j++){
            fscanf(vmlist, "%f ", &c);
            d->insertArc(i, j, c);
        }
    }

    d->setVertexList((List<Vertex>*)vms);

    fclose(vmlist);
    //END LOAD VMLIST


    //LOAD PMLIST
    int npms, k;
    char topology[250];
    pmlist = fopen("pms", "r");
    fscanf(pmlist, "%d %s %d\n", &npms, topology, &k);

    List<CloudMachine>* pms = new List<CloudMachine>(npms);

    Graph* network = buildNetworkTopology(npms, topology, k);

    for(int i = 0; i < npms; i++){
        int mem, hd, mips;

        fscanf(pmlist, "%d %d %d\n", &mem, &hd, &mips);

        pms->add(new CloudMachine(i, mem, hd, mips));
    }

    network->setVertexList((List<Vertex>*)pms);
    fclose(pmlist);

    //END LOAD PMLIST
    VMScheduler *vmp = NULL;

    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i], "klvmp2") == 0){
            vmp = new VMSchedulerMA_NABF((List<VirtualMachine>*)d->getVertexList(), pms, network, d);
            break;
        }

        if(strcmp(argv[i], "klvmp") == 0){
            vmp = new VMSchedulerFFKL((List<VirtualMachine>*)d->getVertexList(), pms, network, d);
            break;
        }

        if(strcmp(argv[i], "ff") == 0){
            vmp = new VMSchedulerFF((List<VirtualMachine>*)d->getVertexList(), pms, network, d);
            break;
        }

         if(strcmp(argv[i], "ffd") == 0){
            vmp = new VMSchedulerFFD((List<VirtualMachine>*)d->getVertexList(), pms, network, d);
            break;
        }
    }

    if(vmp == NULL)
        vmp = new VMSchedulerMA_NABF((List<VirtualMachine>*)d->getVertexList(), pms, network, d);

    clock_t t0 = clock();
    if(volume)
        vmp->volumeSort();

    if(networksort){
        for(int i = 0; i < vmp->getPms()->size(); i++){
            vmp->getPms()->get(i)->setId(i);
        }
    }

    vmp->run();
    double tTotal = (clock() - t0)/(double)CLOCKS_PER_SEC;
    std::cout << "Allocated VMs: " << vmp->allocatedVms() << "\n";
    std::cout << "Total traffic: " << vmp->computeTotalTraffic() << "\n";
    std::cout << "Actives PMs: " << vmp->activesPms() << "\n";
    std::cout << "Packing: " << vmp->checkMemory() << "\n";
    std::cout << "Processing time: " << tTotal;
    vmp->dumpSolution();

    FILE *performance = fopen("performance.txt", "a");

    fprintf(performance, "Algorithm %s \n", argv[1]);
    fprintf(performance, "Allocated VMs: %d \n", vmp->allocatedVms());
    fprintf(performance, "Total traffic: %f \n", vmp->computeTotalTraffic());
    fprintf(performance, "Actives PMs: %d \n", vmp->activesPms());
    fprintf(performance, "Processing time: %f \n\n\n\n", tTotal);

    fclose(performance);

    return 0;
}

