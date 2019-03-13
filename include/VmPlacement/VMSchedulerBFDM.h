#ifndef VMSCHEDULERBFDM_H
#define VMSCHEDULERBFDM_H

#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <Graph.h>
#include "listlib.h"



class VMSchedulerBFDM : public VMScheduler
{
    public:
        VMSchedulerBFDM(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph *pmsTopology, Graph *vmsTraffic);
        virtual ~VMSchedulerBFDM();
        void run() override;

};

#endif // VMSCHEDULERBFDM_H
