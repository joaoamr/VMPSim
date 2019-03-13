#ifndef VMSCHEDULERMA_NABF_H
#define VMSCHEDULERMA_NABF_H

#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <Graph.h>
#include "listlib.h"



class VMSchedulerMA_NABF : public VMScheduler
{
    public:
        VMSchedulerMA_NABF(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph* pmsTopology, Graph* vmsTraffic);
        virtual ~VMSchedulerMA_NABF();
        void run() override;

    private:
        int setK();
        void retryNA(List<VirtualMachine>* tempvms);


};

#endif // VMSCHEDULERMA_NABF_H
