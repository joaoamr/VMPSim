#ifndef VMSCHEDULERNABF_H
#define VMSCHEDULERNABF_H

#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <Graph.h>
#include "listlib.h"



class VMSchedulerNABF : public VMScheduler
{
    public:
        VMSchedulerNABF(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph* pmsTopology, Graph* vmsTraffic);
        virtual ~VMSchedulerNABF();
        void run() override;

    private:
        int success = 0;
        List<CloudMachine>* activesHosts;
        List<CloudMachine>* inactivesHosts;
        bool* firstpm;
        List<List<CloudMachine>> *pmsToCluster;
        CloudMachine *lastpm;
        void naFit(List<VirtualMachine>* list);
        void bfApm(VirtualMachine* vm);
        void restartNaVector();
        void placeVm(VirtualMachine* vm, CloudMachine* pm);
        CloudMachine *expandCluster(VirtualMachine* vm);
        CloudMachine *nextPm(VirtualMachine *vm, CloudMachine *start);
        CloudMachine *bestFit(VirtualMachine* vm, List<CloudMachine> *target);
        CloudMachine *firstFitDecreasing(VirtualMachine* vm, List<CloudMachine> *target);
        Graph* tiny(Graph* big, List<VirtualMachine>* vmlist, int n);

};

#endif // VMSCHEDULERNABF_H
