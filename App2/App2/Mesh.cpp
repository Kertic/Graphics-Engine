#include "pch.h"
#include "Mesh.h"


Mesh::Mesh()
{

}

void Mesh::LoadMeshFromFile(std::string inFileName)
{
	std::string testString;
	std::vector<XMFLOAT3> Vertexes;
	std::vector<XMFLOAT3> Normals;
	std::vector<XMFLOAT2> UVs;
	//First float is index of the vertex, second float is index of normal, third float is index of UV
	//Every 3 of these forms one triangle. Each one also creates a Vertex to check if it is unique or not
	std::vector<XMINT3> PointIndexes;
	std::ifstream file(inFileName);
	if (file.is_open()) {
		while (std::getline(file, testString))
		{
			if (testString.size() > 2) {//This makes sure we dont check indexes when we have just a # or similar symbol

#pragma region Vertex
				if (testString.c_str()[0] == 'v' && testString.c_str()[1] == ' ') {
					std::string Vchar;
					std::string Xfloat;
					std::string Yfloat;
					std::string Zfloat;

					unsigned int termCount = 0;
					for (unsigned int i = 0; i < testString.length(); ++i) {
						if (termCount == 0)
							Vchar.append(1, testString[i]);

						if (termCount == 1)
							Xfloat.append(1, testString[i]);

						if (termCount == 2)
							Yfloat.append(1, testString[i]);

						if (termCount == 3)
							Zfloat.append(1, testString[i]);

						if (testString[i] == ' ')
							++termCount;
					}

					Vertexes.push_back(XMFLOAT3(stof(Xfloat), stof(Yfloat), stof(Zfloat)));
				}
#pragma endregion
#pragma region Normal
				if (testString.c_str()[0] == 'v' && testString.c_str()[1] == 'n' && testString.c_str()[2] == ' ') {

					std::string Vchar;
					std::string Xfloat;
					std::string Yfloat;
					std::string Zfloat;

					unsigned int termCount = 0;
					for (unsigned int i = 0; i < testString.length(); ++i) {
						if (termCount == 0)
							Vchar.append(1, testString[i]);

						if (termCount == 1)
							Xfloat.append(1, testString[i]);

						if (termCount == 2)
							Yfloat.append(1, testString[i]);

						if (termCount == 3)
							Zfloat.append(1, testString[i]);

						if (testString[i] == ' ')
							++termCount;
					}

					Normals.push_back(XMFLOAT3(stof(Xfloat), stof(Yfloat), stof(Zfloat)));
				}
#pragma endregion
#pragma region UV / Texture
				if (testString.c_str()[0] == 'v' && testString.c_str()[1] == 't' && testString.c_str()[2] == ' ') {

					std::string Vchar;
					std::string Xfloat;
					std::string Yfloat;


					unsigned int termCount = 0;
					for (unsigned int i = 0; i < testString.length(); ++i) {
						if (termCount == 0)
							Vchar.append(1, testString[i]);

						if (termCount == 1)
							Xfloat.append(1, testString[i]);

						if (termCount == 2)
							Yfloat.append(1, testString[i]);

						if (testString[i] == ' ')
							++termCount;
					}

					UVs.push_back(XMFLOAT2(stof(Xfloat), stof(Yfloat)));
				}
#pragma endregion
#pragma region Faces / Indexes
				if (testString.c_str()[0] == 'f' && testString.c_str()[1] == ' ') {
					std::vector<std::string> Terms;

					unsigned int currentTerm = 0;
					Terms.push_back(std::string());

					//Create a string for each "term" on the line
					for (unsigned int i = 0; i < testString.length(); ++i) {
						Terms[currentTerm].append(1, testString[i]);

						if (testString[i] == ' ') {
							++currentTerm;
							Terms.push_back(std::string());
						}
					}
					//Erase the "f" terms so we're left with just the numbers
					Terms.erase(Terms.begin());


					for (unsigned int i = 2; i < Terms.size(); ++i) {

						

						std::string vertexIndexString;
						std::string normalIndexString;
						std::string UVindexString;
#pragma region Point i



						for (unsigned int j = 0, currentIndex = 0; j < Terms[i].size(); ++j)
						{
							if (Terms[i][j] != '/')
							{
								if (currentIndex == 0)
									vertexIndexString.append(1, Terms[i][j]);
								if (currentIndex == 1)
									UVindexString.append(1, Terms[i][j]);
								if (currentIndex == 2)
									normalIndexString.append(1, Terms[i][j]);
							}
							else
							{
								++currentIndex;
							}
						}


						//Push a new XMFLOAT3 into PointIndexes with input (term[i][0], term[i][2], term[i][4]) //Push the i's index's x,y, and z 
						PointIndexes.push_back(XMINT3(std::stoi(vertexIndexString), std::stoi(UVindexString), std::stoi(normalIndexString)));
						vertexIndexString.clear();
						normalIndexString.clear();
						UVindexString.clear();

#pragma endregion
#pragma region Point i-1



						for (unsigned int j = 0, currentIndex = 0; j < Terms[i - 1].size(); ++j)
						{
							if (Terms[i - 1][j] != '/')
							{
								if (currentIndex == 0)
									vertexIndexString.append(1, Terms[i - 1][j]);
								if (currentIndex == 1)
									UVindexString.append(1, Terms[i - 1][j]);
								if (currentIndex == 2)
									normalIndexString.append(1, Terms[i - 1][j]);
							}
							else
							{
								++currentIndex;
							}
						}


						//Push a new XMFLOAT3 into PointIndexes with input (term[i-1][0], term[i-1][2], term[i-1][4]) //Push the i-1's index's x,y, and z
						PointIndexes.push_back(XMINT3(std::stoi(vertexIndexString), std::stoi(UVindexString), std::stoi(normalIndexString)));
						vertexIndexString.clear();
						normalIndexString.clear();
						UVindexString.clear();

#pragma endregion
#pragma region Point 0



						for (unsigned int j = 0, currentIndex = 0; j < Terms[0].size(); ++j)
						{
							if (Terms[0][j] != '/') 
							{
								if (currentIndex == 0)
									vertexIndexString.append(1, Terms[0][j]);
								if (currentIndex == 1)
									UVindexString.append(1, Terms[0][j]);
								if (currentIndex == 2)
									normalIndexString.append(1, Terms[0][j]);
							}
							else 
							{
								++currentIndex;
							}
						}
						//Push a new XMFLOAT3 into PointIndexes with input (term[0][0], term[0][2], term[0][4]) //Push the first index's x,y,and z which translate to a vertex index, normal index, and UV index

						PointIndexes.push_back(XMINT3(std::stoi(vertexIndexString),  std::stoi(UVindexString), std::stoi(normalIndexString)));
						vertexIndexString.clear();
						normalIndexString.clear();
						UVindexString.clear();
#pragma endregion


					}

				}
#pragma endregion

			}
		}
		file.close();
	}

	//Create the unique vertex list and corresponding indicies
	for (unsigned int i = 0; i < PointIndexes.size(); i++)
	{
		PositionNormalUV VertexToCheck;
		VertexToCheck.m_position = Vertexes[PointIndexes[i].x - 1];
		VertexToCheck.m_UVcoords = UVs[PointIndexes[i].y - 1];
		VertexToCheck.m_normalVec = Normals[PointIndexes[i].z - 1];

		bool exists = false;

		unsigned int indexOfUniqueVertex = 0;

		for (unsigned int j = 0; j < UniqueVertexArray.size(); j++)
		{
			if (VertexToCheck == UniqueVertexArray[j]) {
				exists = true;
				indexOfUniqueVertex = j;
			}
		}

		if (!exists) {//If it doesnt exist, we make it
			UniqueVertexArray.push_back(VertexToCheck);//Put the vertex in the unique list
			indexOfUniqueVertex = UniqueVertexArray.size() - 1;//Mark its index in our variable
		}

		TrianglePointIndexes.push_back(indexOfUniqueVertex);

	}
	TrianglePointIndexes.shrink_to_fit();
	UniqueVertexArray.shrink_to_fit();
	
#pragma region Pseudocode for loading file info
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
		Create an array with n-1 terms, starting with the 2nd term (to skip the f) called terms. (This is a char array of [n-1][x], where the [x] is formatted "vertIndex/normalIndex/UVindex")
			for(i = 2; i < terms.Length; ++i){

			unsigned int vertexIndex = 0;
			unsigned int normalIndex = 0;
			unsigned int UVindex = 0;


			for(unsigned int j = 0, unsigned int currentIndex = 0, string vertexIndexString, string normalIndexString, string UVindexString; j<term[0].length; ++j){
				if(term[0][j] != '/'){
					if(currentIndex == 0)
						vertexIndexString.append(term[0][j]);
					if(currentIndex == 1)
						normalIndexString.append(term[0][j]);
					if(currentIndex == 2)
						UVindexString.append(term[0][j]);
				}
				else{
				++currentTerm;
				}
			}



				//Push a new XMFLOAT3 into PointIndexes with input (term[0][0], term[0][2], term[0][4]) //Push the first index's x,y,and z which translate to a vertex index, normal index, and UV index
				//Push a new XMFLOAT3 into PointIndexes with input (term[i-1][0], term[i-1][2], term[i-1][4]) //Push the i-1's index's x,y, and z
				//Push a new XMFLOAT3 into PointIndexes with input (term[i][0], term[i][2], term[i][4]) //Push the i's index's x,y, and z
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
#pragma endregion


}

bool PositionNormalUV::operator==(const PositionNormalUV & testSubject)
{
#pragma region Compare Position
	if (this->m_position.x != testSubject.m_position.x)
		return false;
	if (this->m_position.y != testSubject.m_position.y)
		return false;
	if (this->m_position.z != testSubject.m_position.z)
		return false;
#pragma endregion
#pragma region Compare Normals
	if (this->m_normalVec.x != testSubject.m_normalVec.x)
		return false;
	if (this->m_normalVec.y != testSubject.m_normalVec.y)
		return false;
	if (this->m_normalVec.z != testSubject.m_normalVec.z)
		return false;
#pragma endregion
#pragma region Compare UV's'
	if (this->m_UVcoords.x != testSubject.m_UVcoords.x)
		return false;
	if (this->m_UVcoords.y != testSubject.m_UVcoords.y)
		return false;
#pragma endregion
	return true;



	

}
