#ifndef _APPLICATION_CLASS_H_
#define _APPLICATION_CLASS_H_

#include <Windows.h>
#include "D3DClass.h"
#include "ColorShaderClass.h"
#include "TextureShaderClass.h"
#include "CameraClass.h"
#include "AssimpLoader.h"
#include "InfoUiClass.h"
#include "TextClass.h"
#include "TriangleModel.h"

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, bool V_SYNC, bool FullScreen, float ScreenDepth, float ScreenNear);
	void Shutdown();
	bool Frame();

private:
	D3DClass* m_Direct3D;
	ColorShaderClass* m_ColorShader;
	CameraClass* m_CameraClass;
	InfoUiClass* m_InfoUi;
	TextClass* m_TextClass;
	TextureShaderClass* m_ShaderClass;
	AssimpLoader* m_AssimpLoader;
	TriangleModel* m_Triangle;
	XMMATRIX model1_world;
	XMMATRIX model2_world;
};
#endif