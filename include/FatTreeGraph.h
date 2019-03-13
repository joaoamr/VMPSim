#include <Graph.h>

#ifndef FATTREEGRAPH_H
#define FATTREEGRAPH_H

class FatTreeGraph : public Graph{
    private:
        int k;
        int n;

    public:
        double getEdge(Vertex* vi, Vertex* vj) override;
        double getEdge(int vi, int vj) override;
        FatTreeGraph(int ports, int size);

};

#endif
