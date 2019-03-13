#ifndef VERTEX_H
#define VERTEX_H
#include "Partition.h"

class Vertex
{
    public:
        Vertex(int vid);
        Vertex(int vid, bool d);
        Vertex(int vid, int d1, int d2, int d3, bool d);
        virtual ~Vertex();

        int getId() { return id; };
        void setId(int val) { id = val; };
        int getPartitionId() { return p->getId(); };
        Partition *getPartition();
        void setPartition(Partition *newp);
        bool isDummy();
        int getD1();
        int getD2();
        int getD3();


    private:
        int id;
        Partition *p = 0;
        bool dummy;
        int d1;
        int d2;
        int d3;
};

#endif // VERTEX_H
