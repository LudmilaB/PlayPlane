//-----------------------------------------------------------------------------
// File: Tiny.h
//
// Desc: Header file for the CTiny class.  Its declaraction is found here.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------


#ifndef TINY_H
#define TINY_H

#include "DXUT/SDKsound.h"
#include "Instance.h"

#define IDLE_TRANSITION_TIME 0.125f
#define MOVE_TRANSITION_TIME 0.25f


typedef std::basic_string <TCHAR>   String;




#define ANIMINDEX_FAIL      0xffffffff
#define FOOTFALLSOUND00     L"Media\\Tiny\\footfall00.wav"
#define FOOTFALLSOUND01     L"Media\\Tiny\\footfall01.wav"


extern CSound*                      g_apSoundsTiny[ 2 ];


const float fCollRadiusNPC = 0.20f;		// the collision radius used to represent the NPC 

// Callback data for pass to the callback handler
struct CallbackDataTiny
{
    DWORD m_dwFoot;       // Identify which foot caused the callback
    const D3DXVECTOR3* m_pvCameraPos;  // Camera position when callback happened
    const D3DXVECTOR3* m_pvTinyPos;    // Tiny's position when callback happened
};




//-----------------------------------------------------------------------------
// Name: class CBHandlerTiny
// Desc: Derived from ID3DXAnimationCallbackHandler.  Callback handler for
//       CTiny -- plays the footstep sounds
//-----------------------------------------------------------------------------
class CBHandlerTiny :
public ID3DXAnimationCallbackHandler
{
    HRESULT CALLBACK HandleCallback( THIS_ UINT Track, LPVOID pCallbackData )
    {
        CallbackDataTiny* pCD = ( CallbackDataTiny* )pCallbackData;

        // this is set to NULL if we're not playing sounds
        if( /*fornow*/ !pCD || !pCD->m_pvCameraPos )
            return S_OK;

        // scale volume by distance from tiny
        D3DXVECTOR3 vDiff;
        D3DXVec3Subtract( &vDiff, pCD->m_pvCameraPos, pCD->m_pvTinyPos );
        float fVolume = min( D3DXVec3LengthSq( &vDiff ) * 0.2f, 1.f );
        fVolume *= -3000.f;

        // play the sound
        if( pCD && g_apSoundsTiny[ pCD->m_dwFoot ] )
            g_apSoundsTiny[ pCD->m_dwFoot ]->Play( 0, 0, ( LONG )fVolume );

        return S_OK;
    }
};


class CMultiAnim;
class CAnimInstance;
class GameObject;


//-----------------------------------------------------------------------------
// Name: class CTiny
// Desc: This is the character class. It handles character behaviors and the
//       the associated animations.
//-----------------------------------------------------------------------------
class CTiny : public CInstance
{

protected:

    // -- data structuring
    CMultiAnim* m_pMA;               // pointer to mesh-type-specific object
    DWORD m_dwMultiAnimIdx;    // index identifying which CAnimInstance this object uses
    CAnimInstance* m_pAI;               // pointer to CAnimInstance specific to this object
    std::vector <CTiny*>* m_pv_pChars;         // pointer to global array of CTiny* s
    CSoundManager* m_pSM;               // pointer to sound management interface
    DWORD m_dwAnimIdxLoiter,   // Indexes of various animation sets
        m_dwAnimIdxWalk,
        m_dwAnimIdxJog;
    CallbackDataTiny    m_CallbackData[ 2 ]; // Data to pass to callback handler

    // operational status
    double m_dTimePrev;         // global time value before last update
    double m_dTimeCurrent;      // current global time value
    bool m_bPlaySounds;       // true == this instance is playing sounds
    DWORD m_dwCurrentTrack;    // current animation track for primary animation

    // character traits
    float m_fSpeedTurn;        // character's turning speed -- in radians/second
    ID3DXAnimationCallbackHandler*
        m_pCallbackHandler;  // pointer to callback inteface to handle callback keys
    D3DXMATRIX m_mxOrientation;     // transform that gets the mesh into a common world space
    float m_fPersonalRadius;   // personal space radius -- things can't get closer than this
    // (note that no height information is given--not necessary for this sample)
    // character status
    float m_fFacing;           // current direction the character is facing -- in our sample, it's 2D only
    D3DXVECTOR3 m_vPosTarget;        // This indicates where we are moving to
    float m_fFacingTarget;     // The direction from Tiny's current position to the final destination
    bool m_bIdle;             // set when Tiny is idling -- not turning toward a target
    bool m_bWaiting;          // set when Tiny is not idle, but cannot move
    double m_dTimeIdling;       // countdown - Tiny Idles til this goes < 0
    double m_dTimeWaiting;      // countdown - Tiny is waiting til this goes < 0, then picks a new target
    double m_dTimeBlocked;      // countdown - Tiny must wait a small time when encountering a blocker before starting to walk again

    char                m_szASName[64];      // Current track's animation set name (for preserving across device reset)

public:

                        CTiny( GameObject& owner );
    virtual             ~CTiny();
    virtual HRESULT     Setup( CMultiAnim* pMA, std::vector <CTiny*>* pv_pChars, CSoundManager* pSM,
                               double dTimeCurrent );
    virtual void        Cleanup();
    virtual CAnimInstance* GetAnimInstance();
    void                GetPosition( D3DXVECTOR3* pV );
    void                GetFacing( D3DXVECTOR3* pV );
    virtual void        Animate( double dTimeDelta );

	// FIXME: Bit of a kludge: GetInstance() and Unload() aren't implemented for CTiny 
	// yet, because this class needs to load additional items. . . 
    HRESULT             GetInstance(IDirect3DDevice9* pd3dDevice, const LPCWSTR szFilename);
    void                Unload();

    HRESULT             ResetTime();
    HRESULT             AdvanceTime( double dTimeDelta, const D3DXVECTOR3* pvEye );
    HRESULT             Draw( IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld );
    virtual void        Report( std::vector <String>& v_sReport );
    virtual void        SetSounds( bool bSounds );

    void                SmoothLoiter();
    void                SetMoveKey();
    void                SetIdleKey( bool bResetPosition );
    double              GetSpeedScale();

    void                SetOrientation();

    HRESULT             RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT             InvalidateDeviceObjects();

protected:      // ************ The following are not callable by the app -- internal stuff

    DWORD               GetAnimIndex( char sString[] );
    HRESULT             AddCallbackKeysAndCompress( LPD3DXANIMATIONCONTROLLER pAC, LPD3DXKEYFRAMEDANIMATIONSET pAS,
                                                    DWORD dwNumCallbackKeys, D3DXKEY_CALLBACK aKeys[],
                                                    DWORD dwCompressionFlags, FLOAT fCompression );
    HRESULT             SetupCallbacksAndCompression();
};



#endif // #ifndef TINY_H
