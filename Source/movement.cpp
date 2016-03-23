/* Copyright Steve Rabin, 2007. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2007"
 */

#include "DXUT.h"
#include "database.h"
#include "movement.h"
#include "gameobject.h"
#include "body.h"
#include "tiny.h"
#include "WorldCollData.h"
#include "collision.h"
#include "game.h"
#include "global.h"
#include "Stars.h"
#include "Utilities.h"

float round( float x);

Movement::Movement( GameObject& owner )
: m_owner( &owner ),
  m_speedLand( 1.f ),
  m_speedAir( 2.5f ),
  m_movementMode( MOVEMENT_SEEK_TARGET )
{
	m_target.x = m_target.y = m_target.z = 0.0f;
}

Movement::~Movement( void )
{

}

void Movement::Animate( double dTimeDelta )
{

	GameObject* go = g_database.Find( g_objectIDPlayer );
	if( !m_owner->GetBody().IsAlive() || !go->GetBody().IsAlive() )
		return;
	float speed = m_owner->GetBody().GetSpeed();
	if( speed == 0.0f )
	{
		m_movementMode = MOVEMENT_SEEK_TARGET;
	}
	else
	{
		D3DXVECTOR3 pos = m_owner->GetBody().GetPos();
		CollSphere* pcs = m_owner->GetBody().GetPCollSphere();
		if( m_movementMode == MOVEMENT_SEEK_TARGET )
		{
//L			float speed = m_owner->GetBody().GetSpeed();
			D3DXVECTOR3 toTarget = m_target - pos;

			if( D3DXVec3Length( &toTarget)< 0.1f  && m_bTotarget )
			{	//Notify target reached
				//m_owner->GetBody().SetPos( m_target );
				g_database.SendMsgFromSystem( m_owner, MSG_Arrived );
			}
			else
			{	
				//Point character towards target this frame
				D3DXVECTOR3 dir;
				if(m_bTotarget)
				{
					D3DXVec3Normalize( &dir, &toTarget );
					dir *= speed < 0.0f ? -1.0f: 1.0f;	// needed for backward movement 
				}
				else
					dir = m_dir;

				m_owner->GetBody().SetDir( dir );

				//Move character towards target this frame
				double speedScale = 1.0f;
			
				float velocity = m_owner->GetBody().GetSpeed();
				D3DXVec3Scale( &dir, &dir, float(  velocity * speedScale * dTimeDelta ) );

//L				CollSphere* pcs = m_owner->GetBody().GetPCollSphere();
				if (pcs)
				{
					// Do a tentative move to the new position and store it in pcs->center. 
					D3DXVec3Add( &pcs->center, &dir, &pcs->center );

					// Do a collision test of sphere vs the world. 
	//				CollisionVsMob(pcs, dir);
					int r = (int)floor(pcs->center.z);
					int c = (int)floor(pcs->center.x);
					WorldFile::ECell cell = (*g_pWorldCollData->pwf)(r, c);
					if (g_pWorldCollData->CollideSphereVsWorld(*pcs) || cell == WorldFile::OCCUPIED_CELL  )
					{
						if ( cell == WorldFile::OCCUPIED_CELL )
							OutputDebugString(L"Collision\n\r");
						// This is the collision response. That's it. 
	 //L                   pcs->center += gCollOutput.push;
						   dir = -dir;
						   D3DXVec3Add( &pcs->center, &dir, &pcs->center );
	//					   pcs->center -= dir; 
						   m_movementMode = MOVEMENT_SEEK_TARGET;
						   m_waypointList.clear();
						// Notify that we've collided with the environment. 
						g_database.SendMsgFromSystem( m_owner, MSG_CollideEnv );
					}
					CollisionVsMob(pcs, dir);
					pcs->center.y -= fCollYOffsetPlayer;  // undo offsetting of sphere off the ground 
					m_owner->GetBody().SetPos( pcs->center );

					// Check CollSphere against other mobys.
					
				}
			}
		}
	
	else if( m_movementMode == MOVEMENT_WAYPOINT_LIST )
		{

			D3DXVECTOR3 target = m_waypointList.front();
			D3DXVECTOR3 toTarget = target - pos;

			D3DXVECTOR3 goal = m_waypointList.back();
			D3DXVECTOR3 toGoal = goal - pos;

			if( D3DXVec3Length( &toGoal ) < 0.1f )
			{	//Notify goal reached
				m_movementMode = MOVEMENT_SEEK_TARGET;
				m_waypointList.clear();
				g_database.SendMsgFromSystem( m_owner, MSG_Arrived );
			}
			else
			{	
				if( D3DXVec3Length( &toTarget ) < 0.1f )
				{	//Target reached - get new target
					m_waypointList.pop_front();
					target = m_waypointList.front();
					toTarget = target - pos;
				}


				//Point character towards target this frame
				D3DXVECTOR3 dir;
				D3DXVec3Normalize( &dir, &toTarget );
				m_owner->GetBody().SetDir( dir );

				////Move character towards target this frame
				//double speedScale = m_owner->GetTiny().GetSpeedScale();
				//D3DXVec3Scale( &dir, &dir, float( m_owner->GetBody().GetSpeed() * speedScale * dTimeDelta ) );

				//D3DXVECTOR3 newPos;
				//D3DXVec3Add( &newPos, &dir, &m_owner->GetBody().GetPos() );
				//m_owner->GetBody().SetPos( newPos );
				//Move character towards target this frame
				double speedScale = 1.0f;
			
				D3DXVec3Scale( &dir, &dir, float( m_owner->GetBody().GetSpeed() * speedScale * dTimeDelta ) );
				D3DXVECTOR3 newPos;
				D3DXVec3Add( &newPos, &dir, &m_owner->GetBody().GetPos() );
				m_owner->GetBody().SetPos( newPos );
				CollisionVsMob(pcs,dir);
			}
		}
	}
	m_owner->GetInstance()->SetOrientation();
}

void Movement::SetIdleSpeed( void )
{
//L	m_owner->GetBody().SetSpeed( 0.5f );
	m_owner->GetBody().SetSpeed( 0.0f );
}

void Movement::SetLandSpeed( void )
{
	m_owner->GetBody().SetSpeed( m_speedLand );
}

void Movement::SetAirSpeed( void )
{
	m_owner->GetBody().SetSpeed( m_speedAir );
}

void Movement::SetAirSpeed( float Speed)
{
	m_speedAir = Speed;
}

void Movement::ComputePath()
{
//L	m_goal = g_terrain.GetCoordinates( r, c );
  GameObject* player = g_database.Find( g_objectIDPlayer );
  if (!player)
	return;
  D3DXVECTOR3 target = player->GetBody().GetPos();
  int r, c;
  GetRowColumn( &target, &r, &c );  

 // m_movementMode = MOVEMENT_WAYPOINT_LIST;
  float DiagCost = sqrt(2.0f);

  int curR, curC;
  D3DXVECTOR3 cur = m_owner->GetBody().GetPos();
  GetRowColumn( &cur, &curR, &curC );
  m_waypointList.clear();
 
  PathNode* Start = new PathNode( curR, curC);
  Start->onOpen = true;      
  Start->onClosed = false;    
  Start->Parent = 0;         
  Start->cost = 0;   
  int m_heuristicWeight = 1;
  Start->total = GetHeuristic( curR, curC, r, c ) * m_heuristicWeight;  
  PushPriorityQueue(OpenList,Start );
  TotalList.push_back( Start );
	 
  while( !IsPriorityQueueEmpty( OpenList ) )
  {
	PathNode* BestNode = PopPriorityQueue( OpenList );
	if( BestNode->Row == r && BestNode->Col == c)		//Goal achieved
	{
		do
		{
			D3DXVECTOR3 Coord = GetCoordinates( BestNode->Row, BestNode->Col );
			m_waypointList.push_front( Coord );
			BestNode = BestNode->Parent;
		}		
		while (BestNode);

		CleanLists();
		RubberbandCurrentWaypoints();
		SmoothCurrentWaypoints();
		m_movementMode = MOVEMENT_WAYPOINT_LIST;

		return;
	}

	for( int RowOff = -1; RowOff <= 1; RowOff++)
		for( int ColOff = -1; ColOff <= 1; ColOff++)
		{
			int Row  = BestNode->Row + RowOff;
			int Col = BestNode->Col + ColOff;
			WorldFile::ECell cell = (*g_pWorldCollData->pwf)(Row, Col);
			if(!( ColOff == 0 && RowOff == 0 )
			  &&  !(cell == WorldFile::OCCUPIED_CELL )
			  && Row >= 0 && Col >= 0 && Row < g_pWorldCollData->pwf->GetHeight() && Col < g_pWorldCollData->pwf->GetWidth() )
			{
				bool bDiagonal = false;
				if ( ColOff != 0 && RowOff != 0)
					bDiagonal = true;

				WorldFile::ECell cell1 = (*g_pWorldCollData->pwf)(BestNode->Row, Col);
				WorldFile::ECell cell2 = (*g_pWorldCollData->pwf)(Row, BestNode->Col);

				if( bDiagonal && (cell1 == WorldFile::OCCUPIED_CELL || cell2 == WorldFile::OCCUPIED_CELL)) //check for cutting corner
					continue;

				float GivenCost;
				if( bDiagonal )
					GivenCost = BestNode->cost + DiagCost;
				else
					GivenCost = BestNode->cost + 1.f;
				float TotalCost = GivenCost + GetHeuristic( Row, Col, r, c ) * m_heuristicWeight;

				std::vector<PathNode*>::iterator ActNode = FindNode( TotalList, Row, Col);
				if (( ActNode != TotalList.end() && !( TotalCost > (*ActNode)->total )) || ActNode == TotalList.end() )
				{
			
					  PathNode* NewNode  = new PathNode( Row, Col);
					  NewNode->onOpen = true;      
					  NewNode->onClosed = false;    
					  NewNode->Parent = BestNode;         
					  NewNode->cost = GivenCost;          
					  NewNode->total = TotalCost;

					  if( ActNode == TotalList.end())
					  {
						  PushPriorityQueue(OpenList, NewNode );
						  TotalList.push_back( NewNode );
					  }
					  else
					  {
						  (*ActNode)->onOpen = true; 
						  (*ActNode)->onClosed = false;    
						  (*ActNode)->Parent = BestNode;         
					      (*ActNode)->cost = GivenCost;          
					      (*ActNode)->total = TotalCost;
						   UpdateNodeOnPriorityQueue( OpenList, NewNode );
//						   UpdateNodeOnPriorityQueue( OpenList, (Node*)ActNode._Myptr );
					  }
				}
			}
		}
		BestNode->onClosed = true;
  }

  if (IsPriorityQueueEmpty( OpenList ))
  {
	bComputingPath = false;
	CleanLists();
	g_database.SendMsgFromSystem( m_owner, MSG_Arrived );
   }
}

bool Movement::GetRowColumn( D3DXVECTOR3* pos, int* r, int* c )
{
	if( pos->x >= 0.0f && pos->x <= (float)g_pWorldCollData->pwf->GetWidth() && pos->z >= 0.0f && pos->z <= (float)g_pWorldCollData->pwf->GetHeight() )
	{
		*r = (int)floor(pos->z);
		*c = (int)floor(pos->x);
		return true;
	}
	else
	{
		return false;
	}
}

D3DXVECTOR3 Movement::GetCoordinates( int r, int c )
{
	D3DXVECTOR3 pos;
	pos.x = ((float)c) + 0.5f;
	pos.y = 0.0f;
	pos.z = ((float)r) + 0.5f;
	
	return( pos );
}

void Movement::SmoothCurrentWaypoints()
{
	if (m_waypointList.size() < 3) return;
 
 	WaypointList OrigList = m_waypointList;
 
 	m_waypointList.clear();
 	D3DXVECTOR3 Points[4];
 	WaypointList::iterator iter = OrigList.begin();
 	int step = 0;
 	while (step != OrigList.size()-1)
 	{
 		Points[0] = *iter;
 		if (step != 0) iter++;
 		Points[1] = *iter;	iter++;
 		Points[2] = *iter;
 		if (step != OrigList.size()-2) iter++;
 		Points[3] = *iter;

		//(x2 - x1)·(y - y1) = (y2 - y1)·(x - x1) equation of line throw Points (x1,y1) and (x2,y2)

		if( (round((Points[1].x - Points[0].x ) *  (Points[2].z - Points[0].z)) ) !=
			(round((Points[1].z - Points[0].z ) *  (Points[2].x - Points[0].x)) )
			||
			(round((Points[1].x - Points[0].x ) *  (Points[3].z - Points[0].z)) ) !=
			(round((Points[1].z - Points[0].z ) *  (Points[3].x - Points[0].x)) ) )

		{
 			// calculate points between Points[1] and Points[2] 
 			float tStep = 1.f / 3;
 			for (float t = 0.f; t < 1.f; t += tStep)
 			{
 				D3DXVECTOR3 interpoint = CatmullRom(Points[0], Points[1], Points[2], Points[3], t);
 				m_waypointList.push_back( interpoint );
 			}
		}
		else
		{
			m_waypointList.push_back(Points[1]);
			m_waypointList.push_back(Points[2]);
		}
 
 		--(--iter);
 		step++;
 	}
 
 	//final point
 	m_waypointList.push_back( OrigList.back() );

}

D3DXVECTOR3 Movement::CatmullRom( D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, float u )
{
	float u_squared = u*u;
	float u_cubed = u_squared*u;

	D3DXVECTOR3 result = p0 * (-0.5f*u_cubed + u_squared - 0.5f*u) +
		                 p1 * (1.5f*u_cubed - 2.5f*u_squared + 1.0f) +
						 p2 * (-1.5f*u_cubed + 2.0f*u_squared + 0.5f*u) +
						 p3 * (0.5f*u_cubed - 0.5f*u_squared);

	return( result );
}

float Movement::GetHeuristic( int curR, int curC, int goalR, int goalC )
{
	float Rdiff = (float)abs( curR - goalR);
	float Cdiff = (float)abs( curC - goalC);
//	if (m_heuristicCalc) //Cardinal/Intercardinal
	{
		return min(Rdiff, Cdiff) * sqrt(2.f) + max(Rdiff, Cdiff) - min(Rdiff, Cdiff); 
	}
	//else //Euclidean
	//{
	//	return sqrt(Rdiff*Rdiff + Cdiff*Cdiff); 
	//}

}

void Movement::CleanLists()
{
	OpenList.Clean();
	std::vector<PathNode*>::iterator i;
	for( i = TotalList.begin(); i != TotalList.end(); i++ )
	{
			delete (*i);
//			*i._Myptr = 0;
//			delete (*i);
//			TotalList.erase(i);
 	}

	TotalList.clear();

	
}


void Movement::RubberbandCurrentWaypoints()
{
	if (m_waypointList.size() < 3) return;

	WaypointList::iterator iter, MidIter;
	D3DXVECTOR3 Points[3];
	iter = m_waypointList.begin();
	while (iter != m_waypointList.end())
	{
		// get the three points we're testing
		Points[0] = *iter;
		iter++;
		MidIter = iter;
		Points[1] = *iter;
		iter++;
		if (iter == m_waypointList.end())
			break;
		Points[2] = *iter;

		int r1, r2, c1, c2;
		GetRowColumn( &Points[0], &r1, &c1 );		
		GetRowColumn( &Points[2], &r2, &c2 );

		if(r1 > r2)
			swap(&r1, &r2);
		if(c1 > c2)
			swap(&c1, &c2);

		
		// check  for a wall
		bool Wall = false;

		for (int r = r1; r <= r2; r++)
		{
			for (int c = c1; c <= c2; c++)
			{
				WorldFile::ECell cell = (*g_pWorldCollData->pwf)(r, c);

				if( cell == WorldFile::OCCUPIED_CELL) 
				{
					Wall = true;
					break;
				}
			}
			if (Wall) break;
		}

		if (!Wall)
		{
			// no wall; erase the middle point
			m_waypointList.erase(MidIter);
			if (m_waypointList.size() < 3)
				break;

			// step back three
			((iter--)--)--;
		}
		else
		{
			// wall found; 
			// step back two
			(iter--)--;
		}
	}
}

void Movement::DrawDebugVisualization( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj )
{

	if( !m_waypointList.empty() )
	{
		D3DXVECTOR3 p0, p1;
		D3DXVECTOR3 last = *m_waypointList.begin();
		D3DXVECTOR3 cur = m_owner->GetBody().GetPos();
		p0 = last;
		cur.y = p0.y = 0.01f;
		g_debugdrawing.DrawLine( pd3dDevice, pViewProj, cur, p0, DEBUG_COLOR_RED, true );

		WaypointList::iterator i = m_waypointList.begin();
		for( WaypointList::iterator i = m_waypointList.begin(); i != m_waypointList.end(); i++ )
		{
			p0 = *i;
			p0.y = 0.01f;
			if(p0==last)
				continue;
			p1 = p0;
			p1.y += 0.04f;
			g_debugdrawing.DrawLine( pd3dDevice, pViewProj, p0, p1, DEBUG_COLOR_BLACK, false );
			g_debugdrawing.DrawLine( pd3dDevice, pViewProj, last, p0, DEBUG_COLOR_BLACK, false );
			last = p0;
		}
	}
//	//Draw waypoints
//	if( !m_waypointList.empty() )
//	{
//		D3DXVECTOR3 p0, p1;
//		D3DXVECTOR3 last = *m_waypointList.begin();
//		D3DXVECTOR3 cur = m_owner->GetBody().GetPos();
//		p0 = last;
////		cur.y = p0.y = 0.01f;
//		p0.y = 0.02f;
//		last.y = 0.02f;
////		g_debugdrawing.DrawLine( pd3dDevice, pViewProj, cur, p0, DEBUG_COLOR_RED, true );
//
//		WaypointList::iterator i = m_waypointList.begin();
//		for( WaypointList::iterator i = m_waypointList.begin(); i != m_waypointList.end(); i++ )
//		{
//			p0 = *i;
//			p0.y = 0.02f;
//			if(p0==last)
//				continue;
//		/*	p1 = p0;
//			p1.y += 0.02f;*/
//	//		g_debugdrawing.DrawLine( pd3dDevice, pViewProj, p0, p1, DEBUG_COLOR_BLACK, false );
//			g_debugdrawing.DrawLine( pd3dDevice, pViewProj, last, p0, DEBUG_COLOR_RED, false );
//			last = p0;
//		}
//	}
}

void Movement::CollisionVsMob(CollSphere* pcs, D3DXVECTOR3 dir)
{

	D3DXVECTOR3 Center;
	int Mob;
	if (pcs->VsMobys(&Center, &Mob))
	{
		 dir = -dir;
		 D3DXVec3Add( &pcs->center, &dir, &pcs->center );
		// This is the collision response. 
		pcs->center += gCollOutput.push;
		int MobColl;
		if (g_pWorldCollData->CollideSphereVsWorld(*pcs)|| pcs->VsMobys(&Center, &MobColl))
			pcs->center -= gCollOutput.push;
	//		pcs->center -= dir; 
		m_movementMode = MOVEMENT_SEEK_TARGET;
		m_waypointList.clear();
		GameObject* gMob = g_database.Find( g_objectIDPlayer + Mob);
		Body& body = gMob->GetBody();
		int ID = m_owner->GetID();
		if( ID == g_objectIDPlayer )
			body.SetHealth( body.GetHealth()-.5f);
		else if( Mob== 0 )
			body.SetHealth( body.GetHealth()-1);
		if(Mob== 0)
		{

			g_database.SendMsgFromSystem( m_owner, MSG_CollideMoby );
			g_pStar->Init( (Center + pcs->center)/2 );
		}
		CheckWinAndLost();
	}
}

void Movement::CheckWinAndLost()
{
	GameObject* gMob;
	int count = 0;
	for(int i = 0; i <NumberOfMobiles; i++ )
	{
		gMob = g_database.Find( g_objectIDPlayer + i );
		if (!gMob->GetBody().IsAlive())
		{
			if( !i )
			{
				g_Result = Lost;
				return;
			}
			count++;
		}
	}
	if( count == NumberOfMobiles-1)
		g_Result = Win;
}