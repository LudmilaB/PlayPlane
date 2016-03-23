/* Copyright Steve Rabin, 2008. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2008"
 */

#pragma warning(disable: 4995)

#include "DXUT.h"
#include "body.h"
#include "game.h"
#include "world.h"
#include "time.h"
#include "database.h"
#include "msgroute.h"
#include "gameobject.h"
#include "debuglog.h"
#include "debugdrawing.h"
#include "MeshInstance.h"
#include "ModelNode.h"
#include "sound.h"
#include "WorldCollData.h"
#include "layerdrawing.h"
#include "Utilities.h"

extern CSoundManager           g_DSound;               // DirectSound class
extern CMultiAnim              g_MultiAnim;            // the MultiAnim class for holding Tiny's mesh and frame hierarchy
extern std::vector <CTiny*>    g_v_pCharacters;        // array of character objects; each can be associated with any of the CMultiAnims

// State machines
#include "player.h"
#include "Plane.h"
#include "PlaneType1.h"
#include "PlaneType2.h"
#include "PlaneType3.h"
#include "PlaneInFormation.h"

// Unit test state machines
#include "unittest1.h"
#include "unittest2a.h"
#include "unittest2b.h"
#include "unittest2c.h"
#include "unittest3a.h"
#include "unittest3b.h"
#include "unittest4.h"
#include "unittest5.h"
#include "unittest6.h"

//#define UNIT_TESTING
#define VISUALIZEWORLDCOLL  0
//L
#define PLAYER 1

World::World(void)
: m_initialized(false)
{

}

World::~World(void)
{
	delete m_time;
	delete m_database;
	delete m_msgroute;
	delete m_debuglog;
	//delete m_debugdrawing;
	//delete m_layerdrawing;
}

void World::InitializeSingletons( void )
{
	//Create Singletons
	m_time = new Time();
	m_database = new Database();
	m_msgroute = new MsgRoute();
	m_debuglog = new DebugLog();
	//m_debugdrawing = new DebugDrawing();
	//m_layerdrawing = new LayerDrawing();
}

void World::Initialize( IDirect3DDevice9* pd3dDevice )
{
	if(!m_initialized)
	{
		m_initialized = true;
	

#ifdef UNIT_TESTING

		//Create unit test game objects
		GameObject* unittest1 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest1" );
		GameObject* unittest2 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest2" );
		GameObject* unittest3a = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest3a" );
		GameObject* unittest3b = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest3b" );
		GameObject* unittest4 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest4" );
		GameObject* unittest5 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest5" );
		GameObject* unittest6 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest6" );
		
		unittest1->CreateStateMachineManager();
		unittest2->CreateStateMachineManager();
		unittest3a->CreateStateMachineManager();
		unittest3b->CreateStateMachineManager();
		unittest4->CreateStateMachineManager();
		unittest5->CreateStateMachineManager();
		unittest6->CreateStateMachineManager();
		
		g_database.Store( *unittest1 );
		g_database.Store( *unittest2 );
		g_database.Store( *unittest3a );
		g_database.Store( *unittest3b );
		g_database.Store( *unittest4 );
		g_database.Store( *unittest5 );
		g_database.Store( *unittest6 );

		//Give the unit test game objects a state machine
		unittest1->GetStateMachineManager()->PushStateMachine( *new UnitTest1( *unittest1 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest2->GetStateMachineManager()->PushStateMachine( *new UnitTest2a( *unittest2 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest3a->GetStateMachineManager()->PushStateMachine( *new UnitTest3a( *unittest3a ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest3b->GetStateMachineManager()->PushStateMachine( *new UnitTest3b( *unittest3b ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest4->GetStateMachineManager()->PushStateMachine( *new UnitTest4( *unittest4 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest5->GetStateMachineManager()->PushStateMachine( *new UnitTest5( *unittest5 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest6->GetStateMachineManager()->PushStateMachine( *new UnitTest6( *unittest6 ), STATE_MACHINE_QUEUE_0, TRUE );

#else

	// Create the default scene
	g_pScene = new Node(pd3dDevice);

	// Instance level.grd that was previously loaded. 
	std::tr1::shared_ptr<ModelNode> pModelNode(new ModelNode(pd3dDevice));
	HRESULT hr = pModelNode->GetInstance(L"level.grd");
	if (FAILED(hr))
    {
        MessageBox( DXUTGetHWND(), L"Error getting instance of level.grd.", DXUTGetWindowTitle(), MB_ICONERROR|MB_OK );
//        delete pModelNode;
        return;
    }
	g_pScene->AddChild(pModelNode);

#if 0
	// Place .x meshes and .grd levels that have been previously loaded into 
	// the cene graph. We have no intention of moving these models, so we add 
	// them directly to the scene graph without also adding a controller to 
	// update the model's location. 
	{
	std::tr1::shared_ptr<ModelNode> pModelNode(new ModelNode(pd3dDevice));
	HRESULT hr = pModelNode->GetInstance(L"airplane 2.x");
//	hr = pModelNode->GetInstance(L"tiger.x");
	if (FAILED(hr))
    {
        MessageBox( DXUTGetHWND(), L"Error getting instance of airplane 2.x.", DXUTGetWindowTitle(), MB_ICONERROR|MB_OK );
//        delete pModelNode;
        return;
    }
	// airplane 2.x is too big, so scale it down to one fourth of the original size. 
	D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale, 0.25f, 0.25f, 0.25f);
	D3DXMATRIX matTrans;
	// Y offset makes it look like it is resting on the ground. 
	D3DXMatrixTranslation(&matTrans, 17.0f, 0.5f, 22.0f);
	D3DXMatrixMultiply(pModelNode->GetMatrix(), &matScale, &matTrans);
	g_pScene->AddChild(pModelNode);
	}
	{
	std::tr1::shared_ptr<ModelNode> pModelNode(new ModelNode(pd3dDevice));
	hr = pModelNode->GetInstance(L"airplane 2.x");
	if (FAILED(hr))
    {
        MessageBox( DXUTGetHWND(), L"Error getting instance of airplane 2.x.", DXUTGetWindowTitle(), MB_ICONERROR|MB_OK );
//        delete pModelNode;
        return;
    }
	// airplane 2.x is too big, so scale it down by half. 
	D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale, 0.5f, 0.5f, 0.5f);
	D3DXMATRIX matTrans;
	// Y offset makes it look like it is floating in the air.  
	D3DXMatrixTranslation(&matTrans, 17.0f, 1.0f, 22.0f);
	D3DXMatrixMultiply(pModelNode->GetMatrix(), &matScale, &matTrans);
	g_pScene->AddChild(pModelNode);
	}
#endif


	D3DXVECTOR3 pos(15.5f, 0.0f, 20.5f);
#if PLAYER
// Create player. 
    char name[10] = "Player";
    g_objectIDPlayer = g_database.GetNewObjectID();
    GameObject* player = new GameObject( g_objectIDPlayer, OBJECT_Player, name );
   
    player->CreateBody( 100, pos );
    player->GetBody().SetRadius(fCollRadiusPlayer);
	D3DXVECTOR3 Dir(0,0,-1);
	player->GetBody().SetDir( Dir );
    player->CreateMovement();
#if USE_TINY
	player->CreateTiny( &g_MultiAnim, &g_v_pCharacters, &g_DSound, DXUTGetGlobalTimer()->GetTime() );
#else
	player->SetScale(0.05f);
	player->CreateInstance(pd3dDevice, L"mesh", L"airplane 21.x");
#endif
    player->CreateStateMachineManager();
    g_database.Store( *player );

    //Give the game object a state machine
    player->GetStateMachineManager()->PushStateMachine( *new Player( *player ), STATE_MACHINE_QUEUE_0, TRUE );
#endif

//	GameObject* Leader;  //for Formation
	srand ( timeGetTime());

	for (int i = 1; i < NumberOfMobiles; i++)
	{
		//Create game objects
		char name[10] = "NPC"; 
		sprintf( name, "%s%d", name, i );
		GameObject* npc = new GameObject( g_database.GetNewObjectID(), OBJECT_NPC, name );
		//if(i== 5)		//for Formation
		//	Leader = npc;

//		pos.x = .5f+i*2;	pos.y = 0.0f;	pos.z = .5;
		WorldFile::ECell cell;
		do
		{
			pos.y = 0.0f;
			pos.x = (int)RangedRand(0.f, 24.f)+.5f;
			pos.z = (int)RangedRand(0.f, 24.f)+.5f;
			cell = (*g_pWorldCollData->pwf)((int)pos.z, (int)pos.x);
		}
		while(cell == WorldFile::OCCUPIED_CELL );

		npc->CreateBody( 100, pos );

		// Use airplane for NPC. 
		npc->GetBody().SetRadius(fCollRadiusPlane);
		npc->CreateMovement();
		npc->SetScale(0.05f);
		npc->CreateInstance(pd3dDevice, L"mesh", L"airplane 22.x");

		npc->CreateStateMachineManager();
		g_database.Store( *npc );

		//Give the game object a state machine

//		if(i==1 || i==2)
		npc->GetStateMachineManager()->PushStateMachine( *new PlaneType3( *npc ), STATE_MACHINE_QUEUE_0, TRUE );
		/*else if(i==3 || i==4)
			npc->GetStateMachineManager()->PushStateMachine( *new PlaneType1( *npc ), STATE_MACHINE_QUEUE_0, TRUE );
		else if(i== 5)
			npc->GetStateMachineManager()->PushStateMachine( *new PlaneType2( *npc ), STATE_MACHINE_QUEUE_0, TRUE );
		else if(i== 6)
		{
			D3DXVECTOR3 Offset(2.f, 0.f, 2.f);
			npc->GetStateMachineManager()->PushStateMachine( *new PlaneInFormation( *npc, *Leader, Offset), STATE_MACHINE_QUEUE_0, TRUE );
		}
		else
		{
			D3DXVECTOR3 Offset(-2.f, 0.f,- 2.f);
			npc->GetStateMachineManager()->PushStateMachine( *new PlaneInFormation( *npc, *Leader, Offset), STATE_MACHINE_QUEUE_0, TRUE );
		
		}*/

	}

    GameObject* sound = new GameObject(g_database.GetNewObjectID(), OBJECT_Sound, "Sound");
    sound->CreateStateMachineManager();
    g_database.Store(*sound);
    sound->GetStateMachineManager()->PushStateMachine( *new Sound(*sound), STATE_MACHINE_QUEUE_0, TRUE );

#if VISUALIZEWORLDCOLL
    if (g_pWorldCollData)
    {
        // Multiply by 4 because each CollQuad consists of four line segments. 
        std::tr1::shared_ptr<LineNode> pWorldCollLineNode(new LineNode(pd3dDevice, g_pWorldCollData->GetCCollQuads() * 4));
        for (UINT i = 0; i < g_pWorldCollData->GetCCollQuads(); ++i)
        {
            const CollQuad cq = g_pWorldCollData->GetCollQuad(i);
            // first segment 
            pWorldCollLineNode->SetVertex(i * 8 + 0, cq.point[0].x, cq.point[0].y, cq.point[0].z, 0xffffffff);
            pWorldCollLineNode->SetVertex(i * 8 + 1, cq.point[1].x, cq.point[1].y, cq.point[1].z, 0xffffffff);
            // second segment 
            pWorldCollLineNode->SetVertex(i * 8 + 2, cq.point[1].x, cq.point[1].y, cq.point[1].z, 0xffffffff);
            pWorldCollLineNode->SetVertex(i * 8 + 3, cq.point[2].x, cq.point[2].y, cq.point[2].z, 0xffffffff);
            // third segment 
            pWorldCollLineNode->SetVertex(i * 8 + 4, cq.point[2].x, cq.point[2].y, cq.point[2].z, 0xffffffff);
            pWorldCollLineNode->SetVertex(i * 8 + 5, cq.point[3].x, cq.point[3].y, cq.point[3].z, 0xffffffff);
            // fourth segment
            pWorldCollLineNode->SetVertex(i * 8 + 6, cq.point[3].x, cq.point[3].y, cq.point[3].z, 0xffffffff);
            pWorldCollLineNode->SetVertex(i * 8 + 7, cq.point[0].x, cq.point[0].y, cq.point[0].z, 0xffffffff);
        }
        g_pScene->AddChild(pWorldCollLineNode);
    }
#endif

#endif

	}
}

void World::Update()
{
	g_time.MarkTimeThisTick();
	g_database.Update();
}

void World::Animate( double dTimeDelta )
{
	g_database.Animate( dTimeDelta );
}

void World::AdvanceTimeAndDraw( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj, double dTimeDelta, D3DXVECTOR3 *pvEye )
{
	g_database.AdvanceTimeAndDraw( pd3dDevice, pViewProj, dTimeDelta, pvEye );
}

