//-----------------------------------------------------------------------------
// File: ModelFiles.h
//
// Desc: Header file for the CModelFiles class. 
//
// Copyright (c) Dan Chang. All rights reserved. 
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code, for example:
// "Portions Copyright (c) Dan Chang"
//-----------------------------------------------------------------------------

#ifndef MODELFILES_H
#define MODELFILES_H

#include <map>
#include <string>
#include <vector>

#include "ModelFile.h"

typedef std::map<std::wstring, int> MapString_Int;
typedef std::pair<int, IModelFile*> PairInt_PIModelFile;

class CModelFiles
{
public:
	CModelFiles();
	~CModelFiles();

	int Load(IDirect3DDevice9* pd3dDevice, const LPCWSTR szModelFile);
	int GetInstance(const LPCWSTR szModelFile);
	IModelFile* PIModelFileFromIModelFile(int iModelFile);
	void Unload();

	IModelFile* PIModelFileFromName(const LPCWSTR szModelFile)
	{
		int const iModelFile = GetInstance(szModelFile);
		if (iModelFile < 0)
		{
			// Not found;
			return NULL;
		}
		return PIModelFileFromIModelFile(iModelFile);
	}

private:
	MapString_Int m_mapStringModelFileToIModelFile;
	std::vector<IModelFile*> m_vecModelFiles;
};

#endif // #ifndef MODELFILES_H
