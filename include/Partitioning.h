#include "listlib.h"
#include "Vertex.h"
#include "Graph.h"
#include "Partition.h"
#include "Vertex.h"

#ifndef PARTITIONING_H
#define PARTITIONING_H
#define MODE_MIN 1
#define MODE_MAX -1


typedef struct Move_t{
    Vertex* va;
    Vertex* vb;
    bool moved;
} Move;

class Partitioning {

public:
	Partitioning (Graph *sourceGraph, List<Vertex> *vlist, Partition *pa, Partition *pb);
	double computeCost();
	double computeGlobalCost();
	void lockNodes(Move* m);
	void releaseNodes(Move* m);
	void performMove(Move* m);
	void releaseAllNodes();
	virtual ~Partitioning();
	double getBestCost();
	List<Vertex> *getVertexList() { return vertexList;};
	Partition* getBestA() { return currentA;};
    Partition* getBestB() { return currentB;};
    void printPartition(Partition *p);
    void run();
    void setInitialPartitioning(Partition *pa, Partition *pb);
    bool performedMoves(){return moves;};
    void setMaxExternal(double val) {globalMax = val;};
    void setMode(int m) {mode = m;};
    void setVertexList (List<Vertex>* vlist);
    void setHops(int val) {hops = val;};
    void flush();
    void setPartitionTopology (Graph* pt) {partitionTopology = pt;};

private:
    bool autoP;
    double oldGlobal;
    double globalMax;
    List<Vertex>* vertexList;
    int maxId;
	int numberOfPartitions;
	int step;
	bool moves;
	void findBestMove(Move* m);
	Graph* source;
	Graph* partitionTopology;
	Partition* initialA;
	Partition* initialB;
	Partition* currentA;
	Partition* currentB;
	void setInitialPartitioning();
	void sortPartition(Partition* p, int begin, int end);
	void mergeSort(Partition* p, int begin, int end);
	bool performKMoves();
	bool* lock;
	int *edges;
	Move* move;
	void computeDifferential(Vertex* v, Partition *external, Partition *internal);
	void updateDifferentialVector();
	bool isLock(Vertex* v) {return lock[v->getId()];};
	double oldCost;
	double newCost;
	double bestCost;
	double* differential;
	int steps;
	int step_i;
	bool restart();
	void quickSort(Partition *p, int low, int high);
	int mode;
    int hops;

};

#endif // PARTITIONING_H
