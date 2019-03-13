#ifndef VMSCHEDULERKLFLAT_H
#define VMSCHEDULERKLFLAT_H

#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <Graph.h>
#include "listlib.h"



class VMSchedulerKLFlat : public VMScheduler
{
    private:
        double computeTotalCost();
        double computeAdditionalTotalCost(List<Vertex> *vmlist, List<Vertex> *addlist);
        double computePartitionCost(Partition* p1, Partition *p2);
        void fit();
        void performKL2C();
        void performKLMC();
        void retryFF();
        void retryNA();
        void initialPartFF();
        void initialPartNABF();
        void initialPartRR();
        void initialPart(char* file);
        void checkInitialPart();
        List<Vertex>* tempvms;
        List<CloudMachine>* expanded;

    public:
        VMSchedulerKLFlat(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph *pmsTopology, Graph *vmsTraffic);
        virtual ~VMSchedulerKLFlat();
        List<Partition>* plist;
        List<CloudMachine>* getExpanded();
        void run() override;
        void sortVms();
        int lastsize;
        List<Vertex>* vms;
        void sortVmsByMemory(int low, int high);
        void divideAndConquerPlacement();
        void totalPlacement();
        int mode = 0;
};

#endif // VMSCHEDULERKLFLAT_H
