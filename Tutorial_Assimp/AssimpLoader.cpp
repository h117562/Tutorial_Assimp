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
		//에러 내용을 멀티바이트 문자열로 변환하여 메시지 박스에 출력하는 기능
		std::string errorMessage = importer.GetErrorString();
		int strlen = errorMessage.size();
		wchar_t* convertMessage = new wchar_t[strlen + 1];

		//멀티바이트로 변환
		MultiByteToWideChar(CP_UTF8, 0, errorMessage.c_str(), -1, convertMessage, strlen + 1);

		MessageBox(hwnd, convertMessage, L"Error", MB_OK);

		return false;
	}

	//파일 위치의 마지막 \\ 에서 파일 이름만 제외
	std::string frontpath = fileDirectory.substr(0, fileDirectory.find_last_of("\\"));

	int numMeshes = pScene->mNumMeshes;
	//모델 파일의 메쉬 개수만큼 탐색
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

void AssimpLoader::processMesh(aiMesh* pMesh, const aiScene* pScene, std::string filepath, ID3D11Device* pDevice)
{
	Mesh lMesh;
	
	for (int i = 0; i < pMesh->mNumVertices; i++)
	{
		Vertex vt;

		//정점 좌표 저장
		vt.position.x = static_cast<float>(pMesh->mVertices[i].x);
		vt.position.y = static_cast<float>(pMesh->mVertices[i].y);
		vt.position.z = static_cast<float>(pMesh->mVertices[i].z);

		//텍스처 좌표 저장
		if (pMesh->mTextureCoords[0])
		{
			vt.textureCoord.x = static_cast<float>(pMesh->mTextureCoords[0][i].x);
			vt.textureCoord.y = static_cast<float>(pMesh->mTextureCoords[0][i].y);
		}

		//배열에 추가
		lMesh.vertices.emplace_back(vt);

	}

	//인덱스 저장
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

