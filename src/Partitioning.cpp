#include "Partitioning.h"
#include "listlib.h"
#include "Vertex.h"
#include "Partition.h"
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

Partitioning::Partitioning(Graph* sourceGraph, List<Vertex> *vlist, Partition* pa, Partition *pb){
    hops = 1;
    partitionTopology = NULL;
    mode = MODE_MIN;
    autoP = false;
    source = sourceGraph;
    int n = vlist->size();
    vertexList = vlist;
    maxId = vlist->get(0)->getId();
    globalMax = -1;

    for(int i = 1; i < vlist->size(); i++)
        if(vlist->get(i)->getId() > maxId)
            maxId = vlist->get(i)->getId();

    for(int i = 0; i < sourceGraph->getVertexList()->size(); i++)
        if(sourceGraph->getVertexList()->get(i)->getId() > maxId)
            maxId = sourceGraph->getVertexList()->get(i)->getId();

    maxId++;

    lock = new bool[maxId];

    differential = new double[maxId];

    initialA = pa;
    initialB = pb;
}

void Partitioning::run(){
    moves = false;
    double firstMoveCost;
    //Set initial partitioning
    if(initialA == NULL || initialB == NULL)
        setInitialPartitioning();

    for(int i = 0; i < maxId; i++)
        lock[i] = true;

    for(int i = 0; i < currentA->getVertexList()->size(); i++){
        lock[currentA->getVertexList()->get(i)->getId()] = false;
        currentA->getVertexList()->get(i)->setPartition(currentA);
    }

    for(int i = 0; i < currentB->getVertexList()->size(); i++){
        lock[currentB->getVertexList()->get(i)->getId()] = false;
        currentB->getVertexList()->get(i)->setPartition(currentB);
    }

    if(currentA->size() + currentB->size() < 3){
        bestCost = computeCost();
        moves = false;
        return;
    }

    if(currentA->size() > currentB->size())
        steps = currentB->size();
    else
        steps = currentA->size();

    move = new Move[steps];

    for(int i = 0; i < steps; i++){
        move[i].va = NULL;
        move[i].vb = NULL;
        move[i].moved = false;
    }

    bestCost = -1;
    newCost = -2;

    firstMoveCost = computeCost();
    //std::cout << "Initial Partitioning cost: " << firstMoveCost << " Number of vertex: " << initialA->size() + initialB->size() << "\n\n";
    bool go = true;
    while(go){
        if(globalMax != -1){
            oldGlobal = computeGlobalCost();
            if(oldGlobal*mode > globalMax*mode){
                moves = false;
                bestCost = computeCost();
                restart();
                delete[] move;
                return;
            }

        }

        oldCost = computeCost();
        if(firstMoveCost == 0 && mode == MODE_MIN){
            bestCost = 0;
            delete[] move;
            moves = false;
            return;
        }
        //Calculate D for all nodes (except the blocked ones)
        for(step_i = 0; step_i < steps; step_i++){
            updateDifferentialVector();
            //Search for a pair to exchange
            findBestMove(&move[step_i]);
            //std::cout << "\nstep " << step_i << " Pair found: " << move[step_i].va->getId() << " & " << move[step_i].vb->getId() << " actual cost: " << oldCost <<"\n";

            //Lock the pair found
            lockNodes(&move[step_i]);
        }
        //Release blocked nodes
        releaseAllNodes();

        //Choose K and perform moves
        go = performKMoves();

        //Restart current partitions A and B
        go = restart();

        newCost = computeCost();

        if(newCost*mode >= oldCost*mode){
             go = false;
             bestCost = oldCost;
        }else{
             go = true;
             bestCost = newCost;
        }

    }
    if(firstMoveCost == bestCost)
        moves = false;
    else
        moves = true;

    delete[] move;
    //std::cout << "\nEnd\n";
}

bool Partitioning::performKMoves(){
    int k = 0;
    double g = differential[move[0].va->getId()] + differential[move[0].vb->getId()] - 2*source->getEdge(move[0].va, move[0].vb);
    double Gmax = g;
    if(Gmax*mode >= 0)
        k = 1;

    for(int i = 1; i < steps; i++){
        g += differential[move[i].va->getId()] + differential[move[i].vb->getId()] - 2*source->getEdge(move[i].va, move[i].vb);
        if(g*mode >= Gmax*mode){
            k = i + 1;
            Gmax = g;
        }
    }

    if(Gmax*mode >= 0){
        for(int i = 0; i < k; i++){
            performMove(&move[i]);
        }
        return true;
    }
    else
        return false;
}

void Partitioning::setInitialPartitioning(){
    initialA = new Partition(0, vertexList->size()/2 + 1);
    initialB = new Partition(1, vertexList->size()/2 + 1);
    autoP = true;
    List<Vertex>* vlist = vertexList;
    //Round-Robin

    bool rr = false;
    for(int i = 0; i <  vlist->size(); i++){
        if(!rr){
            initialA->getVertexList()->add(vlist->get(i));
            vlist->get(i)->setPartition(initialA);
            rr = true;
        }else{
            initialB->getVertexList()->add(vlist->get(i));
            vlist->get(i)->setPartition(initialB);
            rr = false;
        }
    }
    //FirstFit
    /*
    for(int i = 0; i <  vlist->size()/2; i++){
        initialA->getVertexList()->add(vlist->get(i));
        vlist->get(i)->setPartition(initialA);
    }

    for(int i = vlist->size()/2; i <  vlist->size(); i++){
        initialB->getVertexList()->add(vlist->get(i));
        vlist->get(i)->setPartition(initialB);
    }
*/
    currentA = initialA;//->copyPartition();
    currentB = initialB;//->copyPartition();

}

void Partitioning::updateDifferentialVector() {
    if(step_i == 0){
        for(int i = 0; i < currentA->getVertexList()->size(); i++){
            computeDifferential(currentA->getVertexList()->get(i), currentB, currentA);
        }

        for(int i = 0; i < currentB->getVertexList()->size(); i++){
            computeDifferential(currentB->getVertexList()->get(i), currentA, currentB);
        }
    }else{
        Vertex* va = move[step_i - 1].va;
        Vertex* vb = move[step_i - 1].vb;

        List<Vertex>* adja = source->getVertexAdj(va->getId());
        List<Vertex>* adjb = source->getVertexAdj(vb->getId());

        for(int i = 0; i < adja->size(); i++){
            if(isLock(adja->get(i)))
                continue;

            if(adja->get(i)->getPartition() == va->getPartition()){
                differential[adja->get(i)->getId()] += 2*source->getEdge(va, adja->get(i));
            }
            else{
                differential[adja->get(i)->getId()] -= 2*source->getEdge(va, adja->get(i));
            }
        }
        for(int i = 0; i < adjb->size(); i++){
            if(isLock(adjb->get(i)))
                continue;
            if(adjb->get(i)->getPartition() == vb->getPartition()){
                differential[adjb->get(i)->getId()] += 2*source->getEdge(vb, adjb->get(i));
            }
            else
                differential[adjb->get(i)->getId()] -= 2*source->getEdge(vb, adjb->get(i));
        }
        delete adja;
        delete adjb;
    }
}

void Partitioning::computeDifferential(Vertex* v, Partition *external, Partition *internal){
    if(isLock(v))
        return;

    double d = 0;
    Vertex* vi;

    for(int j = 0;  j < external->getVertexList()->size() ; j++){
        vi = external->getVertexList()->get(j);
        if(!isLock(vi))
            d += source->getEdge(v, vi);
    }

    for(int j = 0;  j < internal->getVertexList()->size() ; j++){
        vi = internal->getVertexList()->get(j);
        if(!isLock(vi))
            d -= source->getEdge(v, vi);
    }
    differential[v->getId()] = d;
}

double Partitioning::computeCost(){
    Vertex* va, *vb;
    double cost = 0;

    for(int i = 0;  i < currentA->getVertexList()->size(); i++){
        va = currentA->getVertexList()->get(i);
        for(int j = 0;  j < currentB->getVertexList()->size(); j++){
            vb = currentB->getVertexList()->get(j);
            cost += source->getEdge(va, vb)*hops;
        }
    }
    return cost;
}

void Partitioning::lockNodes(Move* m){
    if(m->va != NULL)
        lock[m->va->getId()] = true;
    if(m->vb != NULL)
        lock[m->vb->getId()] = true;
}

void Partitioning::releaseNodes(Move* m){
    if(m->va != NULL)
        lock[m->va->getId()] = false;
    if(m->vb != NULL)
        lock[m->vb->getId()] = false;
}

void Partitioning::releaseAllNodes(){
    for(int i = 0; i < steps; i++){
        lock[move[i].va->getId()] = false;
        lock[move[i].vb->getId()] = false;
    }
}

void Partitioning::performMove(Move* m){
    m->va->setPartition(currentB);
    currentB->getVertexList()->add(m->va);

    m->vb->setPartition(currentA);
    currentA->getVertexList()->add(m->vb);

    m->moved = true;
}

void Partitioning::findBestMove(Move* m){
    Vertex *maxa, *maxb, *va, *vb;
    double gmax, g;
    bool gmaxset = false, best = false;
    int aIndex, bIndex;
    quickSort(currentA, 0 , currentA->size()-1);
    quickSort(currentB, 0 , currentB->size()-1);
/*
    for(int i = 0;  i < currentA->size(); i++)
        std::cout << differential[currentA->getVertexList()->get(i)->getId()] << " ";

    std::cout << "\n";

    std::cin >> aIndex;*/
    for(int i = 0;  i < currentA->size() && !best; i++){
        va = currentA->getVertexList()->get(i);
        if(isLock(va))
            continue;

        for(int j = 0; j < currentB->size() && !best; j++){
            vb = currentB->getVertexList()->get(j);
            if(isLock(vb))
                continue;

            g = differential[va->getId()] + differential[vb->getId()] - 2*source->getEdge(va, vb);
            if(!gmaxset){
                aIndex = i;
                bIndex = j;
                maxa = va;
                maxb = vb;
                gmax = g;
                gmaxset = true;
            }else
                if(g*mode > gmax*mode){
                    aIndex = i;
                    bIndex = j;
                    maxa = va;
                    maxb = vb;
                    gmax = g;
                }
                else{
                     best = true;
                }
        }
    }

    currentA->getVertexList()->fastRemove(aIndex);
    currentB->getVertexList()->fastRemove(bIndex);

    m->va = maxa;
    m->vb = maxb;
}

double Partitioning::getBestCost(){
    return bestCost;
}

void Partitioning::sortPartition(Partition* p, int begin, int end){
 	int i, j;
	double pivot;
	i = begin;
	j = end-1;
	int iId = p->getVertexList()->get(i)->getId();
	int jId = p->getVertexList()->get(j)->getId();
	pivot = differential[(iId + jId) / 2];
	while(i <= j)
	{
		while(differential[p->getVertexList()->get(i)->getId()] > pivot && i < end)
			i++;

		while(differential[p->getVertexList()->get(j)->getId()] < pivot && j > begin)
			j--;

		if(i <= j){
			p->getVertexList()->swap(i, j);
			i++;
			j--;
		}
	}
	if(j > begin)
		sortPartition(p, begin, j+1);
	if(i < end)
		sortPartition(p, i, end);
}


void Partitioning::quickSort(Partition *p, int low, int high){
    if (low < high) {
        double pivot = differential[p->getVertexList()->get(high)->getId()];
        int i = (low - 1);

        for (int j = low; j <= high - 1; j++){
            if(differential[p->getVertexList()->get(j)->getId()]*mode > pivot*mode){
                i++;
                p->getVertexList()->swap(i, j);
            }
        }
        p->getVertexList()->swap(i + 1, high);
        int pi = i + 1;
        quickSort(p, low, pi - 1);
        quickSort(p, pi + 1, high);
    }
}

void Partitioning::mergeSort(Partition* p, int begin, int end){
    unsigned int* index = new unsigned int[p->size()];
    double* temp = new double[p->size()];


    delete[] index;
    delete[] temp;
}


void Partitioning::printPartition(Partition *p){
    std::cout << p->getId() << ": ";
    for(int i = 0; i < p->size(); i++)
        //if(!p->getVertexList()->get(i)->isDummy())
            std::cout << p->getVertexList()->get(i)->getId() << " ";

    std::cout << "\n";

}

bool Partitioning::restart(){
    bool backup = false;

    if(globalMax != -1)
        if(computeGlobalCost() > oldGlobal)
            backup = true;


    for(int i = 0 ; i < steps; i++){
        if(!move[i].moved || backup){
            currentA->getVertexList()->add(move[i].va);
            move[i].va->setPartition(currentA);
            currentB->getVertexList()->add(move[i].vb);
            move[i].vb->setPartition(currentB);
            if(move[i].moved){
                currentB->getVertexList()->remove(move[i].va);
                currentA->getVertexList()->remove(move[i].vb);
            }
        }
        move[i].moved = false;
    }
    return !backup;
}

void Partitioning::flush(){
    if(autoP){
        delete initialA;
        delete initialB;

        delete currentA;
        delete currentB;
    }

    initialA = NULL;
    initialB = NULL;

    currentA = NULL;
    currentB = NULL;

}

void Partitioning::setInitialPartitioning(Partition* pa, Partition *pb){
    if(autoP){
        delete initialA;
        delete initialB;
    }

    initialA = pa;
    initialB = pb;

    currentA = pa;
    currentB = pb;

    autoP = false;

}

double Partitioning::computeGlobalCost(){
    double d = 0;
    double cost;
    for(int i = 0; i < vertexList->size(); i++)
        for(int j = i; j < vertexList->size(); j++)
            if(vertexList->get(i)->getPartition() != vertexList->get(j)->getPartition()){
                if (partitionTopology == NULL)
                    cost = 1;
                else
                    cost = partitionTopology->getEdge(vertexList->get(i)->getPartitionId(), vertexList->get(j)->getPartitionId());

                d += source->getEdge(vertexList->get(i), vertexList->get(j))*cost;
            }

    return d;
}

Partitioning::~Partitioning(){
    delete[] lock;
    delete[] differential;
    //delete currentA;
    //delete currentB;
    if(autoP){
        delete initialA;
        delete initialB;
    }
}

void Partitioning::setVertexList (List<Vertex>* vlist){
    vertexList = vlist;
}
