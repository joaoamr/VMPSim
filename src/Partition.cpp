#include "Partition.h"
#include "Vertex.h"
#include "listlib.h"
#include <iostream>

Partition::Partition(int pid, int totalsize)
{
    psize = totalsize;
    id = pid;
    vertexlist = new List<Vertex>(totalsize);
}

Partition::~Partition()
{
    delete vertexlist;
}

Partition* Partition::copyPartition()
{
    Partition* p = new Partition(id, psize);
    p->setD1(d1);
    p->setD1(d2);
    p->setD1(d3);

    for(int i = 0; i < vertexlist->size(); i++)
        p->getVertexList()->add(vertexlist->get(i));

    return p;
}

int Partition::getD1(){
    return d1;
}

int Partition::getD2(){
    return d2;
}

int Partition::getD3(){
    return d3;
}

void Partition::setD1(int d){
    return d1 = d;
}

void Partition::setD2(int d){
    return d2 = d;
}

void Partition::setD3(int d){
    return d3 = d;
}

unsigned int Partition::getId() {
    return id;
}

void Partition::setId(unsigned int val) {
    id = val;
}

void Partition::setVertexList(List<Vertex>* val) {
    vertexlist = val;
}

List<Vertex>* Partition::getVertexList() {
    return vertexlist;
}
