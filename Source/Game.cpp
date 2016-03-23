//--------------------------------------------------------------------------------------
// Based upon MultiAnimation.cpp from the DirectX demos. 
//
// Starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "DXUT.h"
#pragma warning(disable: 4995)

#define VISUALIZEPLAYERCOLL 0

#define NUM_ELEMENTS(a)         (sizeof(a) / sizeof(a[0]))

#include <vector>
#include "resource.h"
#include "database.h"
#include "body.h"
#include "ChaseCamera.h"
#include "DebugCamera.h"
#include "DXUT\DXUTsettingsdlg.h"
#include "DXUT\SDKmisc.h"
#include "DXUT\SDKsound.h"
#include "Game.h"
#include "GameError.h"
#include "ModelFiles.h"
#include "Player.h"
#include "LineNode.h"
//#include "TeapotNode.h"
#include "TransNode.h"
#include "World.h"
#include "WorldCollData.h"
#include "WorldModelFile.h"
#include "Minimap.h"
#include "LayerDrawing.h"
#include "Stars.h"
#include "Decal.h"
#include "movement.h"
#include "Utilities.h"

#define MULTIANIMATION	0

#if MULTIANIMATION
#include "MultiAnimation.h"
#include "Tiny.h"
#endif

using namespace std;

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;         // Font for drawing text
ID3DXFont*              g_pFontTitle = NULL;         // Font for drawing text
ID3DXSprite*            g_pTextSprite = NULL;   // Sprite for batching draw text calls
//CFirstPersonCamera      g_Camera;               // A model viewing camera
//CDebugCamera            g_Camera;               // A debug camera
CBaseCamera*            g_pCamera;              // The camera
CBaseCamera*            g_apCamera[2];          // array of cameras 
int                     g_ipCamera = 0;         // index into array of cameras 
CDXUTDialogResourceManager g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg         g_SettingsDlg;          // Device settings dialog
CDXUTDialog             g_HUD;                  // dialog for standard controls
CDXUTDialog             g_SampleUI;             // dialog for sample specific controls
bool                    g_bShowHelp = false;     // If true, it renders the UI control text
bool                    g_bPlaySounds = true;   // whether to play sounds
double                  g_fLastAnimTime = 0.0;  // Time for the animations
World*					g_pWorld = NULL;				// World for creating singletons and objects
WorldCollData*          g_pWorldCollData = 0;   // World collision data 

objectID				g_objectIDPlayer = 1;
CSoundManager           g_DSound;               // DirectSound class
#if MULTIANIMATION
//L CSoundManager           g_DSound;               // DirectSound class
CMultiAnim              g_MultiAnim;            // the MultiAnim class for holding Tiny's mesh and frame hierarchy
vector <CTiny*>         g_v_pCharacters;        // array of character objects; each can be associated with any of the CMultiAnims

#endif

CModelFiles				g_ModelFiles;			// Holds all the models 

Node*					g_pScene;				// Just use a vector of Nodes* for our scene. 

Minimap*				g_pMinimap;				// The player's minimap

Stars* g_pStar = NULL;
Decal* pDecal = NULL;

#if VISUALIZEPLAYERCOLL
ID3DXMesh*              g_pPlayerSphere = 0;
#endif

DWORD					g_BackBufferWidth  = 0;	// Back buffer dimensions, used to position UI elements.
DWORD					g_BackBufferHeight = 0;


Layers layer = DEFAULT;
LayerDrawing*  g_pLayerDraw;

float InitTime = 0.f;
bool bRenderFirstTime = true; 
GameResult g_Result = GameContinues;
bool b_gAgainButtonCreated = false;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_NEXTVIEW            6
#define IDC_PREVVIEW            7
#define IDC_RESETCAMERA         11
#define IDC_RESETTIME           12
#define IDC_RESETGAME           13


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------

void    CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );

void    RenderText();
void RenderTitleScreen(IDirect3DDevice9* pd3dDevice);
void RenderEndScreen(IDirect3DDevice9* pd3dDevice);


//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
bool InitApp()
{
    // Initialize dialogs
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); int iY = 10;
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;
    g_SampleUI.AddButton( IDC_NEXTVIEW, L"(N)ext View", 45, iY += 26, 120, 24, L'N' );
    g_SampleUI.AddButton( IDC_PREVVIEW, L"(P)revious View", 45, iY += 26, 120, 24, L'P' );
    g_SampleUI.AddButton( IDC_RESETCAMERA, L"(R)eset view", 45, iY += 26, 120, 24, L'R' );
    //g_SampleUI.AddButton( IDC_RESETTIME, L"Reset time", 45, iY += 26, 120, 24 );

    // Add mixed vp to the available vp choices in device settings dialog.
    DXUTGetD3D9Enumeration()->SetPossibleVertexProcessingList( true, false, false, true );

    // Setup the camera with view matrix
    // Position the camera so we can see the entire level. 
  /*L  D3DXVECTOR3 vEye(11.50f, 1.00f, 5.00f);
    D3DXVECTOR3 vAt(13.50f, 0.05f, 5.00f);*/

//	D3DXVECTOR3 vEye(13.50f, 12.00f, -6.00f);
//	D3DXVECTOR3 vAt(13.50f, 0.05f, 8.0f);
	/*D3DXVECTOR3 vEye(13.50f, 3.5f, -6.00f);
    D3DXVECTOR3 vAt(13.50f, 3.f, 2.5f);*/

	g_apCamera[0] = new CChaseCamera;
	g_apCamera[1] = new CDebugCamera;
	g_ipCamera = 0;

	g_pCamera = g_apCamera[g_ipCamera];

	{
		D3DXVECTOR3 vEye(0.f, 32.f, 23.40f);
		D3DXVECTOR3 vAt(0.f, 0.05f, 12.50f);
		g_apCamera[0]->SetViewParams( &vEye, &vAt );
	}
	{
		D3DXVECTOR3 vEye(12.50f, 32.f, 12.40f);
		D3DXVECTOR3 vAt(12.5f, 0.05f, 12.50f);
		g_apCamera[1]->SetViewParams( &vEye, &vAt );
	}
	/*for (int i=0; i<2; i++)
	{
		g_apCamera[i]->SetViewParams( &vEye, &vAt );
		
	}*/


    g_pCamera->SetScalers( 0.01f, 1.0f );  // Camera movement parameters

	g_pWorld = new World();
	g_pWorld->InitializeSingletons();

    // Load world collision data. 
    g_pWorldCollData = new WorldCollData();
    if (!g_pWorldCollData->Load(L"level.grd"))
    {
        ShowError();
//BUGBUG FIXME        MessageBox( DXUTGetHWND(), L"Error loading level.grd.", DXUTGetWindowTitle(), MB_ICONERROR|MB_OK );
//BUGBUG FIXME        delete g_pScene;
        return false;
    }
//	g_pLayerDraw->InitLayers();
	return true;
}


//--------------------------------------------------------------------------------------
// Clean up the app
//--------------------------------------------------------------------------------------
void    CleanupApp()
{
	objectID idSound = g_database.GetIDByName("Sound");
	if (idSound != INVALID_OBJECT_ID )
	{
		g_database.Remove(idSound);
	}
	SAFE_DELETE(g_pScene);
    SAFE_DELETE(g_pWorldCollData);
	for (int i = NUM_ELEMENTS(g_apCamera) - 1; i >= 0; --i)
	{
		SAFE_DELETE(g_apCamera[i]);
	}
#if MULTIANIMATION
    SAFE_DELETE(g_apSoundsTiny[ 0 ]);
    SAFE_DELETE(g_apSoundsTiny[ 1 ]);
#endif
	delete g_pWorld;
}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // Need to support ps 1.1
    if( pCaps->PixelShaderVersion < D3DPS_VERSION( 1, 1 ) )
        return false;

    // Need to support A8R8G8B8 textures
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, 0,
                                         D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8 ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the
// application to modify the device settings. The supplied pDeviceSettings parameter
// contains the settings that the framework has selected for the new device, and the
// application can make any desired changes directly to this structure.  Note however that
// DXUT will not correct invalid device settings so care must be taken
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW
    // then switch to SWVP.
    if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

     // If the hardware cannot do vertex blending, use software vertex processing.
    if( pCaps->MaxVertexBlendMatrices < 2 )
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    // If using hardware vertex processing, change to mixed vertex processing
    // so there is a fallback.
    if( pDeviceSettings->d3d9.BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;

    // Debugging vertex shaders requires either REF or software vertex processing
    // and debugging pixel shaders requires REF.
#ifdef DEBUG_VS
    if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
        pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning(  pDeviceSettings->ver  );
    }

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// created, which will happen during application initialization and windowed/full screen
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these
// resources need to be reloaded whenever the device is destroyed. Resources created
// here should be released in the OnDestroyDevice callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnD3D9CreateDevice( pd3dDevice ) );
    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 18, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                         L"Arial", &g_pFont ) );

	 V_RETURN( D3DXCreateFont( pd3dDevice, 80, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                         L"Arial", &g_pFontTitle ) );


	OutputDebugString(L"Loading level.grd\n");
	if (g_ModelFiles.Load(pd3dDevice, L"level.grd") < 0)
	{
		ShowError();
		return MAKE_HRESULT(SEVERITY_ERROR, 0, 0);
	}

#if 1
	OutputDebugString(L"Loading airplane 22.x\n");
	if (g_ModelFiles.Load(pd3dDevice, L"airplane 22.x") < 0)
	{
		ShowError();
		return MAKE_HRESULT(SEVERITY_ERROR, 0, 0);
	}
	OutputDebugString(L"Loading airplane 21.x\n");
	if (g_ModelFiles.Load(pd3dDevice, L"airplane 21.x") < 0)
	{
		ShowError();
		return MAKE_HRESULT(SEVERITY_ERROR, 0, 0);
	}
#endif

#if VISUALIZEPLAYERCOLL
    D3DXCreateSphere(pd3dDevice, fCollRadiusPlayer, 10, 10, &g_pPlayerSphere, NULL);
#endif
	if (g_pScene)
	{
		g_pScene->Reload(pd3dDevice);
	}

	{
		CWorldModelFile* const pWorldModelFile =
				dynamic_cast<CWorldModelFile*>(g_ModelFiles.PIModelFileFromName(L"level.grd"));
		if (pWorldModelFile == NULL)
		{
			ShowError();
			return MAKE_HRESULT(SEVERITY_ERROR, 0, 0);
		}

		WorldFile& worldFile = pWorldModelFile->GetWorldFile();
		g_pMinimap = new Minimap(pd3dDevice, &worldFile);
	}
	g_pLayerDraw = new LayerDrawing;
	g_pLayerDraw->InitLayers();

	g_pStar = new Stars(pd3dDevice);
	pDecal = new Decal(pd3dDevice, .42f, 15.5f +(.5f-.42f), 0.5f ,20.2f);

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// reset, which will happen after a lost device scenario. This is the best location to
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever
// the device is lost. Resources created here should be released in the OnLostDevice
// callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice,
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D9ResetDevice() );
    V_RETURN( g_SettingsDlg.OnD3D9ResetDevice() );
#if MULTIANIMATION
    // set up MultiAnim
    WCHAR sXFile[MAX_PATH];
    WCHAR str[MAX_PATH];

	OutputDebugString(L"Loading tiny_4anim.x\n");
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"Media\\Tiny\\MultiAnimation.fx" ) );
    V_RETURN( DXUTFindDXSDKMediaFileCch( sXFile, MAX_PATH, L"Media\\Tiny\\tiny_4anim.x" ) );

    CMultiAnimAllocateHierarchy AH;
    AH.SetMA( &g_MultiAnim );

    V_RETURN( g_MultiAnim.Setup( pd3dDevice, sXFile, str, &AH ) );

    // Select the technique that fits the shader version.
    // We could have used ValidateTechnique()/GetNextValidTechnique() to find the
    // best one, but this is convenient for our purposes.
    g_MultiAnim.SetTechnique( "Skinning20" );

    // Restore steps for tiny instances
    vector <CTiny*>::iterator itCurCP, itEndCP = g_v_pCharacters.end();
    for( itCurCP = g_v_pCharacters.begin(); itCurCP != itEndCP; ++ itCurCP )
    {
        ( *itCurCP )->RestoreDeviceObjects( pd3dDevice );
    }

//L	g_pWorld->Initialize(pd3dDevice);

	ID3DXEffect* pMAEffect = g_MultiAnim.GetEffect();
    if( pMAEffect )
    {
        pMAEffect->OnResetDevice();
        pMAEffect->Release();
    }

#endif
	g_pWorld->Initialize(pd3dDevice);
    // get device caps
    D3DCAPS9 caps;
    pd3dDevice->GetDeviceCaps( & caps );

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );
	if( g_pFontTitle )
        V_RETURN( g_pFontTitle->OnResetDevice() );

	// Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_pCamera->SetProjParams( D3DX_PI/3, fAspectRatio, 0.001f, 100.0f );

	// Material sources
	pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	pd3dDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	pd3dDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	pd3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

	// Initialize default material
	D3DMATERIAL9		materialDefault;
	materialDefault.Ambient = D3DXCOLOR(1, 1, 1, 1.0f);		// White to ambient lighting
	materialDefault.Diffuse = D3DXCOLOR(1, 1, 1, 1.0f);		// White to diffuse lighting
	materialDefault.Emissive = D3DXCOLOR(0, 0, 0, 1.0);		// No emissive
	materialDefault.Power = 0;
	materialDefault.Specular = D3DXCOLOR(0, 0, 0, 1.0);		// No specular
	pd3dDevice->SetMaterial(&materialDefault);

	// Set a default light
	// White, directional, pointing downward
	D3DLIGHT9 light;
	memset(&light, 0, sizeof(light));
	light.Type			= D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r		= 1.0f;
	light.Diffuse.g		= 1.0f;
	light.Diffuse.b		= 1.0f;
	light.Ambient.r		= 0.5f;
	light.Ambient.g		= 0.5f;
	light.Ambient.b		= 0.5f;
	light.Direction.x	= 0.0f;
	light.Direction.y	= -1.0f;
	light.Direction.z	= 0.0f;
	light.Attenuation0	= 1.0f;
	light.Range			= 10000.f;
	pd3dDevice->SetLight(0, &light);
	pd3dDevice->LightEnable(0, TRUE);
    pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);
	pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, false);
    pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, false);
    pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

    pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

    // reset the timer
    g_fLastAnimTime = DXUTGetGlobalTimer()->GetTime();

    // Adjust the dialog parameters.
    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width-170, pBackBufferSurfaceDesc->Height-270 );
    g_SampleUI.SetSize( 170, 220 );

	g_BackBufferWidth  = pBackBufferSurfaceDesc->Width;
	g_BackBufferHeight = pBackBufferSurfaceDesc->Height;

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not
// intended to contain actual rendering calls, which should instead be placed in the
// OnFrameRender callback.
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	g_pScene->Update(fTime);

	g_pWorld->Update();
	g_pWorld->Animate( fTime - g_fLastAnimTime );

    g_fLastAnimTime = fTime;

    // Update the camera's position based on user input
    g_pCamera->FrameMove( fElapsedTime );
	g_pStar->FrameMove( fElapsedTime );
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the
// rendering calls for the scene, and it will also be called if the window needs to be
// repainted. After this function has returned, DXUT will call
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    // If the settings dialog is being shown, then
    // render it instead of rendering the app's scene
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }

    if( bRenderFirstTime )
	{
		InitTime = g_time.GetCurTime();
		bRenderFirstTime = false;

	}
    HRESULT hr;


	if( g_time.GetCurTime() - InitTime < 2.5f )
	{
		RenderTitleScreen( pd3dDevice );
		return;
	}

	pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                       D3DCOLOR_ARGB( 0, 0x3F, 0xAF, 0xFF ), 1.0f, 0L );
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        // set up the camera
        D3DXMATRIXA16 mx, mxView, mxProj;
        D3DXVECTOR3 vEye;
        D3DXVECTOR3 vLightDir;

		// I think this is a KLUDGE, but I haven't figured out another 
		// setting for lighting Tiny that doesn't look worse. . . 

		// Light direction is same as camera front (reversed)
        vLightDir = -g_pCamera->GetWorldAhead();

        // set static transforms
        mxView = *g_pCamera->GetViewMatrix();
        mxProj = *g_pCamera->GetProjMatrix();
        V( pd3dDevice->SetTransform( D3DTS_VIEW, & mxView ) );
        V( pd3dDevice->SetTransform( D3DTS_PROJECTION, & mxProj ) );
        vEye = *g_pCamera->GetEyePt();

		// set up projection matrix and light for drawing Tiny 
        GameObject* goPlayer = g_database.Find(g_objectIDPlayer + 1);
        if (goPlayer)
        {
//L #if 01
#if MULTIANIMATION
            ID3DXEffect *pMAEffect = g_MultiAnim.GetEffect();
            if( pMAEffect )
            {
                // set view-proj
                D3DXMatrixMultiply( &mx, &mxView, &mxProj );
                pMAEffect->SetMatrix( "g_mViewProj", &mx );

                // Set the light direction so that the
                // visible side is lit.
                D3DXVECTOR4 v( vLightDir.x, vLightDir.y, vLightDir.z, 1.0f );
                pMAEffect->SetVector( "lhtDir", &v );
            }
            SAFE_RELEASE( pMAEffect );
#endif
        }

        // Render the scene graph
        D3DXMATRIX	matIdentity;		// identity matrix
        D3DXMatrixIdentity( &matIdentity );


		g_pScene->Render(pd3dDevice, matIdentity);
		if( layer != DEFAULT)
		{
			D3DXCOLOR xColor( .9f, .9f ,.9f, 1.0f );
			g_pLayerDraw->DrawLayer( pd3dDevice, layer, xColor); 
		}

//L		g_pWorld->AdvanceTimeAndDraw( pd3dDevice, &mx, fElapsedTime, &vEye );

#if VISUALIZEPLAYERCOLL
        D3DXVECTOR3 vPlayerPos = g_v_pCharacters[0]->GetOwner()->GetBody().GetPos();
        vPlayerPos.y += fCollYOffsetPlayer;
        D3DXMatrixTranslation(&matIdentity, vPlayerPos.x, vPlayerPos.y, vPlayerPos.z);
	    pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity);
	    g_pPlayerSphere->DrawSubset(0);

#endif
		pd3dDevice->SetTransform(D3DTS_WORLD, &matIdentity);
		g_pStar->Render(pd3dDevice);
		g_pMinimap->Render(pd3dDevice);
		pDecal->Render(pd3dDevice);
		

        //
        // Output text information
        //
        RenderText();
		if( g_Result !=  GameContinues)
			RenderEndScreen(pd3dDevice);

        V( g_HUD.OnRender( fElapsedTime ) );
        V( g_SampleUI.OnRender( fElapsedTime ) );

        pd3dDevice->EndScene();
    }
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
	  // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work however the
    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );
    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();

    int start = 5;
	int count = 0;
    txtHelper.Begin();
	for(int i = 0; i < NumberOfMobiles; i++)
	{
		GameObject* goPlayer = g_database.Find(g_objectIDPlayer+i);
		int health = (int)goPlayer->GetBody().GetHealth();
		txtHelper.SetForegroundColor( D3DXCOLOR( 0.f, 0.0f, 0.f, 1.0f ) );
		txtHelper.SetInsertionPos( 6, start+(16*count) );
		char* name = goPlayer->GetName();
		TCHAR* unicode_name = new TCHAR[strlen(name)+1];
		mbstowcs(unicode_name, name, strlen(name)+1);
		txtHelper.DrawFormattedTextLine( L"%s: health  %d", unicode_name, health );
		if(!i)
			txtHelper.SetForegroundColor( D3DXCOLOR( .6f, .4f, 1.4f, 1.0f ) );
		else
			txtHelper.SetForegroundColor( D3DXCOLOR( .3f, .7f, 0.f, 1.0f ) );
		txtHelper.SetInsertionPos( 4, start+1+(16*count++) );
		txtHelper.DrawFormattedTextLine( L"%s: health  %d", unicode_name, health );
		delete unicode_name;
	}
	// Output statistics
   /* txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );*/
    //txtHelper.DrawTextLine( DXUTGetFrameStats() );
    //txtHelper.DrawTextLine( DXUTGetDeviceStats() );

    // Dump out the FPS and device stats
    //txtHelper.SetInsertionPos( 5, 150 );
    //txtHelper.DrawFormattedTextLine( L"  Time: %2.3f", DXUTGetGlobalTimer()->GetTime() );
    //txtHelper.DrawFormattedTextLine( L"  Number of models: %d", g_v_pCharacters.size() );

	
if( g_bShowHelp )
{

	// Print out states
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
	dbCompositionList list;
	g_database.ComposeList( list, OBJECT_Ignore_Type );
	int starttext = start +20;
//	int count = 0;
	dbCompositionList::iterator i;
	for( i=list.begin(); i!=list.end(); ++i )
	{
		StateMachine* pStateMachine = (*i)->GetStateMachineManager()->GetStateMachine(STATE_MACHINE_QUEUE_0);
		if( pStateMachine )
		{
			char* name = (*i)->GetName();
			char* statename = pStateMachine->GetCurrentStateNameString();
			char* substatename = pStateMachine->GetCurrentSubstateNameString();
			TCHAR* unicode_name = new TCHAR[strlen(name)+1];
			TCHAR* unicode_statename = new TCHAR[strlen(statename)+1];
			TCHAR* unicode_substatename = new TCHAR[strlen(substatename)+1];
			mbstowcs(unicode_name, name, strlen(name)+1);
			mbstowcs(unicode_statename, statename, strlen(statename)+1);
			mbstowcs(unicode_substatename, substatename, strlen(substatename)+1);
			if( substatename[0] != 0 )
			{
				txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
				txtHelper.SetInsertionPos( 5, starttext-1+(16*count) );
				txtHelper.DrawFormattedTextLine( L"%s:   %s, %s", unicode_name, unicode_statename, unicode_substatename );
				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
				txtHelper.SetInsertionPos( 4, starttext+(16*count++) );
				txtHelper.DrawFormattedTextLine( L"%s:   %s, %s", unicode_name, unicode_statename, unicode_substatename );
			}
			else
			{
				txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
				txtHelper.SetInsertionPos( 4, starttext-1+(16*count) );
				txtHelper.DrawFormattedTextLine( L"%s:   %s", unicode_name, unicode_statename );
				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
				txtHelper.SetInsertionPos( 5, starttext+(16*count++) );
				txtHelper.DrawFormattedTextLine( L"%s:   %s", unicode_name, unicode_statename );
			}
			delete unicode_name;
			delete unicode_statename;
			delete unicode_substatename;
		}
	}

	if(layer>=1 && layer <=4)
	{
		TCHAR  strLayer[][500] =
		{
//		L"VISIBILITYLAYER",
		L"OPENESSLAYER",
		L"OCCUPANCYLAYER",
		L"COMBINED =OPENESS*OCCUPANCY"
		};
	 
		 txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
		 txtHelper.SetInsertionPos( 5, starttext+10+(16*count) );
		 txtHelper.DrawFormattedTextLine( L"Layer: %s", strLayer[layer-1]  );
		 txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
		 txtHelper.SetInsertionPos( 4, starttext+11+(16*count++) );
		 txtHelper.DrawFormattedTextLine( L"Layer: %s", strLayer[layer-1]  );
	}


    //txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    //txtHelper.SetInsertionPos( 5, 70 );
//    // The helper object simply helps keep track of text position, and color
//    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
//    // If NULL is passed in as the sprite object, then it will work however the
//    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
//    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );
//    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();
//
//    // Output statistics
//    txtHelper.Begin();
//    txtHelper.SetInsertionPos( 5, 5 );
//    GameObject* goPlayer = g_database.Find(g_objectIDPlayer + 1);
//    if (goPlayer)
//    {
//        txtHelper.DrawFormattedTextLine( L"Pos = (%.2f, %.2f, %.2f)", goPlayer->GetBody().GetPos().x, goPlayer->GetBody().GetPos().y, goPlayer->GetBody().GetPos().z);
//    }
//    txtHelper.DrawFormattedTextLine( L"Eye = (%.2f, %.2f, %.2f)", g_pCamera->GetEyePt()->x, g_pCamera->GetEyePt()->y, g_pCamera->GetEyePt()->z);
//    txtHelper.DrawFormattedTextLine( L"LookAt = (%.2f, %.2f, %.2f)", g_pCamera->GetLookAtPt()->x, g_pCamera->GetLookAtPt()->y, g_pCamera->GetLookAtPt()->z);
//#if 1
//	GameObject* goNPC = g_database.FindByName("NPC1");
//	if (goNPC)
//	{
//        txtHelper.DrawFormattedTextLine( L"NPC Pos = (%.2f, %.2f, %.2f)", goNPC->GetBody().GetPos().x, goNPC->GetBody().GetPos().y, goNPC->GetBody().GetPos().z);
//	}
//#endif
}
    txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows
// messages to the application through this callback function. If the application sets
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
    // Always allow dialog resource manager calls to handle global messages
    // so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to the global camera 
    g_pCamera->HandleMessages( hWnd, uMsg, wParam, lParam );

    MapPlayerMessageFromWindowsMessage( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// As a convenience, DXUT inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
            case VK_F1:
				g_bShowHelp = !g_bShowHelp;
				break;
			case 'L':
				layer = (Layers)(layer + 1);
				if(layer == COUNT)
					layer = DEFAULT;
				break;

        }
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:        DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:     g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;

        case IDC_NEXTVIEW:
			if (++g_ipCamera >= NUM_ELEMENTS(g_apCamera))
			{
				g_ipCamera = 0;
			}
			g_pCamera = g_apCamera[g_ipCamera];
            break;

        case IDC_PREVVIEW:
			if (--g_ipCamera < 0)
			{
				g_ipCamera = NUM_ELEMENTS(g_apCamera) - 1;
			}
			g_pCamera = g_apCamera[g_ipCamera];
            break;

        case IDC_RESETCAMERA:
			g_ipCamera = 0;
			g_pCamera = g_apCamera[g_ipCamera];
            break;

        case IDC_RESETTIME:
        {
            DXUTGetGlobalTimer()->Reset();
            g_fLastAnimTime = DXUTGetGlobalTimer()->GetTime();
            break;
        }
		case IDC_RESETGAME:
		{
			g_SampleUI.RemoveControl( IDC_RESETGAME );
			g_Result = GameContinues;
			b_gAgainButtonCreated = false;

			GameObject* gMob;	
			for(int i = 0; i <NumberOfMobiles; i++ )
			{
				gMob = g_database.Find( g_objectIDPlayer + i );
				Body& body = gMob->GetBody();
				if(!i)
				{
					D3DXVECTOR3 pos(15.5f, 0.0f, 20.5f);
					body.SetPos(pos);
					D3DXVECTOR3 Dir(0,0,-1);
					body.SetDir( Dir );
				    ((CChaseCamera*)g_apCamera[0])->SetAfterPlayer();
				}
				else
				{
					WorldFile::ECell cell;
					D3DXVECTOR3 pos;
					do
					{
						pos.y = 0.0f;
						pos.x = (int)RangedRand(0.f, 24.f)+.5f;
						pos.z = (int)RangedRand(0.f, 24.f)+.5f;
						cell = (*g_pWorldCollData->pwf)((int)pos.z, (int)pos.x);
					}
					while(cell == WorldFile::OCCUPIED_CELL );
					body.SetPos(pos);

				}

				body.SetHealth(100);

			}
		}
	}
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for
// information about lost devices.
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D9LostDevice();
    g_SettingsDlg.OnD3D9LostDevice();
    if( g_pFont )
        g_pFont->OnLostDevice();
	if( g_pFontTitle )
        g_pFontTitle->OnLostDevice();
//L
#if MULTIANIMATION
    ID3DXEffect *pMAEffect = g_MultiAnim.GetEffect();
    if( pMAEffect )
    {
        pMAEffect->OnLostDevice();
        pMAEffect->Release();
    }

//L    SAFE_RELEASE( g_pTextSprite );

    vector< CTiny* >::iterator itCurCP, itEndCP = g_v_pCharacters.end();
    for( itCurCP = g_v_pCharacters.begin(); itCurCP != itEndCP; ++ itCurCP )
    {
        ( *itCurCP )->InvalidateDeviceObjects();
    }

    CMultiAnimAllocateHierarchy AH;
    AH.SetMA( & g_MultiAnim );
    g_MultiAnim.Cleanup( & AH );
#endif
	SAFE_RELEASE( g_pTextSprite );
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// been destroyed, which generally happens as a result of application termination or
// windowed/full screen toggles. Resources created in the OnCreateDevice callback
// should be released here, which generally includes all D3DPOOL_MANAGED resources.
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	SAFE_DELETE(g_pMinimap);
	delete g_pLayerDraw;
	SAFE_DELETE(g_pStar);

	if (g_pScene)
	{
		g_pScene->Unload();
	}
//	delete g_pWorld;
	g_ModelFiles.Unload();
#if VISUALIZEPLAYERCOLL
    SAFE_RELEASE(g_pPlayerSphere);

#endif
    g_DialogResourceManager.OnD3D9DestroyDevice();
    g_SettingsDlg.OnD3D9DestroyDevice();
    SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pFontTitle);
	SAFE_DELETE(pDecal);
}

void RenderTitleScreen(IDirect3DDevice9* pd3dDevice)
{
	pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                       D3DCOLOR_ARGB( 0, 0x3F, 0xAF, 0xFF ), 1.0f, 0L );

	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		D3DVIEWPORT9 viewport = { 0 };
		pd3dDevice->GetViewport(&viewport);
		RECT font_rect;
		SetRect(&font_rect,6,6,viewport.Width,viewport.Height-30);

		g_pFontTitle->DrawText(NULL,        //pSprite
									L"PLAY\n\rPLANE",  //pString
									-1,          //Count
									&font_rect,  //pRect
									DT_CENTER|DT_NOCLIP|DT_VCENTER ,//Format,
									D3DXCOLOR(.2f, 0.0f, .4f, 1.0f )); //Color

		SetRect(&font_rect,0,0,viewport.Width,viewport.Height-30);

		g_pFontTitle->DrawText(NULL,        //pSprite
									L"PLAY\n\rPLANE",  //pString
									-1,          //Count
									&font_rect,  //pRect
									DT_CENTER|DT_NOCLIP|DT_VCENTER ,//Format,
									D3DXCOLOR( .6f, .4f, 1.4f, 1.0f )); //Color

		pd3dDevice->EndScene();	
	}
}

void RenderEndScreen(IDirect3DDevice9* pd3dDevice)
{
	D3DVIEWPORT9 viewport = { 0 };
	pd3dDevice->GetViewport(&viewport);
	RECT font_rect;
	SetRect(&font_rect,6,6,viewport.Width,viewport.Height-20);
	TCHAR  strResult[][100] =
		{
			L"YOU WON!",
			L"YOU LOST"
		};

	g_pFontTitle->DrawText(NULL,        //pSprite
								strResult[(int)g_Result - 1],  //pString
								-1,          //Count
								&font_rect,  //pRect
								DT_CENTER|DT_NOCLIP|DT_VCENTER ,//Format,
								D3DXCOLOR(.2f, 0.0f, .4f, 1.0f )); //Color

	SetRect(&font_rect,0,0,viewport.Width,viewport.Height-20);

	g_pFontTitle->DrawText(NULL,        //pSprite
								strResult[(int)g_Result - 1],  //pString
								-1,          //Count
								&font_rect,  //pRect
								DT_CENTER|DT_NOCLIP|DT_VCENTER ,//Format,
								D3DXCOLOR( .6f, .4f, 1.4f, 1.0f )); //Color

	if( !b_gAgainButtonCreated )
	{
		POINT Pt;
		g_SampleUI.GetLocation( Pt );
		int ButtonWidth = 200;
		int ButtonHeight = 40;
		g_SampleUI.AddButton( IDC_RESETGAME, L"Play (A)gain", viewport.Width/2 - Pt.x  - ButtonWidth/2,viewport.Height/2 - Pt.y + 50 , ButtonWidth, ButtonHeight, L'A' );
		b_gAgainButtonCreated = true;
	}


}