#ifndef VMSCHEDULERFFKL_H
#define VMSCHEDULERFFKL_H

#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <Graph.h>
#include "listlib.h"



class VMSchedulerFFKL : public VMScheduler
{
    public:
        VMSchedulerFFKL(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph *pmsTopology, Graph *vmsTraffic);
        virtual ~VMSchedulerFFKL();
        void run() override;
        void volumeSort();

    private:
        void bfApm(VirtualMachine *vm);
        CloudMachine *bestFit(VirtualMachine* vm, List<CloudMachine> *target);
        CloudMachine *firstFitDecreasing(VirtualMachine* vm, List<CloudMachine> *target);
        List<CloudMachine>* activesHosts;
        List<CloudMachine>* inactivesHosts;
        void placeVm(VirtualMachine* vm, CloudMachine* pm);
        int setK();
        void networkSort(List<CloudMachine>* pms);

};

#endif // VMSCHEDULERFFKL_H
