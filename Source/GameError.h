//--------------------------------------------------------------------------------------
// Error reporting. 
//
// By Dan Chang. This code has been released into the public domain. 
//--------------------------------------------------------------------------------------

#pragma once
#ifndef ERROR_H
#define ERROR_H

void SetError(const LPCWSTR pszError);
void PrintfError(const LPCWSTR pszFormat, ...);
void ShowError();

#endif
