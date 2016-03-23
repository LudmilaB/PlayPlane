/* Copyright Steve Rabin, 2007. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2007"
 */

#pragma once
//L
#define USE_TINY	0
#define NumberOfMobiles 4

class GameObject;
class CollSphere;

#if USE_TINY
const float fCollRadiusPlayer = 0.15f;  // the collision radius used to represent the player 
const float fCollYOffsetPlayer = 0.20f; // push the coll sphere this high off the ground 
#else
const float fCollRadiusPlayer = 0.24f;  // the collision radius used to represent the player 
const float fCollYOffsetPlayer = 0.23f; // push the coll sphere this high off the ground 
#endif

class Body
{
public:
	Body( int health, D3DXVECTOR3& pos, GameObject& owner );
	~Body( void );

	inline float GetHealth( void )					{ return( m_health ); }
	inline void SetHealth( float health )				{ if( health > 0 ) { m_health = health; } else { m_health = 0; } }
	inline bool IsAlive( void )						{ return( m_health > 0 ); }

	inline void SetSpeed( float speed )				{ m_speed = speed; }
	inline float GetSpeed( void )					{ return( m_speed ); }

	void SetPos( D3DXVECTOR3& pos );
	inline D3DXVECTOR3& GetPos( void )				{ return( m_pos ); }

	inline void SetDir( D3DXVECTOR3& dir )			{ m_dir = dir; }
	inline D3DXVECTOR3& GetDir( void )				{ return( m_dir ); }

	void SetRadius( float radius );
	float GetRadius();

    inline CollSphere* GetPCollSphere()             { return m_pCollSphere; }

	const D3DXMATRIX* GetMatrix(D3DXMATRIX* pmxWorld) const;

protected:

	GameObject* m_owner;

	float m_health;

	D3DXVECTOR3 m_pos;		//Current position
	D3DXVECTOR3 m_dir;		//Current facing direction
	float m_speed;			//Current movement speed

    CollSphere* m_pCollSphere;  // collision sphere associated with the body 
};