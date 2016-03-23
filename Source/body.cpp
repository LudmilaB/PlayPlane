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
#include "body.h"
#include "collision.h"

Body::Body( int health, D3DXVECTOR3& pos, GameObject& owner )
: m_health( (float)health ),
  m_owner( &owner ),
  m_speed( 0.0f )
{
    CollSphere cs;
    m_pCollSphere = AppendMobyCollSphere(cs);
    SetPos(pos);
    SetRadius(fCollRadiusPlayer);

	m_dir.x = 1.0f;
	m_dir.y = 0.0f;
	m_dir.z = 0.0f;
}

Body::~Body( void )
{

}

void Body::SetPos( D3DXVECTOR3& pos )
{
    m_pos = pos;
    if (m_pCollSphere)
    {
        m_pCollSphere->center = m_pos;
        m_pCollSphere->center.y += fCollYOffsetPlayer;  // offset sphere off of the ground 
    }
}


void Body::SetRadius( float radius )
{
    if (m_pCollSphere)
    {
        m_pCollSphere->radius = radius;
    }
}


float Body::GetRadius()
{
    if (m_pCollSphere)
    {
        return m_pCollSphere->radius;
    }
    return 0.f;
}


const D3DXMATRIX* Body::GetMatrix(D3DXMATRIX* pmxWorld) const
{
	D3DXMATRIX mx;

	// compute world matrix based on pos/face
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 right;
	D3DXVec3Cross( &right, &m_dir, &up );
	D3DXMatrixIdentity( pmxWorld );
	pmxWorld->_11 = m_dir.x; pmxWorld->_12 = m_dir.y; pmxWorld->_13 = m_dir.z;
	pmxWorld->_21 = up.x; pmxWorld->_22 = up.y; pmxWorld->_23 = up.z;
	pmxWorld->_31 = right.x; pmxWorld->_32 = right.y; pmxWorld->_33 = right.z;
	
	D3DXMatrixTranslation( &mx, m_pos.x, m_pos.y, m_pos.z );
	D3DXMatrixMultiply( pmxWorld, pmxWorld, &mx );
	return pmxWorld;
}
