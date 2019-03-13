#ifndef KLMULTCLUSTER_H
#define KLMULTCLUSTER_H

#include <List.h>
#include <Partition.h>
#include <Graph.h>
#include <Vertex.h>

typedef struct exchange {
    Vertex* a;
    Vertex* b;
    double gain;

} Exchange;

typedef struct movegain {
    Vertex* v;
    Partition* p;
    double gain;
} MoveGain;

class KLMultCluster
{
    public:
        KLMultCluster(List<Partition> *p, List<Vertex> *v, Graph *topology, Graph* cost);
        virtual ~KLMultCluster();

    private:
        List<Partition>* plist;
        List<Vertex>* vlist;
        List<Vertex>* fullvlist;
        List<MoveGain>* movegainlist;
        List<Exchange>* movelist;
        List<List<MoveGain>>* mapList;
        Graph* vgraph;
        Graph* pgraph;
        void findBestMove();
        bool findBestMoveEx();
        bool performKMoves();
        void updateTrafficVector();
        double computeCost();
        double computeGain(Vertex* a, Vertex* b);
        double moveGain(Vertex* v, Partition *p);
        double *trafficVector;
        double totalcost;
        void run();
        void clear();
        void sortMoveGainList(List<MoveGain> *src, int low, int high);
        void processMoveGainList();
        bool* block;
        bool dim = true;
        bool fit(Vertex* a, Vertex* b);

};

#endif // KLMULTCLUSTER_H
