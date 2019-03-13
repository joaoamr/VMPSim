#ifndef KLCPP_H
#define KLCPP_H

#include <set>
#include <vector>
//#include "StaticGraph.h"

typedef unsigned NodeIndex_t;

typedef unsigned PartitionIndex_t;

typedef void graph_t;


/* Partitioning is an array of Partition indices. 
   The i'th array position holds the partition index for the i'th node. */
typedef std::vector<PartitionIndex_t> Partitioning_t;

struct Move_t {

  /* Default constructor. */
  Move_t() : move_a(false), move_b(false) {}

  NodeIndex_t a;
  PartitionIndex_t a_to;
  bool move_a;
  NodeIndex_t b;
  PartitionIndex_t b_to;
  bool move_b;

  void move_node(NodeIndex_t n, PartitionIndex_t p) {
    move_a = true;
    move_b = false;
    a = n;
    a_to = p;
  }

  void exchange_node(NodeIndex_t n1, PartitionIndex_t p1, NodeIndex_t n2, PartitionIndex_t p2) {
    move_a = true;
    move_b = true;
    a = n1;
    a_to = p2;
    a = n2;
    a_to = p1;
  }

};

class KLC 
{
 public:

  /* input: p - initial partitioning 
     Output: p - best partitioning found 
     Returns: true if initial partitioning improved
  */
  bool run(graph_t* g, unsigned n_nodes,
	   const Partitioning_t& initial_partitioning, Partitioning_t& best_partitioning, unsigned n_partitions,
	   bool consider_moving_nodes, bool consider_exchanging_nodes);

  /* Returns the cost of partition the graph with partitioning p. */
  virtual double ComputeCost(Partitioning_t& p) = 0;

  /* Returns true if the partitioning is valid, false otherwise. */
  virtual bool ValidPartitioning(Partitioning_t& p) = 0;


 private:

  bool FindBestSingleNodeMove(NodeIndex_t i, Move_t& m, 
			      double& bestC, Partitioning_t& currentPartitioning, 
			      unsigned n_partitions);

/* Check what is the best exchange for the 
   node i considering the current Partitioning */
  bool FindBestNodeExchange(NodeIndex_t i, Move_t& m, 
			    double& bestC, Partitioning_t& currentPartitioning, 
			    unsigned n_partitions);

  bool FindBestMove(Move_t& m, double& cost, 
		    Partitioning_t& currentPartitioning, 
		    unsigned n_partitions,
		    bool consider_moving_nodes, bool consider_exchanging_nodes);

  void Lock_Nodes(Move_t& m)
  {
    if (m.move_a)
      released_nodes.erase(m.a);
    if (m.move_b)	
      released_nodes.erase(m.b);
  }

  void Perform_Move(Partitioning_t& p, Move_t& m)
  {
    if (m.move_a) p[m.a] = m.a_to;
    if (m.move_b) p[m.b] = m.b_to;
  }

  void Release_All_Nodes() { 
    /* Add all node indices to released_nodes. */ 
    for (NodeIndex_t i = 0; i<N_nodes; i++) 
      released_nodes.insert(i);
  }

private:

  graph_t* graph;
  std::set<NodeIndex_t> released_nodes;

  /* This should be replaced by dfg->nodes() */
  unsigned N_nodes;

};

#endif // KLCPP_H
