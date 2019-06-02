#include <KLMultCluster.h>

#include <List.h>
#include <Partition.h>
#include <Graph.h>
#include <Vertex.h>
#include <iostream>

KLMultCluster::KLMultCluster(List<Partition> *p, List<Vertex> *v, Graph *topology, Graph* cost){
    plist = p;
    vlist = v;
    fullvlist = v->copyStatic();
    pgraph = topology;
    vgraph = cost;
    totalcost = -1;

    int pairs = v->size();

    movelist = new List<Exchange>(pairs);

    int maxId = vlist->get(0)->getId();

    for(int i = 1; i < vlist->size(); i++)
        if(vlist->get(i)->getId() > maxId)
            maxId = vlist->get(i)->getId();


    trafficVector = new double[maxId + 1];
    block = new bool[maxId + 1];

    maxId = plist->get(0)->getId();

    for(int i = 1; i < plist->size(); i++)
        if(plist->get(i)->getId() > maxId)
            maxId = plist->get(i)->getId();

    mapList = new List<List<MoveGain>>(maxId + 1);

    for(int i = 0; i < maxId + 1; i++)
        mapList->add(new List<MoveGain>(vlist->size()));

    movegainlist = new List<MoveGain>(p->size() * (maxId + 1));

    run();
}

KLMultCluster::~KLMultCluster(){
    delete[] block;
    delete[] trafficVector;

    delete movelist;

    delete movegainlist;

    delete mapList;

    delete fullvlist;
}

void KLMultCluster::run(){
    double oldCost = -1, newCost = -2;
    bool go = true;

    while(go){
        oldCost = computeCost();

        if(oldCost == 0)
            return;

        std::cout << "cost " << oldCost;
        while(vlist->size() > 1){
            if(!findBestMoveEx())
                break;
        }
        std::cout << "moving";
        go = performKMoves();
        if(go)
            if(oldCost <= computeCost())
                go = false;

        std::cout << " go " << go << "\n ";
    }
    std::cout << "Final cost: " << computeCost() << "\n";
}

double KLMultCluster::computeCost(){
    Vertex* va, *vb;
    double cost = 0;

    for(int i = 0;  i < vlist->size() - 1; i++){
        for(int j = i ; j < vlist->size(); j++){
            va = vlist->get(i);
            vb = vlist->get(j);
            cost += vgraph->getEdge(va, vb)*pgraph->getEdge(va->getPartitionId(), vb->getPartitionId());
        }
    }
    return cost;
}

double KLMultCluster::computeGain(Vertex *a, Vertex *b){
    double gain;

    Partition *pa = a->getPartition(), *pb = b->getPartition();

    a->setPartition(pb);
    b->setPartition(pa);

    gain = moveGain(a, pb) + moveGain(b, pa);

    b->setPartition(pb);
    a->setPartition(pa);

    return gain;
}

void KLMultCluster::findBestMove(){
    Vertex *a, *b;
    double gain;

    double gmax = computeGain(vlist->get(0), vlist->get(1)), g;
    int vi = 0, vj = 1;

    for(int i = 0; i < vlist->size() - 1; i++){
        for(int j = i + 1; j < vlist->size(); j++){
            if(vlist->get(i)->getPartition() == vlist->get(j)->getPartition())
                g = 0;
            else
                g = computeGain(vlist->get(i), vlist->get(j));
            if(gmax < g){
                gmax = g;
                vi = i;
                vj = j;
            }
        }
    }
    /*a = movegainlist->get(0)->v;

    for(int i = 1; i < movegainlist->size(); i++)
        if(movegainlist->get(i)->v != a && movegainlist->get(i)->p != a->getPartition()){
            b = movegainlist->get(i)->v;
            gain = movegainlist->get(0)->gain + movegainlist->get(i)->gain;
            break;
        }
    */
    a = vlist->get(vi);
    b = vlist->get(vj);

    vlist->remove(a);
    vlist->remove(b);

    Exchange* ex = new Exchange;
    ex->a = a;
    ex->b = b;
    ex->gain = gmax;

    movelist->add(ex);
    //std::cout << "Pair found: " << a->getId() << " & " << b->getId() << " pairs " << movelist->size() << "\n";
}

bool KLMultCluster::findBestMoveEx(){
    updateTrafficVector();
    processMoveGainList();

    Vertex *a, *b, *va, *vb;
    double gain = -1;
    bool gainset = false;
    int n = vlist->size();
    MoveGain *mgb = NULL;
    List<MoveGain> *part;
    int c = 2;
    bool go = true;

    for(int i = 0; i < movegainlist->size() - 1 && go; i++){
            if(block[movegainlist->get(i)->v->getId()])
                continue;

            va = movegainlist->get(i)->v;

            if(mapList->get(movegainlist->get(i)->p->getId())->size() == 0)
                continue;

            part = mapList->get(movegainlist->get(i)->p->getId());
            //part = movegainlist;

            for(int j = 0; j < part->size(); j++){
                mgb = part->get(j);
                if(mgb == NULL)
                    continue;

                vb = mgb->v;

                mgb = part->get(j);

                if(vb == va)
                    continue;

                if(!gainset){
                    if(fit(va, vb) &&
                       va->getPartition() == mgb->p
                       && vb->getPartition() == movegainlist->get(i)->p && va != vb){
                            gain = mgb->gain + movegainlist->get(i)->gain - c*vgraph->getEdge(va,vb)*pgraph->getEdge(va->getPartitionId(), vb->getPartitionId());
                            a = va;
                            b = vb;
                            gainset = true;
                       }

                }else{
                    if(fit(va, vb) &&
                       va->getPartition() == mgb->p && mgb->v->getPartition() == movegainlist->get(i)->p && va != vb){
                        if(mgb->gain + movegainlist->get(i)->gain - c*vgraph->getEdge(va,vb)*pgraph->getEdge(va->getPartitionId(), vb->getPartitionId()) > gain){
                            gain = mgb->gain + movegainlist->get(i)->gain - c*vgraph->getEdge(va,vb)*pgraph->getEdge(va->getPartitionId(), vb->getPartitionId());
                            a = va;
                            b = vb;
                        }else{
                            //go = false;
                            break;
                        }
                        }
                       }


            }
    }

    if(!gainset)
        return false;

    //vlist->remove(a);
    //vlist->remove(b);

    Exchange* ex = new Exchange;
    ex->a = a;
    ex->b = b;
    ex->gain = gain;

    Partition* pa = a->getPartition();
    a->setPartition(b->getPartition());
    b->setPartition(pa);

    a->getPartition()->getVertexList()->remove(b);
    a->getPartition()->getVertexList()->add(a);

    b->getPartition()->getVertexList()->remove(a);
    b->getPartition()->getVertexList()->add(b);

    movelist->add(ex);
    //std::cout << "Pair found: " << a->getId() << " "<< a->getPartitionId() <<"-->"<< b->getPartitionId() << " " << " & " << b->getId() << " "<< b->getPartitionId() << "-->" << a->getPartitionId() << " movelist: " << movelist->size() << " gain: " << ex->gain << "\n";
    return true;
}

bool KLMultCluster::performKMoves(){
    int k = 1;
    if(movelist->size() == 0)
        return false;

    double gmax = movelist->get(0)->gain, g = movelist->get(0)->gain;
    Partition *pa, *pb;
    Vertex *a, *b;

    for(int i = 1; i < movelist->size(); i++){
        g += movelist->get(i)->gain;
        if(g > gmax){
            k = i + 1;
            gmax = g;
        }
    }

    if(gmax <= 0){
        clear();
        return false;
    }

    for(int i = 0; i < k; i++){
        delete movelist->remove(0);

    }

    clear();
    return true;
}

void KLMultCluster::clear(){
    Vertex *a, *b;
    Partition *pa, *pb;

    while(movelist->size() > 0){
        a = movelist->get(0)->a;
        b = movelist->get(0)->b;

        //vlist->add(a);
        //vlist->add(b);

        pa = a->getPartition();
        pa->getVertexList()->remove(a);
        pa->getVertexList()->add(b);

        pb = b->getPartition();
        pb->getVertexList()->remove(b);
        pb->getVertexList()->add(a);


        a->setPartition(pb);
        b->setPartition(pa);


        delete movelist->remove(0);
    }

    for(int i = 0; i < mapList->size(); i++){
        mapList->get(i)->clear();
    }

    while(movegainlist->size() > 0)
        delete movegainlist->remove(0);
}

double KLMultCluster::moveGain(Vertex* v, Partition *p){
    double d = 0;

    for(int i = 0; i < vlist->size(); i++)
        d += vgraph->getEdge(v->getId(), vlist->get(i)->getId()) * pgraph->getEdge(p->getId(), vlist->get(i)->getPartitionId());


    return trafficVector[v->getId()] - d;
}

void KLMultCluster::updateTrafficVector(){
    double d;
    List<Vertex>* adj = vlist;

    if(movelist->size() == 0){
        for(int i = 0; i < vlist->size(); i++){
            d = 0;
            for(int j = 0; j < adj->size(); j++){
                d += vgraph->getEdge(vlist->get(i)->getId(), adj->get(j)->getId()) * pgraph->getEdge(vlist->get(i)->getPartitionId(), adj->get(j)->getPartitionId());
            }

            trafficVector[vlist->get(i)->getId()] = d;
        }
    }else{
        for(int i = 0; i < vlist->size(); i++){
            if(vgraph->getEdge(vlist->get(i)->getId(), movelist->get(movelist->size() - 1)->a->getId()) == 0)
                if(vgraph->getEdge(vlist->get(i)->getId(), movelist->get(movelist->size() - 1)->b->getId()) == 0)
                    continue;

            d = 0;
            for(int j = 0; j < adj->size(); j++){
                d += vgraph->getEdge(vlist->get(i)->getId(), adj->get(j)->getId()) * pgraph->getEdge(vlist->get(i)->getPartitionId(), adj->get(j)->getPartitionId());
            }

            trafficVector[vlist->get(i)->getId()] = d;
        }
    }
}

void KLMultCluster::sortMoveGainList(List<MoveGain> *src, int low, int high){
    if (low < high) {
        double pivot = src->get(high)->gain;
        int i = (low - 1);

        for (int j = low; j <= high - 1; j++){
            if(src->get(j)->gain > pivot){
                i++;
                src->swap(i, j);
            }
        }
        src->swap(i + 1, high);
        int pi = i + 1;
        sortMoveGainList(src, low, pi - 1);
        sortMoveGainList(src, pi + 1, high);
    }
}

void KLMultCluster::processMoveGainList(){
    MoveGain *mg;
    Vertex *va, *vb;

    if(movegainlist->size() == 0){
        for(int i = 0 ; i < vlist->size(); i++){
            for(int j = 0; j < plist->size(); j++){
                if(vlist->get(i)->getPartition() == plist->get(j))
                    continue;

                if(plist->get(j)->size() == 0)
                    break;

                mg = new MoveGain;
                mg->v = vlist->get(i);
                mg->p = plist->get(j);
                mg->gain = moveGain(vlist->get(i),plist->get(j));
                movegainlist->add(mg);
                mapList->get(mg->v->getPartitionId())->add(mg);
            }
        }
    }else{
        va = movelist->get(movelist->size() - 1)->a;
        vb = movelist->get(movelist->size() - 1)->b;

        for(int i = 0; i < movegainlist->size(); i++){
            mg = movegainlist->get(i);

            if(mg->v == va || mg->v == vb){
                movegainlist->remove(i);
                mapList->get(va->getPartitionId())->remove(mg);
                mapList->get(vb->getPartitionId())->remove(mg);
                delete mg;
                i--;
            }else{
                if(vgraph->getEdge(mg->v, vb) != 0 || vgraph->getEdge(mg->v, va) != 0)
                    mg->gain = moveGain(mg->v, mg->p);

               //movegainlist->get(i)->gain = moveGain(movegainlist->get(i)->v, movegainlist->get(i)->p);
            }
        }
    }

    for(int i = 0; i < mapList->size(); i++){
        sortMoveGainList(mapList->get(i), 0, mapList->get(i)->size() - 1);
    }

    sortMoveGainList(movegainlist, 0, movegainlist->size() - 1);
}

bool KLMultCluster::fit(Vertex* a, Vertex* b){
    /*if(dim == false)
        return true;*/

    Partition *p = b->getPartition();

    if(p->getD1() + b->getD1() < a->getD1()
        || p->getD2() + b->getD2() < a->getD2() ||
           p->getD3() + b->getD3() < a->getD3())
                return false;

    p = a->getPartition();

    if(p->getD1() + a->getD1() < b->getD1()
       || p->getD2() + a->getD2() < b->getD2() ||
          p->getD3() + a->getD3() < b->getD3())
                return false;

    return true;
}
