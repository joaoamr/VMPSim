#include "listlib.h"

#ifndef PARTITION_H
#define PARTITION_H

class Vertex;

class Partition
{
    public:
        Partition(int pid, int totalsize);
        virtual ~Partition();

        unsigned int getId();
        void setId(unsigned int val);
        List<Vertex>* getVertexList();
        void setVertexList(List<Vertex>* val);
        Partition *copyPartition();
        int size() {return vertexlist->size();};
        int getD1();
        int getD2();
        int getD3();
        void setD1(int d);
        void setD2(int d);
        void setD3(int d);

    private:
        unsigned int id;
        List<Vertex>* vertexlist;
        int psize;
        int d1;
        int d2;
        int d3;
};

#endif // PARTITION_H
