//-----------------------------------------------------------------------------
// File: ModelFile.h
//
// Desc: Header file for the CModelFile class. 
//
// Copyright (c) Dan Chang. All rights reserved. 
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (c) Dan Chang"
//-----------------------------------------------------------------------------

#ifndef MODELFILE_H
#define MODELFILE_H

class IModelFile
{
public:
	IModelFile();
	virtual ~IModelFile();

    virtual HRESULT     Load(IDirect3DDevice9* pd3dDevice, const LPCWSTR szFilename) = 0;
    virtual void        Unload() = 0;

						// Render traversal for drawing objects
	virtual void		Render(IDirect3DDevice9* pd3dDevice, D3DXMATRIX matWorld) = 0;
};

#endif // #ifndef MODELFILE_H
