#pragma once
#include <DirectXMath.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
using namespace DirectX;





struct PositionNormalUV
{
	XMFLOAT3 m_position;
	XMFLOAT3 m_normalVec;
	XMFLOAT2 m_UVcoords;
};




class Mesh sealed
{
	std::vector<PositionNormalUV> UniqueVertexArray;
	std::vector<XMINT3> TrianglePointIndexes;
	
public:
	Mesh();
	void LoadMeshFromFile(std::string inFileName);
};

