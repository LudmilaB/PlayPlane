//-----------------------------------------------------------------------------
// File: Tiny.cpp
//
// Desc: Defines the character class, CTiny, for the MultipleAnimation sample.
//       The class does some basic things to make the character aware of
//       its surroundings, as well as implements all actions and movements.
//       CTiny shows a full-featured example of this.  These
//       classes use the MultiAnimation class library to control the
//       animations.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUT/SDKmisc.h"
#include "GameError.h"
#pragma warning(disable: 4995)
#include "MultiAnimation.h"
#include "DXUT/SDKsound.h"
#include "Tiny.h"
#include "InstanceNode.h"
#include "gameobject.h"
#include "body.h"
#include "movement.h"
#pragma warning(default: 4995)

using namespace std;


CSound *g_apSoundsTiny[ 2 ];




//-----------------------------------------------------------------------------
// Name: CTiny::CTiny
// Desc: Constructor for CTiny
//-----------------------------------------------------------------------------
CTiny::CTiny( GameObject& owner ) :
	CInstance(owner), 
    m_pMA( NULL ),
    m_dwMultiAnimIdx( 0 ),
    m_pAI( NULL ),
    m_pv_pChars( NULL ),
    m_pSM( NULL ),

    m_dTimePrev( 0.0 ),
    m_dTimeCurrent( 0.0 ),
    m_bPlaySounds( true ),
    m_dwCurrentTrack( 0 ),

    //m_fSpeed( 0.f ),
    m_fSpeedTurn( 0.f ),
    m_pCallbackHandler( NULL ),
    m_fPersonalRadius( 0.f ),

    m_bIdle( false ),
    m_bWaiting( false ),
    m_dTimeIdling( 0.0 ),
    m_dTimeWaiting( 0.0 ),
    m_dTimeBlocked( 0.0 )
{
    D3DXMatrixIdentity( &m_mxOrientation );

    m_fSpeedTurn = 1.3f;
    m_pCallbackHandler = NULL;
    m_fPersonalRadius = .035f;

    m_szASName[0] = '\0';
}




//-----------------------------------------------------------------------------
// Name: CTiny::~CTiny
// Desc: Destructor for CTiny
//-----------------------------------------------------------------------------
CTiny::~CTiny()
{
    Cleanup();

    vector< CTiny* >::iterator itCur, itEnd = m_pv_pChars->end();
    for( itCur = m_pv_pChars->begin(); itCur != itEnd; ++ itCur )
	{
        if( ( * itCur ) == this )
		{
			m_pv_pChars->erase( itCur );
			break;
		}
	}
}




//-----------------------------------------------------------------------------
// Name: CTiny::Setup
// Desc: Initializes the class and readies it for animation
//-----------------------------------------------------------------------------
HRESULT CTiny::Setup( CMultiAnim *pMA,
                           vector< CTiny* > *pv_pChars,
                           CSoundManager *pSM,
                           double dTimeCurrent )
{
    m_pMA = pMA;
    m_pv_pChars = pv_pChars;
    m_pSM = pSM;

    m_dTimeCurrent = m_dTimePrev = dTimeCurrent;

    HRESULT hr;
    hr = m_pMA->CreateNewInstance( &m_dwMultiAnimIdx );
    if( FAILED( hr ) )
        return E_OUTOFMEMORY;

    m_pAI = m_pMA->GetInstance( m_dwMultiAnimIdx );

#if 1
	// BUGBUG: FIXME: it may be wrong to automatically add it to the scene here; 
	// maybe the caller should take care of this. 
	InstanceNode* pin = new InstanceNode(0);
	pin->SetInstance(this);
	m_pNode = std::tr1::shared_ptr<Node>(pin);
	g_pScene->AddChild(m_pNode);
#endif

    // set up anim indices
    m_dwAnimIdxLoiter = GetAnimIndex( "Loiter" );
    m_dwAnimIdxWalk = GetAnimIndex( "Walk" );
    m_dwAnimIdxJog = GetAnimIndex( "Jog" );
    if( m_dwAnimIdxLoiter == ANIMINDEX_FAIL ||
        m_dwAnimIdxWalk == ANIMINDEX_FAIL ||
        m_dwAnimIdxJog == ANIMINDEX_FAIL )
        return E_FAIL;

    // set up callback key data
    m_CallbackData[ 0 ].m_dwFoot = 0;
	m_CallbackData[ 0 ].m_pvTinyPos = &m_owner->GetBody().GetPos();
    m_CallbackData[ 1 ].m_dwFoot = 1;
    m_CallbackData[ 1 ].m_pvTinyPos = &m_owner->GetBody().GetPos();

    // set up footstep callbacks
    SetupCallbacksAndCompression();
    m_pCallbackHandler = new CBHandlerTiny;
    if( m_pCallbackHandler == NULL )
        return E_OUTOFMEMORY;

    // set up footstep sounds
    WCHAR sPath[ MAX_PATH ];
    if( g_apSoundsTiny[ 0 ] == NULL )
    {
        hr = DXUTFindDXSDKMediaFileCch( sPath, MAX_PATH, FOOTFALLSOUND00 );
        if( FAILED( hr ) )
            StringCchCopy( sPath, MAX_PATH, FOOTFALLSOUND00 );

        hr = m_pSM->Create( &g_apSoundsTiny[ 0 ], sPath, DSBCAPS_CTRLVOLUME );
        if( FAILED( hr ) )
        {
            SetError( FOOTFALLSOUND00 L" not found; continuing without sound.\n" );
            ShowError();
            m_bPlaySounds = false;
        }
    }

    if( g_apSoundsTiny[ 1 ] == NULL )
    {
        hr = DXUTFindDXSDKMediaFileCch( sPath, MAX_PATH, FOOTFALLSOUND01 );
        if( FAILED( hr ) )
            StringCchCopy( sPath, MAX_PATH, FOOTFALLSOUND01 );

        hr = m_pSM->Create( &g_apSoundsTiny[ 1 ], sPath, DSBCAPS_CTRLVOLUME );
        if( FAILED( hr ) )
        {
            SetError( FOOTFALLSOUND01 L" not found; continuing without sound.\n" );
            ShowError();
            m_bPlaySounds = false;
        }
    }

    // compute reorientation matrix based on default orientation and bounding radius
    D3DXMATRIX mx;
    float fScale = 4.f * 1.f / m_pMA->GetBoundingRadius() / 7.f;
    D3DXMatrixScaling( &mx, fScale, fScale, fScale );
    m_mxOrientation = mx;
    D3DXMatrixRotationX( &mx, -D3DX_PI / 2.0f );
    D3DXMatrixMultiply( &m_mxOrientation, &m_mxOrientation, &mx );
    D3DXMatrixRotationY( &mx, D3DX_PI / 2.0f );
    D3DXMatrixMultiply( &m_mxOrientation, &m_mxOrientation, &mx );

    LPD3DXANIMATIONCONTROLLER pAC;
    m_pAI->GetAnimController( &pAC );
    pAC->AdvanceTime( m_dTimeCurrent, NULL );
    pAC->Release();

    // Add this instance to the list
    try
    {
        pv_pChars->push_back( this );
    }
    catch( ... )
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CTiny::Cleanup()
// Desc: Performs cleanup tasks for CTiny
//-----------------------------------------------------------------------------
void CTiny::Cleanup()
{
    delete m_pCallbackHandler; m_pCallbackHandler = NULL;
}




//-----------------------------------------------------------------------------
// Name: CTiny::GetAnimInstance()
// Desc: Returns the CAnimInstance object that this instance of CTiny
//       embeds.
//-----------------------------------------------------------------------------
CAnimInstance *CTiny::GetAnimInstance()
{
    return m_pAI;
}




//-----------------------------------------------------------------------------
// Name: CTiny::GetPosition()
// Desc: Returns the position of this instance.
//-----------------------------------------------------------------------------
void CTiny::GetPosition( D3DXVECTOR3 *pV )
{
    //*pV = m_vPos;
	*pV = m_owner->GetBody().GetPos();
}


void CTiny::SetOrientation()
{
	D3DXMATRIX mxWorld;
	m_owner->GetBody().GetMatrix(&mxWorld);
	D3DXMatrixMultiply( &mxWorld, &m_mxOrientation, &mxWorld );
	m_pAI->SetWorldTransform( &mxWorld );
}



//-----------------------------------------------------------------------------
// Name: CTiny::Animate()
// Desc: Advances the local time by dTimeDelta. Determine an action for Tiny,
//       then update the animation controller's tracks to reflect the action.
//-----------------------------------------------------------------------------
void CTiny::Animate( double /* dTimeDelta */ )
{
    // loop the loiter animation back on itself to avoid the end-to-end jerk
    SmoothLoiter();
}


HRESULT CTiny::GetInstance(IDirect3DDevice9* /* pd3dDevice */, const LPCWSTR /* szFilename */)
{
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}


void CTiny::Unload()
{
}


//-----------------------------------------------------------------------------
// Name: CTiny::ResetTime()
// Desc: Resets the local time for this CTiny instance.
//-----------------------------------------------------------------------------
HRESULT CTiny::ResetTime()
{
    m_dTimeCurrent = m_dTimePrev = 0.0;
    return m_pAI->ResetTime();
}




//-----------------------------------------------------------------------------
// Name: CTiny::AdvanceTime()
// Desc: Advances the local animation time by dTimeDelta, and call
//       CAnimInstance to set up its frames to reflect the time advancement.
//-----------------------------------------------------------------------------
HRESULT CTiny::AdvanceTime( double dTimeDelta, const D3DXVECTOR3 *pvEye )
{
    // if we're playing sounds, set the sound source position
    if( m_bPlaySounds )
    {
        m_CallbackData[ 0 ].m_pvCameraPos = pvEye;
        m_CallbackData[ 1 ].m_pvCameraPos = pvEye;
    }
    else    // else, set it to null to let the handler know to be quiet
    {
        m_CallbackData[ 0 ].m_pvCameraPos = NULL;
        m_CallbackData[ 1 ].m_pvCameraPos = NULL;
    }

    m_dTimePrev = m_dTimeCurrent;
    m_dTimeCurrent += dTimeDelta;
    return m_pAI->AdvanceTime( dTimeDelta, m_pCallbackHandler );
}




//-----------------------------------------------------------------------------
// Name: CTiny::Draw()
// Desc: Renders this CTiny instace using the current animation frames.
//-----------------------------------------------------------------------------
HRESULT CTiny::Draw( IDirect3DDevice9* /*pd3dDevice*/, D3DXMATRIX /*matWorld*/ )
{
    return m_pAI->Draw();
}




//-----------------------------------------------------------------------------
// Name: CTiny::Report()
// Desc: Add to the vector of strings, v_sReport, with useful information
//       about this instance of CTiny.
//-----------------------------------------------------------------------------
void CTiny::Report( vector <String>& v_sReport )
{
    WCHAR s[ 256 ];

    try
    {
		D3DXVECTOR3 pos = m_owner->GetBody().GetPos();
        StringCchPrintf( s, 256, L"Pos: %f, %f", pos.x, pos.z );
        v_sReport.push_back( String( s ) );
        StringCchPrintf( s, 256, L"Facing: %f", m_fFacing );
        v_sReport.push_back( String( s ) );
        StringCchPrintf( s, 256, L"Local time: %f", m_dTimeCurrent );
        v_sReport.push_back( String( s ) );
        StringCchPrintf( s, 256, L"Pos Target: %f, %f", m_owner->GetMovement().GetTarget().x, m_owner->GetMovement().GetTarget().z );
        v_sReport.push_back( String( s ) );
        StringCchPrintf( s, 256, L"Facing Target: %f", m_fFacingTarget );
        v_sReport.push_back( String( s ) );
        StringCchPrintf( s, 256, L"Status: %s", m_bIdle ? L"Idle" : ( m_bWaiting ? L"Waiting" : L"Moving" ) );
        v_sReport.push_back( String( s ) );

        // report track data
        LPD3DXANIMATIONCONTROLLER pAC;
        LPD3DXANIMATIONSET pAS;
        D3DXTRACK_DESC td;
        m_pAI->GetAnimController( &pAC );

        pAC->GetTrackAnimationSet( 0, &pAS );
        WCHAR wstr[256];
        MultiByteToWideChar( CP_ACP, 0, pAS->GetName(), -1, wstr, 256 );
        StringCchPrintf( s, 256, L"Track 0: %s%s", wstr, m_dwCurrentTrack == 0 ? L" (current)" : L"" );
        v_sReport.push_back( String( s ) );
        pAS->Release();

        pAC->GetTrackDesc( 0, &td );
        StringCchPrintf( s, 256, L"  Weight: %f", td.Weight );
        v_sReport.push_back( String( s ) );
        StringCchPrintf( s, 256, L"  Speed: %f", td.Speed );
        v_sReport.push_back( String( s ) );
        StringCchPrintf( s, 256, L"  Position: %f", td.Position );
        v_sReport.push_back( String( s ) );
        StringCchPrintf( s, 256, L"  Enable: %s", td.Enable ? L"true" : L"false" );
        v_sReport.push_back( String( s ) );

        pAC->GetTrackAnimationSet( 1, &pAS );
        if( pAS )
        {
            MultiByteToWideChar( CP_ACP, 0, pAS->GetName(), -1, wstr, 256 );
            pAS->Release();
        }
        else
        {
            StringCchPrintf( wstr, 256, L"n/a" );
        }
        StringCchPrintf( s, 256, L"Track 1: %s%s", wstr, m_dwCurrentTrack == 1 ? L" (current)" : L"" );
        v_sReport.push_back( String( s ) );

        pAC->GetTrackDesc( 1, &td );
        StringCchPrintf( s, 256, L"  Weight: %f", td.Weight );
        v_sReport.push_back( String( s ) );
        StringCchPrintf( s, 256, L"  Speed: %f", td.Speed );
        v_sReport.push_back( String( s ) );
        StringCchPrintf( s, 256, L"  Position: %f", td.Position );
        v_sReport.push_back( String( s ) );
        StringCchPrintf( s, 256, L"  Enable: %s", td.Enable ? L"true" : L"false" );
        v_sReport.push_back( String( s ) );

        pAC->Release();
    }
    catch( ... )
    {
    }
}




//-----------------------------------------------------------------------------
// Name: CTiny::SetSounds()
// Desc: Enables or disables the sound support for this instance of CTiny.
//       In this case, whether we hear the footstep sound or not.
//-----------------------------------------------------------------------------
void CTiny::SetSounds( bool bSounds )
{
    m_bPlaySounds = bSounds;
}




//-----------------------------------------------------------------------------
// Name: CTiny::GetAnimIndex()
// Desc: Returns the index of an animation set within this animation instance's
//       animation controller given an animation set name.
//-----------------------------------------------------------------------------
DWORD CTiny::GetAnimIndex( char sString[] )
{
    HRESULT hr;
    LPD3DXANIMATIONCONTROLLER pAC;
    LPD3DXANIMATIONSET pAS;
    DWORD dwRet = ANIMINDEX_FAIL;

    m_pAI->GetAnimController( &pAC );

    for( DWORD i = 0; i < pAC->GetNumAnimationSets(); ++ i )
    {
        hr = pAC->GetAnimationSet( i, &pAS );
        if( FAILED( hr ) )
            continue;

        if( pAS->GetName() &&
            !strncmp( pAS->GetName(), sString, min( strlen( pAS->GetName() ), strlen( sString ) ) ) )
        {
            dwRet = i;
            pAS->Release();
            break;
        }

        pAS->Release();
    }

    pAC->Release();

    return dwRet;
}




//-----------------------------------------------------------------------------
// Name: CTiny::AddCallbackKeysAndCompress()
// Desc: Replaces an animation set in the animation controller with the
//       compressed version and callback keys added to it.
//-----------------------------------------------------------------------------
HRESULT CTiny::AddCallbackKeysAndCompress( LPD3DXANIMATIONCONTROLLER pAC,
                                           LPD3DXKEYFRAMEDANIMATIONSET pAS,
                                           DWORD dwNumCallbackKeys,
                                           D3DXKEY_CALLBACK aKeys[],
                                           DWORD dwCompressionFlags,
                                           FLOAT fCompression )
{
    HRESULT hr;
    LPD3DXCOMPRESSEDANIMATIONSET pASNew = NULL;
    LPD3DXBUFFER pBufCompressed = NULL;

    hr = pAS->Compress( dwCompressionFlags, fCompression, NULL, &pBufCompressed );
    if( FAILED( hr ) )
        goto e_Exit;

    hr = D3DXCreateCompressedAnimationSet( pAS->GetName(),
                                           pAS->GetSourceTicksPerSecond(),
                                           pAS->GetPlaybackType(),
                                           pBufCompressed,
                                           dwNumCallbackKeys,
                                           aKeys,
                                           &pASNew );
	pBufCompressed->Release();

    if( FAILED( hr ) )
        goto e_Exit;

    pAC->UnregisterAnimationSet( pAS );
    pAS->Release();

    hr = pAC->RegisterAnimationSet( pASNew );
    if( FAILED( hr ) )
        goto e_Exit;

    pASNew->Release();
    pASNew = NULL;


e_Exit:
    
    if( pASNew )
        pASNew->Release();

    return hr;
}




//-----------------------------------------------------------------------------
// Name: CTiny::SetupCallbacksAndCompression()
// Desc: Add callback keys to the walking and jogging animation sets in the
//       animation controller for playing footstepping sound.  Then compress
//       all animation sets in the animation controller.
//-----------------------------------------------------------------------------
HRESULT CTiny::SetupCallbacksAndCompression()
{
    LPD3DXANIMATIONCONTROLLER pAC;
    LPD3DXKEYFRAMEDANIMATIONSET pASLoiter, pASWalk, pASJog;

    m_pAI->GetAnimController( &pAC );
    pAC->GetAnimationSet( m_dwAnimIdxLoiter, (LPD3DXANIMATIONSET *) &pASLoiter );
    pAC->GetAnimationSet( m_dwAnimIdxWalk, (LPD3DXANIMATIONSET *) &pASWalk );
    pAC->GetAnimationSet( m_dwAnimIdxJog, (LPD3DXANIMATIONSET *) &pASJog );

    D3DXKEY_CALLBACK aKeysWalk[ 2 ];
    aKeysWalk[ 0 ].Time = 0;
    aKeysWalk[ 0 ].pCallbackData = &m_CallbackData[ 0 ];
    aKeysWalk[ 1 ].Time = float( pASWalk->GetPeriod() / 2.0 * pASWalk->GetSourceTicksPerSecond() );
    aKeysWalk[ 1 ].pCallbackData = &m_CallbackData[ 1 ];

    D3DXKEY_CALLBACK aKeysJog[ 8 ];
    for( int i = 0; i < 8; ++ i )
    {
        aKeysJog[ i ].Time = float( pASJog->GetPeriod() / 8 * (double) i * pASWalk->GetSourceTicksPerSecond() );
        aKeysJog[ i ].pCallbackData = &m_CallbackData[ ( i + 1 ) % 2 ];
    }

    AddCallbackKeysAndCompress( pAC, pASLoiter, 0, NULL, D3DXCOMPRESS_DEFAULT, .8f );
    AddCallbackKeysAndCompress( pAC, pASWalk, 2, aKeysWalk, D3DXCOMPRESS_DEFAULT, .4f );
    AddCallbackKeysAndCompress( pAC, pASJog, 8, aKeysJog, D3DXCOMPRESS_DEFAULT, .25f );

    m_dwAnimIdxLoiter = GetAnimIndex( "Loiter" );
    m_dwAnimIdxWalk = GetAnimIndex( "Walk" );
    m_dwAnimIdxJog = GetAnimIndex( "Jog" );
    if( m_dwAnimIdxLoiter == ANIMINDEX_FAIL ||
        m_dwAnimIdxWalk == ANIMINDEX_FAIL ||
        m_dwAnimIdxJog == ANIMINDEX_FAIL )
        return E_FAIL;

    pAC->Release();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CTiny::SmoothLoiter()
// Desc: If Tiny is loitering, check if we have reached the end of animation.
//       If so, set up a new track to play Loiter animation from the start and
//       smoothly transition to the track, so that Tiny can loiter more.
//-----------------------------------------------------------------------------
void CTiny::SmoothLoiter()
{
    LPD3DXANIMATIONCONTROLLER pAC;
    LPD3DXANIMATIONSET pASTrack, pASLoiter;
    m_pAI->GetAnimController( &pAC );

    // check if we're loitering
    pAC->GetTrackAnimationSet( m_dwCurrentTrack, &pASTrack );
    pAC->GetAnimationSet( m_dwAnimIdxLoiter, &pASLoiter );
    if( pASTrack && pASTrack == pASLoiter )
    {
        D3DXTRACK_DESC td;
        pAC->GetTrackDesc( m_dwCurrentTrack, &td );
        if( td.Position > pASTrack->GetPeriod() - IDLE_TRANSITION_TIME )  // come within the change delta of the end
            SetIdleKey( true );
    }

    SAFE_RELEASE( pASTrack );
    SAFE_RELEASE( pASLoiter );
    SAFE_RELEASE( pAC );
}




//-----------------------------------------------------------------------------
// Name: CTiny::GetSpeedScale()
// Desc: Returns the speed of the current track.
//-----------------------------------------------------------------------------
double CTiny::GetSpeedScale()
{
    LPD3DXANIMATIONCONTROLLER pAC;
    D3DXTRACK_DESC td;

    if( m_bIdle )
        return 1.0;
    else
    {
        m_pAI->GetAnimController( &pAC );
        pAC->GetTrackDesc( m_dwCurrentTrack, &td );
        pAC->Release();

        return td.Speed;
    }
}




//-----------------------------------------------------------------------------
// Name: CTiny::SetMoveKey()
// Desc: Initialize a new track in the animation controller for the movement
//       animation (run or walk), and set up the smooth transition from the idle
//       animation (current track) to it (new track).
//-----------------------------------------------------------------------------
void CTiny::SetMoveKey()
{
    DWORD dwNewTrack = ( m_dwCurrentTrack == 0 ? 1 : 0 );
    LPD3DXANIMATIONCONTROLLER pAC;
    LPD3DXANIMATIONSET pAS;
    m_pAI->GetAnimController( &pAC );

    float speed = fabs(m_owner->GetBody().GetSpeed());
 //L   if (0.0f < speed && speed <= m_owner->GetMovement().GetWalkSpeed())
	if (0.0f < speed && speed <= m_owner->GetMovement().GetLandSpeed())
        pAC->GetAnimationSet( m_dwAnimIdxWalk, &pAS );
//L    else if (m_owner->GetMovement().GetWalkSpeed() < speed)
	else if (m_owner->GetMovement().GetLandSpeed() < speed)
        pAC->GetAnimationSet( m_dwAnimIdxJog, &pAS );

    pAC->SetTrackAnimationSet( dwNewTrack, pAS );
    pAS->Release();

    pAC->UnkeyAllTrackEvents( m_dwCurrentTrack );
    pAC->UnkeyAllTrackEvents( dwNewTrack );

    pAC->KeyTrackEnable( m_dwCurrentTrack, FALSE, m_dTimeCurrent + MOVE_TRANSITION_TIME );
    pAC->KeyTrackSpeed( m_dwCurrentTrack, 0.0f, m_dTimeCurrent, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->KeyTrackWeight( m_dwCurrentTrack, 0.0f, m_dTimeCurrent, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->SetTrackEnable( dwNewTrack, TRUE );
    pAC->KeyTrackSpeed( dwNewTrack, 1.0f, m_dTimeCurrent, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->KeyTrackWeight( dwNewTrack, 1.0f, m_dTimeCurrent, MOVE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );

    m_dwCurrentTrack = dwNewTrack;

    pAC->Release();
}




//-----------------------------------------------------------------------------
// Name: CTiny::SetIdleKey()
// Desc: Initialize a new track in the animation controller for the idle
//       (loiter ) animation, and set up the smooth transition from the
//       movement animation (current track) to it (new track).
//
//       bResetPosition controls whether we start the Loiter animation from
//       its beginning or current position.
//-----------------------------------------------------------------------------
void CTiny::SetIdleKey( bool bResetPosition )
{
    DWORD dwNewTrack = ( m_dwCurrentTrack == 0 ? 1 : 0 );
    LPD3DXANIMATIONCONTROLLER pAC;
    LPD3DXANIMATIONSET pAS;
    m_pAI->GetAnimController( &pAC );

    pAC->GetAnimationSet( m_dwAnimIdxLoiter, &pAS );
    pAC->SetTrackAnimationSet( dwNewTrack, pAS );
    pAS->Release();

    pAC->UnkeyAllTrackEvents( m_dwCurrentTrack );
    pAC->UnkeyAllTrackEvents( dwNewTrack );

    pAC->KeyTrackEnable( m_dwCurrentTrack, FALSE, m_dTimeCurrent + IDLE_TRANSITION_TIME );
    pAC->KeyTrackSpeed( m_dwCurrentTrack, 0.0f, m_dTimeCurrent, IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->KeyTrackWeight( m_dwCurrentTrack, 0.0f, m_dTimeCurrent, IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->SetTrackEnable( dwNewTrack, TRUE );
    pAC->KeyTrackSpeed( dwNewTrack, 1.0f, m_dTimeCurrent, IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    pAC->KeyTrackWeight( dwNewTrack, 1.0f, m_dTimeCurrent, IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
    if( bResetPosition )
        pAC->SetTrackPosition( dwNewTrack, 0.0 );

    m_dwCurrentTrack = dwNewTrack;

    pAC->Release();
}




//-----------------------------------------------------------------------------
// Name: CTiny::RestoreDeviceObjects()
// Desc: Reinitialize necessary objects
//-----------------------------------------------------------------------------
HRESULT CTiny::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
    // Compress the animation sets in the new animation controller
    SetupCallbacksAndCompression();

    LPD3DXANIMATIONCONTROLLER pAC;
    m_pAI->GetAnimController( &pAC );
    pAC->ResetTime();
    pAC->AdvanceTime( m_dTimeCurrent, NULL );

    // Initialize current track
    if( m_szASName[0] != '\0' )
    {
        DWORD dwActiveSet = GetAnimIndex( m_szASName );
        LPD3DXANIMATIONSET pAS = NULL;
        pAC->GetAnimationSet( dwActiveSet, &pAS );
        pAC->SetTrackAnimationSet( m_dwCurrentTrack, pAS );
        SAFE_RELEASE( pAS );
    }

    pAC->SetTrackEnable( m_dwCurrentTrack, TRUE );
    pAC->SetTrackWeight( m_dwCurrentTrack, 1.0f );
    pAC->SetTrackSpeed( m_dwCurrentTrack, 1.0f );

    SAFE_RELEASE( pAC );

    // Call animate to initialize the tracks.
    Animate( 0.0 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CTiny::RestoreDeviceObjects()
// Desc: Free D3D objects so that the device can be reset.
//-----------------------------------------------------------------------------
HRESULT CTiny::InvalidateDeviceObjects()
{
    // Save the current track's animation set name
    // so we can reset it again in RestoreDeviceObjects later.
    LPD3DXANIMATIONCONTROLLER pAC = NULL;
    m_pAI->GetAnimController( &pAC );
    if( pAC )
    {
        LPD3DXANIMATIONSET pAS = NULL;
        pAC->GetTrackAnimationSet( m_dwCurrentTrack, &pAS );
        if( pAS )
        {
            if( pAS->GetName() )
                StringCchCopyA( m_szASName, 64, pAS->GetName() );
            SAFE_RELEASE( pAS );
        }
        SAFE_RELEASE( pAC );
    }

    return S_OK;
}
