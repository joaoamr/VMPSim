#ifndef VMSCHEDULERRR_H
#define VMSCHEDULERRR_H

#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <Graph.h>
#include "listlib.h"



class VMSchedulerRR : public VMScheduler
{
    public:
        VMSchedulerRR(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph* pmsTopology, Graph* vmsTraffic);
        virtual ~VMSchedulerRR();
        void run() override;

    private:
        int last;
};

#endif // VMSCHEDULERRR_H
