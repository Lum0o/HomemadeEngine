// header.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclure les en-têtes Windows rarement utilisés
// Fichiers d'en-tête Windows
#include <windows.h>
// Fichiers d'en-tête C RunTime
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// Release pointers functions
#define RELPTRDX(ptr) if (ptr) { ptr->Release(); ptr = nullptr; }
#define RELPTR(ptr) if (ptr) { delete ptr; ptr = nullptr; }

// DirectX Librairies
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// DirectX Header Files
#include <d3d12.h>
#include "../d3dx12.h"
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using namespace DirectX;
using namespace std;

