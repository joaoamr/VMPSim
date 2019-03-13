#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include "Vertex.h"

class CloudMachine;

class VirtualMachine : public Vertex
{
    public:
        VirtualMachine(int vid, int memory, int disk, int mips);
        VirtualMachine(int vid, int memory, int disk, int mips, char* lbl);
        VirtualMachine(int vid);
        virtual ~VirtualMachine();

        int getMem() { return mem; }
        void setMem(int val) { mem = val; }
        int getHd() { return hd; }
        void setHd(int val) { hd = val; }
        void setHost(CloudMachine* mac);
        CloudMachine *getHost() {return host;};
        bool isAlloc() {return alloc;}
        char *getLabel(){return label;}
        void setLabel(char* val){label = val;};
        void setGroup(int val) {group = val;};
        int getGroup(){return group;};
        void setAlloc(bool b) {alloc = b;};
        void setPower(int mips) {power = mips;};
        int getPower(){return power;};


    private:
        int mem;
        int hd;
        CloudMachine* host;
        bool alloc;
        char* label;
        int group;
        int power;
};

#endif // VIRTUALMACHINE_H
