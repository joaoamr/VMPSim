#ifndef VMSCHEDULERFFD_H
#define VMSCHEDULERFFD_H

#include <VmPlacement/VMScheduler.h>
#include <VmPlacement/CloudMachine.h>
#include <VmPlacement/VirtualMachine.h>
#include <Graph.h>
#include "listlib.h"



class VMSchedulerFFD : public VMScheduler
{
    public:
        VMSchedulerFFD(List<VirtualMachine>* vmlist, List<CloudMachine>* pmlist, Graph *pmsTopology, Graph *vmsTraffic);
        virtual ~VMSchedulerFFD();
        void run() override;

};

#endif // VMSCHEDULERFFD_H
