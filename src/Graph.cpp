#include <iostream>
#include <string>

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "Graph.h"
#include "listlib.h"
#include "Vertex.h"

using namespace std;

void Graph::buildStaticGraph(int n) {
    enableVertexLabels = 0;
	enableEdgeWeights = 0;
	enableVertexWeights = 0;
	enableMemory = 0;
    V = n;
	A = 0;

	vertexList = new List<Vertex>(n);

    for(int v = 0; v < n; v++){
        vertexList->add(new Vertex(v));
    }

    matrix = new float[n*n];

    for(int i = 0; i < n*n; i++)
        matrix[i] = 0;

}

void Graph::buildDynamicGraph(int n) {
	bool odd = false;
	int v;

	if ((n % 2) != 0) {
		n++;
		odd = true;
	}

	V = n;
	A = 0;
	enableVertexLabels = 0;
	enableEdgeWeights = 0;
	enableVertexWeights = 0;
	enableMemory = 0;

	vertexList = new List<Vertex>(n);
    for(int v = 0; v < n; v++){
        vertexList->add(new Vertex(v));
    }

	adj = new List<List<Vertex> >(n);
	for(int v = 0; v < n; v++){
        adj->add(new List<Vertex>);
    }

	/*if (odd) {
		vertexList[n - 1].setMem(0);
	}*/
}

Graph::Graph(char *fileName) {
    buildGraph(fileName, false);
}

Graph::Graph(int n) {
    dyn = false;
    buildStaticGraph(n);
}

Graph::Graph(char *fileName, bool dynamic) {
    buildGraph(fileName, dynamic);
}

void Graph::buildGraph(char *fileName, bool dynamic){
    dyn = dynamic;
	FILE *fp;
	char *fgetsAux, *fgetsEdge, *fgetsWeight, adjVertex[50], flagAux, adjAux[6000], weightAuxStr[6000];
	int i, weightAux, enableVertexLabels, enableEdgeWeights, enableVertexWeights, enableMemoryWeights, aux, numVertices, weight, v;

	/* Opens SourceGraph file to read SourceGraph data*/
	fp = fopen(fileName, "r");
	if(fp == NULL){
		cout << "SourceGraph file could not be open! \n";
		exit(EXIT_FAILURE);
	}

	// read number of vertices
	fscanf(fp, "%d\n%d ", &numVertices, &numVertices);

	if(dyn)
        buildDynamicGraph(numVertices);
    else
        buildStaticGraph(numVertices);

	// (not used, other graph parameters)
	fscanf(fp, "%s\n%s ", &flagAux, &flagAux);

	// read flags
	flagAux = fgetc(fp);
	enableMemoryWeights = atoi(&flagAux);
	flagAux = fgetc(fp);
	enableVertexLabels = atoi(&flagAux);
	flagAux = fgetc(fp);
	enableEdgeWeights = atoi(&flagAux);
	flagAux = fgetc(fp);
	enableVertexWeights = atoi(&flagAux);
	setFlags(enableVertexLabels, enableEdgeWeights, enableVertexWeights, enableMemoryWeights);

	for(i = 0; i < getNumberOfVertices(); i++) {
		// read memory weights if necessary
		if(enableMemoryWeights != 0) {
			fscanf(fp, "%d ", &weightAux);
			//GraphUpdateMemoryWeight(i, weight_aux);
		}

		// read vertex weights if necessary
		if(enableVertexWeights != 0) {
			fscanf(fp, "%d ", &weightAux);
			//GraphUpdateVertexWeight(i, weight_aux);
		}

		// read vertex degree (not used)
		fscanf(fp, "%d ", &weightAux);

		if(enableEdgeWeights == 0) {
			// read vertex adjacency list
			fgets(adjAux, sizeof(adjAux), fp);
			fgetsAux = strtok(adjAux, " ");
			while(fgetsAux != NULL){
				//GraphInsertEdgeWeight(Source_graph, i, 1);
				insertArc(vertexList->get(i), vertexList->get(atoi(fgetsAux)), 1);
				fgetsAux = strtok(NULL, " ");
			}
		} else {
			// read vertex adjacency list
			fgets(adjAux, sizeof(adjAux), fp);

			fgetsAux = strtok(adjAux, " ");
			int id, weight;

			while(fgetsAux != NULL){
				//GraphInsertEdgeWeight(Source_graph, i, 1);
				weight = atoi(fgetsAux);

				fgetsAux = strtok(NULL, " ");


				id = atoi(fgetsAux);

				//std::cout << i << " " << id << "  " << weight << "\n";

				insertArc(vertexList->get(i), vertexList->get(id), weight);
				fgetsAux = strtok(NULL, " ");
			}
		}
	}

	fclose(fp);
}

int Graph::getNumberOfVertices() {
	return V;
}

/*int DynamicGraph::setEdgeWeight(Vertex v, Vertex w) {
	int weight;
	List<Vertex>* aux;
	Vertex* vaux;

	aux = adj->get(v);
	vaux = aux->get(0);
	if (vaux->getId() == w.getId()) {

	}
}*/

void Graph::setFlags(int vertexLabels, int edgeWeights, int vertexWeights, int memory) {
	enableVertexLabels = vertexLabels;
	enableEdgeWeights = edgeWeights;
	enableVertexWeights = vertexWeights;
	enableMemory = memory;
}

void Graph::insertArc(int v, int w, float edgeWeight) {
    if(dyn){

	}else{
        matrix[v*V + w] = (float)edgeWeight;
        matrix[w*V + v] = (float)edgeWeight;
	}
}


void Graph::insertArc(Vertex *v, Vertex *w, float edgeWeight) {
	if(dyn){
        List<Vertex>* aux;

        aux = getVertexAdj(v->getId());
        aux->add(w);
	}else{
        insertArc(v->getId(), w->getId(), edgeWeight);
	}
}

void Graph::printGraph() {
	int i, j = 0, id;
	List<Vertex>* aux;
	Vertex* vaux;

	cout << "\nSourceGraph: V=" << V << ", A=" << A << ", memory=" << enableMemory << ", vertexLabels=" << enableVertexLabels << ", edgeWeights=" << enableEdgeWeights << ", vertexWeights=" << enableVertexWeights << "\n";

	for (i = 0; i < V; i++) {
		cout << "Vertex[" << i << "]: ";
		aux = adj->get(i);
		vaux = aux->get(j);
		while(vaux != NULL){
			cout << vaux->getId() << " ";
			j++;
			vaux = aux->get(j);
		}
		j = 0;
		//cout << "Memory: " << aux[i]->memory << " ";
		/*if (enableEdgeWeights == 1) {
			cout << "Edge weights: ";
			aux = adj[i];
			while(aux != NULL) {
				cout << aux->get(j)->getEdgeWeight() << " ";
				//aux = aux->next;
				j++;
			}
		}*/
		cout << "\n";
	}
}

List<Vertex>* Graph::getVertexList() {
	return vertexList;
}

List<List<Vertex> >* Graph::getFullAdjList() {
	return adj;
}

List<Vertex>* Graph::getVertexAdj(int i) {
    if(dyn)
        return adj->get(i)->copyDynamic();

    List<Vertex>* vadj = new List<Vertex>(V);
    for(int j = 0 ; j < V; j++)
        if(getEdge(i, j) != 0)
                vadj->add(vertexList->get(j));

    return vadj;
}

double Graph::getEdge(int vi, int vj){
	if(dyn){

	}else{
        return (double)matrix[vi*V + vj];
	}
}

double Graph::getEdge(Vertex* vi, Vertex* vj){
    if(vi->isDummy() || vj->isDummy())
        return 0;

    if(dyn)
        if(adj->get(vi->getId())->contains(vj) != -1)
            return 1;
        else
            return 0;
    else
        return getEdge(vi->getId(), vj->getId());
}

Graph::~Graph() {
	if(dyn){

	}else{
        delete []matrix;
	}
}

void Graph::setVertexList(List<Vertex>* vlist){
    delete vertexList;
    vertexList = vlist->copyStatic();
}

double Graph::getEdgesSum(int v){
    double sum = 0;
    List<Vertex>* vlist = getVertexAdj(v);
    for(int i = 0; i < vlist->size(); i++){
        sum += getEdge(v, vlist->get(i)->getId());
    }
    return sum;
}

/*int main(){
	char fileName[10] = "graph.grf";
	DynamicGraph g(fileName);
	Vertex v(0), w(1), y(2), z(5);

	g.insertArc(&v, &w, 0);
	g.insertArc(&v, &y, 0);
	g.insertArc(&v, &z, 0);
	g.insertArc(&y, &v, 0);
	g.insertArc(&y, &w, 0);
	g.insertArc(&y, &z, 0);
	g.printGraph();

	return 0;
}*/
