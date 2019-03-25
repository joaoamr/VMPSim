#include <StarGraph.h>
#include <iostream>

double StarGraph::getEdge(Vertex* vi, Vertex* vj){
    return getEdge(vi->getId(), vj->getId());
}

double StarGraph::getEdge(int vi, int vj){
    int i = vi, j = vj;
    double dist;
    if(i == j)
        dist = 0;
    else
        dist = 1;

    return dist;
}

StarGraph::StarGraph(int ports, int size) : Graph(1){
    k = ports;
    n = size;

}
