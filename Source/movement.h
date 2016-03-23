/* Copyright Steve Rabin, 2007. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2007"
 */
#include "PathNode.h"
#include "collision.h"
#include "debugdrawing.h"

#pragma once

class GameObject;

typedef std::list<D3DXVECTOR3> WaypointList;

enum MovementMode
{
	MOVEMENT_NULL,
	MOVEMENT_SEEK_TARGET,
	MOVEMENT_WAYPOINT_LIST
};
enum GameResult
{
	GameContinues,
	Win,
	Lost,
};


class Movement
{
public:
	Movement( GameObject& owner );
	~Movement( void );

	inline void SetTarget( D3DXVECTOR3& target )			{ m_target = target; m_bTotarget = true; }
	inline D3DXVECTOR3& GetTarget( void )					{ return( m_target ); }
	inline void SetDirection( D3DXVECTOR3& dir )			{ m_dir = dir; m_bTotarget = false; }

	void Animate( double dTimeDelta );

    float GetLandSpeed()    { return m_speedLand; }
    float GetAirSpeed()     { return m_speedAir; }

	void SetIdleSpeed( void );
	void SetLandSpeed( void );
	void SetAirSpeed( void );
	void SetAirSpeed( float Speed);
	void ComputePath();
	void DrawDebugVisualization( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj );

protected:

	GameObject* m_owner;

	D3DXVECTOR3 m_target;
	bool m_bTotarget;
	D3DXVECTOR3 m_dir;
	float m_speedLand;
	float m_speedAir;
	MovementMode m_movementMode;
	WaypointList m_waypointList;

	PriorityQueue OpenList;
	std::vector<PathNode*> TotalList;

	bool GetRowColumn( D3DXVECTOR3* pos, int* r, int* c );
	D3DXVECTOR3 GetCoordinates( int r, int c );
	void SmoothCurrentWaypoints();
	D3DXVECTOR3 CatmullRom( D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, float u );
	float GetHeuristic( int curR, int curC, int goalR, int goalC );
	void CleanLists();
	bool bComputingPath;
	void RubberbandCurrentWaypoints();
	void CollisionVsMob(CollSphere* pcs, D3DXVECTOR3 dir);
	DebugDrawing g_debugdrawing; 
	void CheckWinAndLost();
};

