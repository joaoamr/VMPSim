#include <Graph.h>

#ifndef STARGRAPH_H
#define STARGRAPH_H

class StarGraph : public Graph{
    private:
        int k;
        int n;

    public:
        double getEdge(Vertex* vi, Vertex* vj) override;
        double getEdge(int vi, int vj) override;
        StarGraph(int ports, int size);

};

#endif
