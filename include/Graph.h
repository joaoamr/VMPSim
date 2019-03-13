#include "listlib.h"
#include "Vertex.h"
#include <string>

#ifndef DYNGRAPH_H
#define DYNGRAPH_H

class Graph {

public:
	Graph(char *fileName);
	Graph(char *fileName, bool dynamic);
	Graph(int n);
	Graph(){};
	int getNumberOfVertices();
	void setFlags(int vertexLabels, int edgeWeights, int vertexWeights, int memory);
	//TODO int getEdgeWeight(Vertex v, Vertex w);
	void insertArc(Vertex *v, Vertex *w, float edgeWeight);
	void printGraph();
	List<Vertex>* getVertexList();
	List<List<Vertex> >* getFullAdjList();
	List<Vertex>* getVertexAdj(int i);
	virtual ~Graph(); 							  //TODO
	virtual double getEdge(Vertex* vi, Vertex* vj);
	virtual double getEdge(int vi, int vj);
	void setVertexList(List<Vertex>* vlist);
	void insertArc(int v, int w, float edgeWeight);
	double getEdgesSum(int v);

private:
	int V;
	int A;
	int enableVertexLabels;
	int enableEdgeWeights;
	int enableVertexWeights;
	int enableMemory;
	List<Vertex>* vertexList;
	List<List<Vertex> >* adj;
	Vertex* vvector;
	float* matrix;
	void buildDynamicGraph(int n);
	void buildStaticGraph(int n);
	void buildGraph(char* fileName, bool dynamic);
	bool dyn;
};

#endif // DYNGRAPH_H
