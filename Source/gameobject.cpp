/* Copyright Steve Rabin, 2008. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2008"
 */

#include "DXUT.h"
#include "gameobject.h"
#include "MeshInstance.h"
#include "msgroute.h"
#include "statemch.h"
#include "movement.h"
#include "body.h"


GameObject::GameObject( objectID id, unsigned int type, char* name )
: m_markedForDeletion(false),
  m_stateMachineManager(0),
  m_body(0),
  m_movement(0),
  m_instance(0)
{
	m_id = id;
	m_type = type;
	
	if( strlen(name) < GAME_OBJECT_MAX_NAME_SIZE ) {
		strcpy( m_name, name );
	}
	else {
		strcpy( m_name, "invalid_name" );
		ASSERTMSG(0, "GameObject::GameObject - name is too long" );
	}
}

GameObject::~GameObject( void )
{
	if(m_stateMachineManager)
	{
		delete m_stateMachineManager;
	}
	if(m_body)
	{
		delete m_body;
	}
	if(m_movement)
	{
		delete m_movement;
	}
	if(m_instance)
	{
		delete m_instance;
	}
}

void GameObject::CreateStateMachineManager( void )
{
	m_stateMachineManager = new StateMachineManager( *this );
}

void GameObject::CreateMovement( void )
{
	m_movement = new Movement( *this ); 
}

void GameObject::CreateBody( int health, D3DXVECTOR3& pos )
{
	m_body = new Body( health, pos, *this );
}

void GameObject::CreateTiny( CMultiAnim *pMA, std::vector< CTiny* > *pv_pChars, CSoundManager *pSM, double dTimeCurrent )
{
    m_instance = new CTiny( *this );
    if( m_instance == NULL )
	{
		return;
	}

    if( SUCCEEDED( dynamic_cast<CTiny*>(m_instance)->Setup( pMA, pv_pChars, pSM, dTimeCurrent ) ) )
	{
        dynamic_cast<CTiny*>(m_instance)->SetSounds( true );
	}
	else
	{
		delete m_instance;
	}
}

// Instance
HRESULT GameObject::CreateInstance(IDirect3DDevice9* pd3dDevice, const LPCWSTR szType, const LPCWSTR szFilename)
{
	if (m_instance)
	{
		delete m_instance;
	}
	if (wcscmp(szType, L"mesh") == 0)
	{
		CMeshInstance* pmi = new CMeshInstance(*this);
		HRESULT hr = pmi->GetInstance(pd3dDevice, szFilename);
		// BUGBUG: FIXME: it may be wrong to automatically add it to the scene here; 
		// maybe the caller should take care of this. 
		g_pScene->AddChild(pmi->GetNode());
		m_instance = pmi;
		return hr;
	}
	if (!m_instance)
	{
		return MAKE_HRESULT(SEVERITY_ERROR, 0, 0);	// should return some sort of out of memory error. 
	}
	return m_instance->GetInstance(pd3dDevice, szFilename);
}

void GameObject::Initialize( void )
{

}

/*---------------------------------------------------------------------------*
  Name:         Update

  Description:  Calls the update function of the currect state machine.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void GameObject::Update( void )
{
	if(m_stateMachineManager)
	{
		m_stateMachineManager->Update();
	}
}

void GameObject::Animate( double dTimeDelta )
{
	if( m_movement )
	{
		m_movement->Animate( dTimeDelta );
	}
}

void GameObject::AdvanceTime( double dTimeDelta, D3DXVECTOR3 *pvEye )
{
	if( m_instance )
	{
		m_instance->AdvanceTime( dTimeDelta, pvEye );
	}
}

void GameObject::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( m_instance )
	{
		m_instance->RestoreDeviceObjects( pd3dDevice );
	}
}

void GameObject::InvalidateDeviceObjects( void )
{

}

void GameObject::Draw( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj )
{
	if( m_movement )
	{
		m_movement->DrawDebugVisualization( pd3dDevice, pViewProj );
	}
}




