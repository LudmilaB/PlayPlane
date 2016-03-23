#pragma once
#include <vector>


class PathNode
{
public:
	PathNode(){};
	PathNode(int row, int col): Row(row), Col(col){};
	int Row;	
	int Col;
	PathNode* Parent;
	//int parentRow;	//Parent node (-1 is start node)
	//int parentCol;	//Parent node (-1 is start node)

	float cost;	//Cost to get to this node, g(x)

	float total;	//Total cost, g(x) + h(x)

	bool onOpen;	//On Open List
	bool onClosed;	//On Closed List
};

class PriorityQueue
{
public:
	std::vector<PathNode*> heap;
	~PriorityQueue();
	void Clean();
};

bool IsPriorityQueueEmpty( PriorityQueue& pqueue );


class NodeTotalGreater 
{
public:
   //This is required for STL to sort the priority queue
   //(it’s entered as an argument in the STL heap functions)
   bool operator()( PathNode * first, PathNode * second ) const {
      return( first->total > second->total );
   }
};

PathNode* PopPriorityQueue( PriorityQueue& pqueue );

void PushPriorityQueue( PriorityQueue& pqueue, PathNode* node );

void UpdateNodeOnPriorityQueue( PriorityQueue& pqueue, PathNode* node );

std::vector<PathNode*>::iterator FindNode( std::vector<PathNode*>& list, int Row, int Col );



