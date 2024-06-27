#ifndef _ASSIMP_LOADER_H_
#define _ASSIMP_LOADER_H_

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"

class AssimpLoader
{
public:
	AssimpLoader();

	bool LoadFile(ID3D11Device*, HWND, std::string FileDirectoryPath);
	void Shutdown();
	void Render(ID3D11DeviceContext* pDeviceContext);
private:
	void processMesh(aiMesh*, const aiScene*, std::string, ID3D11Device* pDevice);

private:
	std::vector<Mesh> m_meshes;
};

#endif