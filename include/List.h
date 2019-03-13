#ifndef LIST_H
#define LIST_H

/***************************************************************************
* Para usar a lista incluir o header listlib.h
*
* Para declarar uma lista "l" do tipo "Classe"
* (pode ser Vertex, Partition, etc) utilizar as diretivas:
*
* List<Classe> l (n) : Lista est�tica com no m�ximo n itens
* List<Classe> l : Lista din�mica
*
* Para converter uma lista din�mica em est�tica utilizar a seguinte diretiva:
*
* List<Classe>* estatica = listadinamica.copyStatic()
* delete &listadinamica;
* listadinamica = *estatica;
*
* Para converter uma lista est�tica em din�mica utilizar a seguinte diretiva:
*
* List<Classe>* dinamica = listaestatica.copyStatic()
* delete &listaestatica;
* listaestatica = *dinamica;
****************************************************************************/

template <class e> struct Link{
    e* element;
    Link* prev;
    Link* next;
};

template <class e> class List
{
    public:
        List(int n){
            itindex = 0;
            listsize = 0;
            array = new e*[n];
            dyn = false;
            maxsize = n;
            iterator = first;
        };

        List(){
            first = 0;
            listsize = 0;
            dyn = true;
        };

        virtual ~List(){
            Link<e>* p;
            if(!dyn)
                delete[] array;
            else{
                if(listsize > 0)
                    for(p = first; p != 0; p = p->next)
                        delete p;
            }
        };

        bool add(e* elem);  // Adiciona elemento "elem" na lista
        bool add(e* elem, int p);
        bool remove(e* elem); // Remove elemento "elem" da lista
        int contains(e* elem);  // Retorna a posi��o do item "elem" se esse estiver na lista ou -1 caso contr�rio
        e* remove(int i); // Remove o i-�simo elemento e o retorna caso haja sucesso na remo��o, caso contr�rio retorna NULL
        e* fastRemove(int i); // Troca o elemento i pelo �ltimo elemento da lista e decrementa seu tamnho
        e* get(int i); // Retorna o i-�simo elemento
        bool swap(int i, int j); // Troca o elementos i e j de posi��o
        int size() {return listsize;}; // Retorna tamanho da lista
        List<e> *copyStatic(int n); // Retorna um ponteiro para uma lista est�tica (de tamanho n) com os mesmo elementos
        List<e> *copyStatic(){return copyStatic(maxsize);}; // Retorna um ponteiro para uma lista est�tica (do tamanho de elementos presentes na atual) com os mesmo elementos
        List<e> *copyDynamic(); // Retorna um ponteiro para uma lista din�mica com os mesmos elementos
        void clear();

    private:
        bool dyn;
        int listsize;
        int maxsize;
        e** array;
        bool staticAdd(e* elem);
        bool dynamicAdd(e* elem);
        e* staticGet(int i);
        e* dynamicGet(int i);
        bool staticSwap(int i, int j);
        bool dynamicSwap(int i, int j);
        Link<e>* first;
        Link<e>* last;
        Link<e>* iterator;
        int itindex;
        bool staticRemove(e* elem);
        bool dynamicRemove(e* elem);
};

#endif // LIST_H
