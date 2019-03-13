#include "List.h"


template <class e>
bool List<e>::add(e* elem)
{
    if(!dyn)
        return staticAdd(elem);
    else
        return dynamicAdd(elem);
}

template <class e>
bool List<e>::add(e* elem, int p)
{
    if(listsize == maxsize - 1)
        return false;

    for(int i = listsize; i > p; i--)
        array[i] = array[i-1];

    listsize++;
    array[p] = elem;
    return true;
}


//Insert element for static list
template <class e>
bool List<e>::staticAdd(e* elem)
{
    if(listsize == maxsize)
        return false;

    array[listsize] = elem;
    listsize++;
    return true;
}

//Insert element for dynamic list
template <class e>
bool List<e>::dynamicAdd(e* elem)
{
    if(listsize == 0){
        first = new Link<e>;
        first->element = elem;
        last = first;
    }else{
        last->next = new Link<e>;
        last->next->element = elem;
        last->next->prev = last;
        last = last->next;
    }
    last->next = 0;
    listsize++;
    return true;
}

template <class e>
e* List<e>::get(int i)
{
    if(i > listsize - 1 || i < 0 || listsize < 0)
        return 0;

    if(!dyn)
        return staticGet(i);

    return dynamicGet(i);
}

template <class e>
e* List<e>::staticGet(int i)
{
    return array[i];
}

template <class e>
e* List<e>::dynamicGet(int i)
{
    int k;
    e* element;
    if(i == itindex + 1){
        itindex++;
        iterator = iterator->next;
        return iterator->element;
    }else
        itindex = i;

    Link<e>* p = first;
    for(k = 0; k < i; k++){
        p = p->next;
    }
    iterator = p;

    element = p->element;
    return element;
}

template <class e>
bool List<e>::swap(int i, int j)
{
    if(i < 0 || j > listsize - 1 || i > listsize -1)
        return false;

    if(i == j)
        return true;

    if(!dyn)
        return staticSwap(i, j);

    return dynamicSwap(i, j);
}

template <class e>
bool List<e>::staticSwap(int i, int j)
{
    e* p = array[i];
    array[i] = array[j];
    array[j] = p;
    return true;
}

template <class e>
bool List<e>::dynamicSwap(int i, int j)
{
    int min, max;
    int k;
    Link<e>* pmin, *pmax, *p = first;
    if(i > j){
        min = j;
        max = i;
    }else{
        min = i;
        max = j;
    }

    for(int k = 0; k < max; k++, p = p->next){
        if(k == min)
            pmin = p;
    }
    pmax = p;

    e* elem;
    elem = pmax->element;
    pmax->element = pmin->element;
    pmin->element = elem;

    return true;
}

template <class e>
int List<e>::contains(e* elem){
    int i;
    Link<e>* p;
    if(!dyn){
        for(int i = 0; i < listsize; i++)
           if(array[i] == elem)
                return i;
    }else{
        i = 0;
        for(p = first; p != 0; p = p->next){
            if(p->element == elem)
                return i;
            i++;
        }
    }

    return -1;
}

template <class e>
e* List<e>::fastRemove(int i){
    if(dyn)
        return 0;

     if(i > listsize - 1)
            return 0;

    swap(i , listsize - 1);
    listsize--;
    return array[listsize + 1];
}

template <class e>
e* List<e>::remove(int i){
    e* elem;
    Link<e>* p;
    int k;
    if(i > listsize - 1)
            return 0;

    if(!dyn){
        elem = array[i];
        for(int k = i; k < listsize; k++)
            array[k] = array[k+1];

        listsize--;
        return elem;
    }else{
        iterator = first;
        itindex = 0;
        if(i == 0){
            if(i == listsize - 1){
                p = last;
                last = last->prev;
            }else{
                first = first->next;
                p = first;
            }
        }else
            if(i == listsize - 1){
                p = last;
                last = last->prev;
            }else{
                for(k = 0; k <= i; k++)
                    p = p->next;

                p->prev->next = p->next;
                p->next->prev = p->prev;

            }
        elem = p->element;
        delete p;
        listsize--;
        return elem;
    }

}

template <class e>
bool List<e>::remove(e* elem){
    int pos;
    Link<e>* p;

    if(!dyn){
        pos = contains(elem);
        if(pos == -1)
            return false;

        for(int i = pos; i < listsize; i++)
            array[i] = array[i+1];

        listsize--;
        return true;
    }else{
        iterator = first;
        itindex = 0;

        if(listsize == 1){
            delete first;
            listsize--;
            return true;
        }

        for(p = first; p != 0; p = p->next){
            if(p->element == elem){
                if(p == first)
                    first = p->next;
                else
                    if(p == last)
                        last = last->prev;
                    else{
                        p->prev->next = p->next;
                        p->next->prev = p->prev;

                    }
                delete p;
                listsize--;
                return true;
            }
        }
    }
    return false;
}

template <class e>
List<e>* List<e>::copyStatic(int n)
{
    List<e> *l = new List<e>(n);
    for(int i = 0; i < listsize; i++)
        l->add(get(i));

    return l;
}

template <class e>
List<e>* List<e>::copyDynamic()
{
    List<e> *l = new List<e>;
    for(int i = 0; i < listsize; i++)
        l->add(get(i));

    return l;
}

template <class e>
void List<e>::clear(){
    while(listsize > 0)
        remove(0);

}
