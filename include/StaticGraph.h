#include "listlib.h"
#include "Vertex.h"
#ifndef STATICGRAPH_H
#define STATICGRAPH_H


class StaticGraph
{
    public:
        StaticGraph(char* file);
        StaticGraph(int n, double** m);
        virtual ~StaticGraph();
        double** getMatrix() { return matrix; };
        double getEdge(int vi, int vj) {return matrix[vi][vj];};
        List<Vertex>* getVertexList(){return vertexlist;};

    private:
        double** matrix;
        int rows;
        List<Vertex>* vertexlist;
};

#endif // STATICGRAPH_H
