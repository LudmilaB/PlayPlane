//-----------------------------------------------------------------------------
// File: ModelFiles.cpp
//
// Desc: CModelFiles holds a vector of IModelFile classes. A ModelNode then 
//       holds an index to that vector in CModelFiles. There can be several 
//       ModelNode classes refering to a single IModelFile class. 
//       (Currently CMeshFile and CWorldModelFile inherit from IModelFile, so 
//       this class supports both.) 
//
// Copyright (c) Dan Chang. All rights reserved. 
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (c) Dan Chang"
//-----------------------------------------------------------------------------

#include "DXUT.h"
#include "GameError.h"
#include "ModelFiles.h"
#include "MeshFile.h"
#include "WorldModelFile.h"

CModelFiles::CModelFiles() : 
	m_mapStringModelFileToIModelFile()
,	m_vecModelFiles()
{
}


void CModelFiles::Unload()
{
	std::vector<IModelFile*>::reverse_iterator r;
	for (r = m_vecModelFiles.rbegin(); r != m_vecModelFiles.rend(); ++r)
	{
		delete *r;
	}
	m_vecModelFiles.clear();
	m_mapStringModelFileToIModelFile.clear();
}


/*
	Returns index into m_vecModelFiles for the model that was loaded from szModelFile. 
	This function returns -1 if the model could not be found or successfully loaded. 
	If the model has already been loaded it won't load it again; instead, it simply 
	returns an index of the previously loaded model. 
 */
int CModelFiles::Load(IDirect3DDevice9* pd3dDevice, const LPCWSTR szModelFile)
{
	int iModel = GetInstance(szModelFile);
	if (iModel >= 0)
	{
		return iModel;
	}

	IModelFile* pModelFile = 0;
	const wchar_t *pc = wcsrchr(szModelFile, L'.');
	if (pc && wcscmp(pc, L".grd") == 0)
	{
		CWorldModelFile* pWorldModelFile = new CWorldModelFile();
		HRESULT hr = pWorldModelFile->Load(pd3dDevice, szModelFile);
		if (FAILED(hr))
		{
			delete pWorldModelFile;
			return -1;
		}
		pModelFile = pWorldModelFile;
	}
	else
	{
		CMeshFile* pMeshFile = new CMeshFile();
		HRESULT hr = pMeshFile->Load(pd3dDevice, szModelFile);
		if (FAILED(hr))
		{
			delete pMeshFile;
			return -1;
		}
		pModelFile = pMeshFile;
	}
	int i = m_vecModelFiles.size();
	m_mapStringModelFileToIModelFile[szModelFile] = i;
	m_vecModelFiles.push_back(pModelFile);
	return i;
}

/*
	Returns index into m_vecModelFiles for the model that was previously 
	loaded from szModelFile. This function returns -1 if the model could 
	not be found. 
 */
int CModelFiles::GetInstance(const LPCWSTR szModelFile)
{
	MapString_Int::iterator imap;

	imap = m_mapStringModelFileToIModelFile.find(szModelFile);
	if (imap != m_mapStringModelFileToIModelFile.end())
	{
		return imap->second;
	}

	return -1;
}

IModelFile* CModelFiles::PIModelFileFromIModelFile(int iModelFile)
{
	if (iModelFile >=0)
	{
		return m_vecModelFiles[iModelFile];
	}
	return 0;
}


CModelFiles::~CModelFiles()
{
	Unload();
}
