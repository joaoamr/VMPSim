#ifndef VMSCHEDULERFF_H
#define VMSCHEDULERFF_H

#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <Graph.h>
#include "listlib.h"



class VMSchedulerFF : public VMScheduler
{
    public:
        VMSchedulerFF(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph *pmsTopology, Graph *vmsTraffic);
        virtual ~VMSchedulerFF();
        void run() override;

};

#endif // VMSCHEDULERFF_H
