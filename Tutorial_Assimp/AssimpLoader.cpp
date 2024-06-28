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
		//���� ������ ��Ƽ����Ʈ ���ڿ��� ��ȯ�Ͽ� �޽��� �ڽ��� ����ϴ� ���
		std::string errorMessage = importer.GetErrorString();
		int strlen = errorMessage.size();
		wchar_t* convertMessage = new wchar_t[strlen + 1];

		//��Ƽ����Ʈ�� ��ȯ
		MultiByteToWideChar(CP_UTF8, 0, errorMessage.c_str(), -1, convertMessage, strlen + 1);

		MessageBox(hwnd, convertMessage, L"Error", MB_OK);

		return false;
	}

	//���� ��ġ�� ������ \\ ���� ���� �̸��� ����
	std::string frontpath = fileDirectory.substr(0, fileDirectory.find_last_of("\\"));

	int numMeshes = pScene->mNumMeshes;
	//�� ������ �޽� ������ŭ Ž��
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

void AssimpLoader::processMesh(aiMesh* pMesh, const aiScene* pScene, std::string filepath, ID3D11Device* pDevice)
{
	Mesh lMesh;
	
	for (int i = 0; i < pMesh->mNumVertices; i++)
	{
		Vertex vt;

		//���� ��ǥ ����
		vt.position.x = static_cast<float>(pMesh->mVertices[i].x);
		vt.position.y = static_cast<float>(pMesh->mVertices[i].y);
		vt.position.z = static_cast<float>(pMesh->mVertices[i].z);

		//�ؽ�ó ��ǥ ����
		if (pMesh->mTextureCoords[0])
		{
			vt.textureCoord.x = static_cast<float>(pMesh->mTextureCoords[0][i].x);
			vt.textureCoord.y = static_cast<float>(pMesh->mTextureCoords[0][i].y);
		}

		//�迭�� �߰�
		lMesh.vertices.emplace_back(vt);

	}

	//�ε��� ����
	for (int i = 0; i < pMesh->mNumFaces; i++)
	{
		aiFace face = pMesh->mFaces[i];

		for (int j = 0; j < face.mNumIndices; j++)
			lMesh.indices.emplace_back(face.mIndices[j]);
	}

	{
		aiString tempstr;
		pScene->mMaterials[pMesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &tempstr, NULL, NULL, NULL, NULL, NULL);

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

