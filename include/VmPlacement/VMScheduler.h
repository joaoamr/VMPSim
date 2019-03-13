#ifndef VMSCHEDULER_H
#define VMSCHEDULER_H

#include "Graph.h"
#include "VmPlacement/CloudMachine.h"
#include "VmPlacement/Slot.h"
#include "VmPlacement/VirtualMachine.h"


class VMScheduler
{
    public:
        VMScheduler(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph* pmsTopology, Graph* vmsTraffic);
        virtual ~VMScheduler();
        virtual void run() = 0;

        List<VirtualMachine>* getVms() { return vms; }
        void setVms(List<VirtualMachine>* val) { vms = val; }
        List<CloudMachine>* getPms() { return pms; }
        Graph* getVmsGraph() {return vmsgraph;};
        Graph* getPmsGraph() {return pmsgraph;};
        void setPms(List<CloudMachine>* val) { pms = val; }
        double computeTotalTraffic();
        double computeTotalTraffic(List<VirtualMachine> *vmlist);
        int allocatedVms();
        int activesPms();
        bool checkMemory();
        void dumpSolution();
        void volumeSort();

    protected:
        List<VirtualMachine>* vms;
        List<CloudMachine>* pms;
        List<List<VirtualMachine>>* clusters;
        Graph* pmsgraph;
        Graph* vmsgraph;
        void sortClusters();
        void sortClusters(int size);
        void sortClustersEx();
        void sortVmsByEdge(List<VirtualMachine>* list);
        void sortVmsByMemory(List<VirtualMachine>* list);
        void klCut(List<VirtualMachine>* cluster, List<VirtualMachine>* rest, int size);

};

#endif // VMSCHEDULER_H
