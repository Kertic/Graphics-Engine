#include "pch.h"
#include "Mesh.h"


Mesh::Mesh()
{
	
}

void Mesh::LoadMeshFromFile(std::string inFileName)
{
	char testString[256];
	std::vector<XMFLOAT3> Vertexes;
	std::vector<XMFLOAT3> Normals;
	std::vector<XMFLOAT2> UVs;
	//First float is index of the vertex, second float is index of normal, third float is index of UV
	//Every 3 of these forms one triangle. Each one also creates a Vertex to check if it is unique or not
	std::vector<XMFLOAT3> PointIndexes;
	std::ifstream file(inFileName);



	/*
	test string
	vector of strings to hold vertexes
	vector of strings to hold normals
	vector of strings to hold UV

	while there are lines left to read
	{
		If that line begins with "v "
		split it into 4 sections split on the spaces 
		push a new XMFLOAT3 into Vertexes with (section 2, 3, 4) as the input

		If that line begins with "vn "
		split it into 4 sections split on the spaces
		push a new XMFLOAT3 into Normals with (section 2, 3, 4) as the input

		If that line begins with "vt " 
		split it into 3 sections based on the spaces between them
		push a new XMFLOAT2 into UVs with (section 2,3) as the input

		IF that line begins with "f " 
		find out how many terms there are where a "term" is a set of characters seperated by " "																	1    2     3     4   5
		Create an array with n-1 terms, starting with the 2nd term (to skip the f) called terms. (This is a char array of [n-1][5], where the [5] is formatted "vertIndex/normalIndex/UVindex")
			for(i = 2; i < terms.Length; ++i){
				Push a new XMFLOAT3 into PointIndexes with input (term[0][0], term[0][2], term[0][4])
				Push a new XMFLOAT3 into PointIndexes with input (term[i-1][0], term[i-1][2], term[i-1][4])
				Push a new XMFLOAT3 into PointIndexes with input (term[i][0], term[i][2], term[i][4])
			}

		
	}


	for(i = 0; i < PointIndexes.Length(); ++i){
		PositionNormalUV VertexToCheck;
		VertexToCheck.m_position = Vertexes[PointIndexes[i].x];
		VertexToCheck.m_normalVec = Normals[PointIndexes[i].y];
		VertexToCheck.m_UVcoords = UVs[PointIndexes[i].z];

		bool exists = false;
		indexOfInfo = UniqueVertex.size() - 1 ;//The lastmost Vertex in the array
		for(i = 0; i  < UniqueVertex.size(); ++i){
			if(Info == vertexArray[i]){
			exists = true;
			indexOfInfo = i;
			}
		}

		if(exists == false){
		Push VertexToCheck into UniqueVertex
		
		}

		push indexOfInfo into TrianglePointIndexes


	}
	
	*/
}
