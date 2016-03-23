#include "DXUT.h"
#include <algorithm>
#include "PathNode.h"

//PathNode::PathNode(int row, int col): Row(row), Col(col)
//{
//}

bool IsPriorityQueueEmpty( PriorityQueue& pqueue )
{
	return( pqueue.heap.empty() );
}

PathNode* PopPriorityQueue( PriorityQueue& pqueue )
{ //Total time = O(log n) 
  //Get the node at the front - it has the lowest total cost
  PathNode * node = pqueue.heap.front();

  //pop_heap will move the node at the front to position N and
  //then sort the heap to make positions 1 through N-1 correct
  //(STL makes no assumptions about your data and doesn't want
  //to change the size of the container.)
  std::pop_heap( pqueue.heap.begin(), pqueue.heap.end(), 
                 NodeTotalGreater() );

  //pop_back() will remove the last element from the heap
  //(now the heap is sorted for positions 1 through N)
  pqueue.heap.pop_back();

  return( node );
}

void PushPriorityQueue( PriorityQueue& pqueue,
                        PathNode* node )
{ //Total time = O(log n)
  
  //Pushes the node onto the back of the vector
  //(the heap is now unsorted)
  pqueue.heap.push_back( node );

  //Sorts the new element into the heap
  std::push_heap( pqueue.heap.begin(),
                  pqueue.heap.end(),
                  NodeTotalGreater() );
}

void UpdateNodeOnPriorityQueue( PriorityQueue& pqueue,
                                PathNode* node )
{ //Total time = O(n+log n)
   
  //Loop through the heap and find the node to be updated
  std::vector<PathNode*>::iterator i;
  for( i=pqueue.heap.begin(); i!=pqueue.heap.end(); i++ )
  {
    if( (*i)->Row == node->Row && (*i)->Col == node->Col )
    { //Found node - resort from this position in the heap
      //(since its total value was changed before this
      //function was called)

      std::push_heap( pqueue.heap.begin(), i+1, 
                      NodeTotalGreater() );
      return;
    }
  }
}

PriorityQueue::~PriorityQueue()
{
	Clean();
}

std::vector<PathNode*>::iterator FindNode( std::vector<PathNode*>& list, int Row, int Col )
{
	std::vector<PathNode*>::iterator iter;
	for (iter = list.begin(); iter != list.end(); iter++)
		if ( (*iter)->Row == Row && (*iter)->Col == Col )
			return iter;
	return iter;
}

void PriorityQueue::Clean()
{
	if(!heap.empty())
	{
	/*	std::vector<PathNode*>::pointer ptr = &heap[0];
	
		std::vector<PathNode*>::iterator it;
		for( it = heap.begin(); it != heap.end(); it++ )
			*it._Myptr = 0;*/

		int Size = heap.size();
		for( int i = 0; i < Size; i++ )
		{				
	/*			ptr = &heap[Size - i -1];
				ptr = NULL;*/
				heap.pop_back();
 		}


	}
}
