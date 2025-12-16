#pragma once
#include "EngineModule/RenderEngine.h"
#include "UtilityModule/StringLamda.h"
#include "UtilityModule/StringKey.h"
#include "AnimationModule/AnimateMesh.h"
#include "AnimationModule/IK/CCD_IKSolver.h"
#include <filesystem>
using namespace hyunwoo;
using KeyCode = hyunwoo::InputManager::KeyCode;

namespace hyunwoo {
	class RendererApp;
}


/*=========================================================================================================================================================
 *    사용자 정의 랜더 엔진을 정의한다...
 *==========*/
class hyunwoo::RendererApp final : public RenderEngine
{
	//==================================================================================
	////////							   Fields..								////////
	//==================================================================================
private:
	/*******************************************
	 *   Transform Component 관련 필드....
	 ******/
	Camera       m_cam;
	AnimateMesh  m_animateMesh;


	/*******************************************
	 *  Transform 약참조 관련 필드....
	 ******/
	WeakPtr<Transform> m_animateMesh_tr;
	WeakPtr<Transform> m_cam_tr;


	/******************************************
	 *  Control Transform 관련 필드...
	 *******/
	WStringKey		   m_control_name;
	WeakPtr<Transform> m_control_tr;


	/*******************************************
	 *  리소스 관련 필드....
	 ******/
	Mesh				   m_mesh;
	CCD_IKSolver           m_ikSolver;
	AnimationClip		   m_clip;
	std::vector<Material>  m_mats;
	std::vector<Texture2D> m_texs;


	/*******************************************
	 *  기타 필드...
	 *******/
	bool		 m_showBones = false;
	bool		 m_playAnim  = false;
	bool         m_useIK     = true;
	std::wstring m_meshName;
	std::wstring m_clipName;
	std::wstring m_debugLog;





	//======================================================================================
	////////						  Override methods..							////////
	//======================================================================================
protected:
	virtual void OnStart() override final
	{
		OnStart_InitRendererProperties();
		OnStart_CreateTransforms();

		m_meshName = L"-";
		m_clipName = L"-";
	}

	virtual void OnFileDropped(const wchar_t* filePath) override final
	{
		std::wstring extension = std::filesystem::path(filePath).extension().wstring();

		if (extension == L".pmx")	   OnFileDropped_LoadPmx(filePath);
		else if (extension == L".vmd") OnFileDropped_LoadVmd(filePath);
		else m_debugLog = (const wchar_t*)w$(L"'", extension.c_str(), L"' is not a supported file format.\nCurrently, only PMX and VMD formats are supported.");
	}

	virtual void OnEnterFrame(float deltaTime) override final
	{
		OnEnterFrame_ControlsAndRender(deltaTime);
	}




	//======================================================================================
	////////						  Private methods...							////////
	//======================================================================================
private:
	/*******************************************
	 *  OnStart()에서 호출될 메소드들...
	 ******/
	void OnStart_InitRendererProperties();
	void OnStart_CreateTransforms();


	/********************************************
	 *   OnFileDropped()에서 호출될 메소드들...
	 *******/
	void OnFileDropped_LoadPmx(const wchar_t* filePath);
	void OnFileDropped_LoadVmd(const wchar_t* filePath);


	/********************************************
	 *  OnEnterFrame()에서 호출될 메소드들...
	 *******/
	void OnEnterFrame_ControlsAndRender(float deltaTime);
	void OnEnterFrame_ShowBoneTransforms(const RenderMesh& renderMesh, WeakPtr<Transform>& control_tr, WStringKey& control_name);
};