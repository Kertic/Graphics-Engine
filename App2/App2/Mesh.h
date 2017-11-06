#pragma once
#include <DirectXMath.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
using namespace DirectX;





struct  PositionNormalUV
{
	XMFLOAT3 m_position;
	XMFLOAT3 m_normalVec;
	XMFLOAT2 m_UVcoords;

	bool operator ==(const PositionNormalUV &testSubject);
};




class Mesh 
{

	
public:
	std::vector<PositionNormalUV> UniqueVertexArray;
	std::vector<unsigned int> TrianglePointIndexes;
	Mesh();
	Mesh(std::string fileName);
	void LoadMeshFromFile(std::string inFileName);
};

