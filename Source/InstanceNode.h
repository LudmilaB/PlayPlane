#pragma once
#include "TransNode.h"

////////////////////////////////////////////////////////////////////////////

class CInstance;

class InstanceNode : public TransNode
{
public:
	InstanceNode(IDirect3DDevice9* pd3dDevice);
	~InstanceNode();

						// Update traversal for physics, AI, etc.
	virtual void		Update(double fTime);

						// Render traversal for drawing objects
	virtual void		Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld);

	void				SetInstance(CInstance* pInstance)	{ m_pInstance = pInstance; }

private:
    HRESULT             GetInstance(const LPCWSTR szFilename);

	CInstance*			m_pInstance;
	double				m_fLastTime;
	double				m_fTime;
};

////////////////////////////////////////////////////////////////////////////
