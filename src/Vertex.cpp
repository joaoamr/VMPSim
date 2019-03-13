#include "Vertex.h"

Vertex::Vertex(int vid)
{
    id = vid;
    dummy = false;
}


Vertex::Vertex(int vid, bool d)
{
    id = vid;
    dummy = d;
}

Vertex::Vertex(int vid, int d1, int d2, int d3, bool d){
    id = vid;
    dummy = false;
    this->d1 = d1;
    this->d2 = d2;
    this->d3 = d3;
}

Vertex::~Vertex()
{

}

void Vertex::setPartition(Partition *newp){
    if(p != 0){
        p->setD1(p->getD1() + d1);
        p->setD2(p->getD2() + d2);
        p->setD3(p->getD3() + d3);
    }

    p = newp;
    newp->setD1(newp->getD1() - d1);
    newp->setD2(newp->getD2() - d2);
    newp->setD3(newp->getD3() - d3);
}

int Vertex::getD1(){
    return d1;
}

int Vertex::getD2(){
    return d2;
}

int Vertex::getD3(){
    return d3;
}

bool Vertex::isDummy(){
    return dummy;
}

Partition *Vertex::getPartition(){
    return p;
}
