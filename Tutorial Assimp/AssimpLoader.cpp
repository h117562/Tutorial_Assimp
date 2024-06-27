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
	//�� ������ ��� Ž��
	for (int i = 0; i < numMeshes; i++)
	{
		processMesh(pScene->mMeshes[i], pScene, frontpath, pDevice);
	}

	//�޽����� ������ �ε��� ���۸� ����
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

		//�迭�� �߰�
		lMesh.vertices.emplace_back(vt);

	}

	for (int i = 0; i < pNode->mNumFaces; i++)
	{
		aiFace face = pNode->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++)
			lMesh.indices.emplace_back(face.mIndices[j]);
	}

	//�ؽ�ó�� �ִ� ���  ����
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
	//���� �޽� �ϳ��� ������ ��
	for (Mesh& m : m_meshes)
	{
		m.Render(pDeviceContext);
	}

	return;
}

void AssimpLoader::Shutdown()
{
	//����ü �迭���� �����ʹ� ���� ����
	for (Mesh& m : m_meshes)
	{
		m.Shutdown();
	}

	m_meshes.clear();
}

