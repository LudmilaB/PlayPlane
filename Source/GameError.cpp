//--------------------------------------------------------------------------------------
// Error reporting. 
//
// By Dan Chang. This code has been released into the public domain. 
//--------------------------------------------------------------------------------------

#include "DXUT.h"

#include <strsafe.h>

#include "GameError.h"

wchar_t szError[1024];

void SetError(const LPCWSTR pszError)
{
	wcscpy_s(szError, pszError);
}

void PrintfError(const LPCWSTR pszFormat, ...)
{
	va_list args;
	va_start(args, pszFormat);
	vswprintf_s(szError, pszFormat, args);
	va_end( args );
}

void ShowError()
{
	MessageBox( DXUTGetHWND(), szError, DXUTGetWindowTitle(), MB_ICONERROR|MB_OK );
}
