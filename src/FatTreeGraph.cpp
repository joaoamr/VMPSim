#include <FatTreeGraph.h>
#include <iostream>

double FatTreeGraph::getEdge(Vertex* vi, Vertex* vj){
    return getEdge(vi->getId(), vj->getId());
}

double FatTreeGraph::getEdge(int vi, int vj){
    int i = vi, j = vj;
    double dist;
    if(i == j)
        dist = 0;
            else
                if(2*i/k == 2*j/k)
                    dist = 1;
                else
                    if((2*i/k != 2*j/k) && (4*i/(k*k) == 4*j/(k*k)))
                        dist = 3;
                    else
                        if(4*i/(k*k) != 4*j/(k*k))
                            dist  = 5;

    return dist;
}

FatTreeGraph::FatTreeGraph(int ports, int size) : Graph(1){
    k = ports;
    n = size;

}
