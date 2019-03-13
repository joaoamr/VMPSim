#include "listlib.h"
#include "Vertex.h"

#define HIGH_TRAFFIC 0
#define LOW_TRAFFIC 1

#ifndef CLOUDMACHINE_H
#define CLOUDMACHINE_H

class VirtualMachine;

class CloudMachine : public Vertex
{
    public:
        CloudMachine(int mid, int memory, int disk, int mips);
        virtual ~CloudMachine();

        int getFullMem() { return fullmem; }
        int getFullHd() { return fullhd; }
        int getFullPower() { return fullpower; }
        int getMem() { return mem; }
        void setMem(int val) { mem = val; }
        int getHd() { return hd; }
        void setHd(int val) { hd = val; }
        int getId() { return id; }
        void setId(int val) { id = val; }
        List<VirtualMachine>* getVms() { return vms; }
        void addVm(VirtualMachine* vm);
        bool fit(VirtualMachine* vm);
        void clear();
        void removeVm(VirtualMachine *vm);
        void setPower(int mips) {power = mips;};
        int getPower(){return power;};


    private:
        int mem;
        int hd;
        int id;
        int power;

        int fullmem;
        int fullhd;
        int fullpower;

        List<VirtualMachine>* vms;
};

#endif // CLOUDMACHINE_H
