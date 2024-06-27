#include "AssimpLoader.h"

AssimpLoader::AssimpLoader()
{

}

bool AssimpLoader::LoadFile(ID3D11Device* pDevice, HWND hwnd, std::string fileDirectory)
{
	bool result;

	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(fileDirectory,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded
	);

	if (pScene == NULL)
	{
		return false;
	}

	std::string frontpath = fileDirectory.substr(0, fileDirectory.find_last_of("\\"));

	int numMeshes = pScene->mNumMeshes;
	//모델 파일의 노드 탐색
	for (int i = 0; i < numMeshes; i++)
	{
		processMesh(pScene->mMeshes[i], pScene, frontpath, pDevice);
	}

	//메쉬마다 정점과 인덱스 버퍼를 생성
	for (Mesh& m : m_meshes)
	{
		result = m.InitializeBuffer(pDevice);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize Vertex Buffer", L"Error", MB_OK);
			return false;
		}
	}

	return true;
}

void AssimpLoader::processMesh(aiMesh* pNode, const aiScene* pScene, std::string filepath, ID3D11Device* pDevice)
{
	Mesh lMesh;
	
	for (int i = 0; i < pNode->mNumVertices; i++)
	{
		Vertex vt;

		static_cast<float>(vt.position.x = pNode->mVertices[i].x);
		static_cast<float>(vt.position.y = pNode->mVertices[i].y);
		static_cast<float>(vt.position.z = pNode->mVertices[i].z);

		if (pNode->HasTextureCoords(i))
		{
			static_cast<float>(vt.textureCoord.x = pNode->mTextureCoords[0][i].x);
			static_cast<float>(vt.textureCoord.y = pNode->mTextureCoords[0][i].y);
		}

		//배열에 추가
		lMesh.vertices.emplace_back(vt);

	}

	for (int i = 0; i < pNode->mNumFaces; i++)
	{
		aiFace face = pNode->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++)
			lMesh.indices.emplace_back(face.mIndices[j]);
	}

	//텍스처가 있는 경우  저장
	int materialCount = pNode->mMaterialIndex;
	if (materialCount > 0)
	{
		aiString tempstr;
		pScene->mMaterials[pNode->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &tempstr, NULL, NULL, NULL, NULL, NULL);

		std::string path = tempstr.C_Str();
		path = path.substr(path.find_last_of("\\") + 1, path.length());
		path = filepath +"\\" + path;

	
		lMesh.SetResource(pDevice, path);
	}

	m_meshes.emplace_back(lMesh);
	
	return;
}

void AssimpLoader::Render(ID3D11DeviceContext* pDeviceContext)
{
	//모델의 메쉬 하나씩 렌더링 함
	for (Mesh& m : m_meshes)
	{
		m.Render(pDeviceContext);
	}

	return;
}

void AssimpLoader::Shutdown()
{
	//구조체 배열안의 포인터는 직접 해제
	for (Mesh& m : m_meshes)
	{
		m.Shutdown();
	}

	m_meshes.clear();
}

