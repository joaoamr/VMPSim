#include "StaticGraph.h"
#include "List.h"
#include "Vertex.h"

StaticGraph::StaticGraph(int n, double** m)
{
    rows = n;
    matrix = m;
    vertexlist = new List<Vertex>(n);
    for(int i = 0; i < n; i++){
        vertexlist->add(new Vertex(i));
    }
}

StaticGraph::~StaticGraph()
{
    delete vertexlist;
}
