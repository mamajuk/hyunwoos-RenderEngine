#include "RendererApp.h"
#include "ImportModule/PmxImporter.h"
#include "ImportModule/VmdImporter.h"
#include "UtilityModule/ByteStream.h"

/***********************************************************
 *   OnStart()에서 호출되는 메소드들....
 *******/

/*================================================================================================================================
 *   랜더러의 프로퍼티를 초기화한다....
 ***********/
void RendererApp::OnStart_InitRendererProperties()
{
	Renderer& renderer = GetRenderer();
	renderer.UseAutoClear	  = true;
	renderer.UseAlphaBlending = false;
	renderer.WireFrameColor   = Color::Black;
	renderer.ClearColor		  = Color::White;
	SetTargetFrameRate(60);
}


/*================================================================================================================================
 *  RenderMesh/Camera 등의 TransformComponent가 부착될 Transform들을 생성하고, TransformComponent들을 부착한다...
 ***********/
void RendererApp::OnStart_InitTransformComponents()
{
	m_animateMesh_tr = Transform::CreateTransform();
	m_animateMesh_tr->AttachTransformComponent(&m_animateMesh);
	m_control_name = L"animateMesh component";
	m_control_tr   = m_animateMesh_tr;

	m_cam_tr = Transform::CreateTransform();
	m_cam_tr->AttachTransformComponent(&m_cam);
	m_cam.Far = 200.f;

	GetViewPort().RenderCamera = &m_cam;
}














/***********************************************************
 *   OnFileDropped()에서 호출되는 메소드들....
 *******/

 /*================================================================================================================================
   *   인자로 주어진 확장명에 알맞는 Import 메소드를 호출한다....
   ***********/
void hyunwoo::RendererApp::OnFileDropped_Internal(const wchar_t* filePath)
{
	std::wstring extension = std::filesystem::path(filePath).extension().wstring();

	/******************************************
	 *  pmx 모델링 파일인가..?
	 ******/
	if (extension == L".pmx") {
		OnFileDropped_LoadPmx(filePath);
	}


	/******************************************
	 *  vmd 모델링 파일인가..?
	 ******/
	else if (extension == L".vmd") {
		OnFileDropped_LoadVmd(filePath);
	}


	/******************************************
	 *  지원하지 않는 파일인가..?
	 ******/
	else {
		m_debugLog = (const wchar_t*)w$(L"'", extension.c_str(), L"' is not a supported file format.\nCurrently, only PMX and VMD formats are supported.");
	}
}



/*================================================================================================================================
  *   드로그 앤 드랍된 파일이, pmx파일이라면 데이터를 읽어들이고 리소스를 갱신한다....
  ***********/
void RendererApp::OnFileDropped_LoadPmx(const wchar_t* filePath)
{
	TimePoint prev_time = HighClock::now();

	if (m_animateMesh.GetMesh().Get()!=nullptr) {
		m_animateMesh.SetMesh(nullptr);
		m_animateMesh.SetCurrentClip(nullptr);
	}

	m_meshName = L"-";
	m_clipName = L"-";
	m_animateMesh_tr->SetWorldPosition(Vector3::Zero);
	m_texs.clear();

	/*************************************************************************************
	 *  주어진 경로의 Pmx파일을 읽어들인다....
	 *******/
	PmxImporter::StorageDescription storage_desc;
	storage_desc.OutMesh        = &m_mesh;
	storage_desc.OutMaterials   = &m_mats;
	storage_desc.OutTextures    = &m_texs;
	storage_desc.OutCCDIKSolver = &m_ikSolver;


	PmxImporter::ImportResult pmx_ret;
	if ((pmx_ret = PmxImporter::Import(storage_desc, filePath)).Success==false) 
	{
		/*-----------------------------------------------------------
		 *  Import 실패 사유를 디버그 로그에 남기고, 함수를 종료한다....
		 *******/

		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 *  주어진 파일을 열 수가 없었음...
		 ******/
		if (pmx_ret.Failed_OpenFile) {
			m_debugLog = L"pmxImporter: failed_OpenFile..";
		}

		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 *  올바른 Pmx 파일이 아님....
		 ******/
		else if (pmx_ret.IsNotPmxFile) {
			m_debugLog = L"pmxImporter: isNotPmxFile..";
		}

		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 *  저장할 공간이 지정되지 않았음...
		 ******/
		else if (pmx_ret.LoadDataIsNothing) {
			m_debugLog = L"pmxImporter: load data is Nothing...";
		}

		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 *  해당 pmx에서 사용하는 텍스쳐들의
		 *  임포트가 실패함....
		 ******/
		else if (pmx_ret.TextureLoadResult.Success==false) 
		{
			m_debugLog = (const wchar_t*)w$(L"pmxImporter: Import failed for all textures except the ", pmx_ret.TextureLoadResult.ImportCount, L"...");

			//지정된 경로에 텍스쳐가 존재하지 않음...
			if (pmx_ret.TextureLoadResult.Failed_OpenFile) {
				m_debugLog += L"\n(Failed Open Png File)";
			}

			//올바르지 않은 텍스쳐 형식...
			else if (pmx_ret.TextureLoadResult.Invalid_File) {
				m_debugLog += L"\n(Invalid Png File)";
			}

			//Deflate 압축해제 실패...
			else if (pmx_ret.TextureLoadResult.Failed_Deflate) {
				m_debugLog += L"\n(Failed Png Deflate)";
			}

			//잘못된 필터 타입...
			else if (pmx_ret.TextureLoadResult.Invalid_FilterType) {
				m_debugLog += L"\n(Invalid Png FilterType)";
			}

			//잘못된 인터레이스 타입...
			else if (pmx_ret.TextureLoadResult.Invalid_Interlace) {
				m_debugLog += L"\n(Invalid Png Interlace)";
			}

			//올바른 Png 시그니쳐가 아님...
			else if (pmx_ret.TextureLoadResult.Invalid_Signature) {
				m_debugLog += L"\n(Invalid Png Signature)";
			}
		}
		return;
	}



	/********************************************************************
	 *   로드된 메시의 바운딩 박스를 재계산한다....
	 ******/
	m_mesh.RecalculateBoundingBox();
	m_animateMesh.SetMesh(&m_mesh);



	/*****************************************************************
	 *  머터리얼들을 초기화한다...
	 *******/
	std::vector<WeakPtr<Material>>& matList = m_animateMesh.GetMaterialList();
	matList.clear();

	for (uint32_t i = 0; i < m_mats.size(); i++) {
		Material& mat = m_mats[i];
		mat.Shaders.VertexShader   = Shader::VertexShader_MulFinalMat;
		mat.Shaders.FragmentShader = Shader::FragmentShader_Tex0Mapping;
		matList.push_back(WeakPtr<Material>(& m_mats[i]));
	}

	m_animateMesh_tr->SetLocalPosition(Vector3(0.f, -3.f, 14.f));
	m_meshName = std::filesystem::path(filePath).filename().c_str();
	m_debugLog = L"pmx file load success!!";

	float import_deltaTime = std::chrono::duration_cast<FloatDuration>((HighClock::now() - prev_time)).count();
	m_debugLog += (const wchar_t*)w$(L"(", import_deltaTime, L" seconds)");
}



/*================================================================================================================================
 *   드로그 앤 드랍된 파일이, vmd파일이라면 데이터를 읽어들이고 리소스를 갱신한다....
 ***********/
void RendererApp::OnFileDropped_LoadVmd(const wchar_t* filePath)
{
	TimePoint prev_time = HighClock::now();

	m_clipName = L"-";
	m_animateMesh.SetCurrentClip(nullptr);

	/*****************************************************************
	 *  주어진 경로의 Vmd파일을 읽어들인다....
	 *******/
	VmdImporter::ImportResult vmd_ret;
	if ((vmd_ret = VmdImporter::Import(m_clip, m_mesh, filePath)).Success==false) 
	{
		/*---------------------------------------
		 *  Import 실패 사유를 디버그 로그에 남기고,
		 *  함수를 종료한다....
		 *******/

		 //주어진 경로의 파일을 열 수가 없었음...
		if (vmd_ret.Failed_OpenFile) {
			m_debugLog = L"vmdImporter: failed_OpenFile..";
		}

		//해당 파일이 제대로 된 pmx파일이 아님...
		else if (vmd_ret.IsNotVmdFile) {
			m_debugLog = L"vmdImporter: isNotVmdFile..";
		}

		//인자로 주어진 PmxMesh가 유효하지 않음...
		else if (vmd_ret.InPmxMeshIsNotValid) {
			m_debugLog = L"vmdImporter: The mesh is not compatible with the specified VMD file...";
		}

		return;
	}

	m_animateMesh.SetCurrentClip(&m_clip);
	m_clipName = std::filesystem::path(filePath).filename().c_str();

	float import_deltaTime = std::chrono::duration_cast<FloatDuration>(HighClock::now() - prev_time).count();
	m_debugLog = (const wchar_t*)w$(L"vmd file load success!!(", import_deltaTime, L")");
}


















/***********************************************************
 *   OnEnterFrame()에서 호출되는 메소드들....
 *******/

 /*================================================================================================================================
  *   사용자 입력을 처리한다....
  ***********/
void RendererApp::OnEnterFrame_ControlsAndRender(float deltaTime)
{
	Renderer&			renderer = GetRenderer();
	const InputManager& input    = GetInputManager();

	/**************************************************************
	 *   각종 옵션들을 조작한다....
	 *******/

	 //카메라의 fov를 조절한다...
	m_cam.Fov += input.GetAxis(KeyCode::NUMPAD_6, KeyCode::NUMPAD_9);

	//와이어 프레임 모드를 사용하는가?
	if (input.WasPressedThisFrame(KeyCode::Num_1)) {
		renderer.UseWireFrameMode = !renderer.UseWireFrameMode;
	}

	//백페이스 컬링을 사용하는가?
	if (input.WasPressedThisFrame(KeyCode::Num_2)) {
		renderer.UseBackfaceCulling = !renderer.UseBackfaceCulling;
	}

	//IK를 사용하는가?
	if (input.WasPressedThisFrame(KeyCode::Num_3)) {
		m_useIK = !m_useIK;
	}

	//랜더메시를 조작 대상으로 선택한다...
	if (input.WasPressedThisFrame(KeyCode::Num_4)) {
		m_control_tr = m_animateMesh_tr;
		m_control_name = L"animateMesh component";
	}

	//카메라를 조작 대상으로 선택한다...
	if (input.WasPressedThisFrame(KeyCode::Num_5)) {
		m_control_tr = m_cam_tr;
		m_control_name = L"camera component";
	}

	//본을 표시한다...
	if (input.WasPressedThisFrame(KeyCode::Num_6)) {
		m_showBones = !m_showBones;
	}

	//애니메이션을 재생한다...
	if (input.WasPressedThisFrame(KeyCode::P)) {
		m_playAnim = !m_playAnim;
	}



	/**************************************************************
	 *   각종 옵션들을 조작한다....
	 *******/
	const float speedScale    = (input.IsInProgress(KeyCode::Space) ? .2f : 1.f);
	const float moveSpeedSec  = (100.f * speedScale * deltaTime);
	const float rotSpeedSec   = (200.f * speedScale * deltaTime);
	const float scaleSpeedSec = (1.f * speedScale * deltaTime);

	Transform* control_tr_raw = m_control_tr.Get();

	const Vector3 add_move = Vector3(
		input.GetAxis(KeyCode::Left, KeyCode::Right) * moveSpeedSec,
		input.GetAxis(KeyCode::Down, KeyCode::Up) * moveSpeedSec,
		input.GetAxis(KeyCode::NUMPAD_2, KeyCode::NUMPAD_8) * moveSpeedSec
	);

	const Quaternion add_rot = Quaternion::Euler(
		input.GetAxis(KeyCode::A, KeyCode::D) * rotSpeedSec,
		input.GetAxis(KeyCode::W, KeyCode::S) * rotSpeedSec,
		input.GetAxis(KeyCode::E, KeyCode::Q) * rotSpeedSec
	);

	const Vector3 add_scale = Vector3::One * input.GetAxis(KeyCode::F, KeyCode::R) * scaleSpeedSec;

	control_tr_raw->SetLocalPositionAndScaleAndRotation(
		(control_tr_raw->GetLocalPosition() + add_move),
		(control_tr_raw->GetLocalScale() + add_scale),
		(add_rot * control_tr_raw->GetLocalRotation())
	);

	if (input.WasPressedThisFrame(KeyCode::Del) && m_control_tr.GetUUID()!=m_animateMesh_tr.GetUUID()) {
		Transform::DestroyTransform(control_tr_raw);
		m_control_tr   = m_animateMesh_tr;
		m_control_name = L"animateMesh component";
	}



	/********************************************************************************
	 *   애니메이션 적용과 IK를 적용한 후, 뷰포트에 RenderMesh를 랜더링한다...
	 *******/
	if (m_playAnim) 
	{
		m_animateMesh.Update(scaleSpeedSec);

		if (m_useIK) {
			m_ikSolver.ResolveIK(m_animateMesh);
		}
	}

	renderer.DrawRenderMesh(m_animateMesh, GetViewPort());

	if (m_showBones) {
		OnEnterFrame_ShowBoneTransforms(m_animateMesh, m_control_tr, m_control_name);
	}




	/********************************************************************************
	 *   디버그 출력...
	 *******/
	static int   frameCount = 0;
	static int   lastFps    = 0;
	static float totalTime  = 0.f;

	frameCount++;
	if ((totalTime += deltaTime) >= 1.f) {
		lastFps = frameCount;
		frameCount = 0;
		totalTime = 0.f;
	}

	renderer.DrawTextField(w$(
		L"fps: ", lastFps,
		L"\n\n------※Controls※-----------------------------------",
		L"\n-play/stop animation(P): ", (int)m_playAnim,
		L"\n-use WireFrame Mode(1): ", (int)renderer.UseWireFrameMode,
		L"\n-use Backface Culling(2): ", (int)renderer.UseBackfaceCulling,
		L"\n-use ik(3): ", (int)m_useIK,
		L"\n-set control transform - animateMesh(4)",
		L"\n-set control transform - camera(5)",
		L"\n-show bone transforms(6): ", (int)m_showBones,
		L"\n(click the highlighted bone to manipulate it.)",
		L"\n\n-decrease or increase the camera FOV(NUMPAD 6 / NUMPAD 9): ", m_cam.Fov,
		L"\n-move along the control transform's X-axis(Left / Right)",
		L"\n-move along the control transform's Y-axis(Down / Up)",
		L"\n-move along the control transform's Z-axis(NUMPAD 2 / NUMPAD 8)",
		L"\n\n-rotate control transform's yaw(A / D)",
		L"\n-rotate control transform's pitch(S / W)",
		L"\n-rotate control transform's roll(Q / E)",
		L"\n●Press the Delete key to remove the currently manipulated transform."
		L"\n●Holding the Space bar slows down movement and animation playback."
		L"\n●Drag and drop Pmx or VMD files onto the screen to load them.",
		L"\n(imported mesh: ", m_meshName.c_str(), L")"
		L"\n(imported animation clip: ", m_clipName.c_str(), L")",
		L"\n\n--------※control transform info※----------------------",
		L"\n-name: ", m_control_name.GetView().data(),
		L"\n-local_pos: ", control_tr_raw->GetLocalPosition(),
		L"\n-local_rot: ", control_tr_raw->GetLocalRotation(),
		L"\n-local_scale: ", control_tr_raw->GetLocalScale(),
		L"\n\n-world_pos: ", control_tr_raw->GetWorldPosition(),
		L"\n-world_rot: ", control_tr_raw->GetWorldRotation(),
		L"\n-world_scale: ", control_tr_raw->GetWorldScale(),
		L"\n\n-----------※animation info※--------------------------------",
		L"\nnormalizedTime: ", m_animateMesh.GetNormalizedTime(),
		L"\n\n--------------※Debug Text※--------------------------------------",
		L"\n", m_debugLog.c_str()),
		Vector2Int::Zero,
		GetViewPort()
	);
}



void hyunwoo::RendererApp::OnEnterFrame_ShowBoneTransforms(const RenderMesh& renderMesh, WeakPtr<Transform>& control_tr, WStringKey& control_name)
{
	/********************************************************************
	 *   표시에 필요한 값들을 캐싱하고, 계산한다....
	 ********/
	Renderer& renderer		  = GetRenderer();
	const InputManager& input = GetInputManager();

	const float wHalf = 5.f;
	const float hHalf = 5.f;

	const Vector3 p1 = Vector3(-wHalf, -hHalf, 0.f);
	const Vector3 p2 = Vector3(wHalf, -hHalf, 0.f);
	const Vector3 p3 = Vector3(-wHalf, hHalf, 0.f);
	const Vector3 p4 = Vector3(wHalf, hHalf, 0.f);

	Mesh*   mesh  = renderMesh.GetMesh().Get();
	Camera* cam   = GetViewPort().RenderCamera.Get();

	if (mesh == nullptr || cam == nullptr) {
		return;
	}

	const Matrix4x4 V = cam->GetViewMatrix();
	const Matrix4x4 P = cam->GetPerspectiveMatrix(GetViewPort().RenderTarget.GetAspectRatio());
	const Matrix4x4 PV = (P * V);



	/********************************************************************
	 *   각 본들을 랜더링한다....
	 ********/
	const int32_t bone_count = mesh->Bones.size();

	for (int32_t boneIdx = 0; boneIdx < bone_count; boneIdx++) {

		const Bone& bone = mesh->Bones[boneIdx];
		Transform* bone_tr = renderMesh.GetBoneTransformAt(boneIdx).Get();

		if (bone_tr == nullptr) {
			continue;
		}

		const Vector3& bone_pos		  = bone_tr->GetWorldPosition();
		const Vector2  bone_ScreenPos = renderer.NDCToScreen(renderer.ClipToNDC(PV * Vector4(bone_pos, 1.f)), GetViewPort());

		const Vector2 sp1 = (bone_ScreenPos + p1);
		const Vector2 sp2 = (bone_ScreenPos + p2);
		const Vector2 sp3 = (bone_ScreenPos + p3);
		const Vector2 sp4 = (bone_ScreenPos + p4);

		Color rectColor = Color::Yellow;

		//부모 본이 존재할 경우에만 그린다...
		if (bone_tr->GetParent() != Transform::GetRoot())
		{
			Transform* parent_bone_tr = renderMesh.GetBoneTransformAt(bone.Parent_BoneIdx).Get();

			if (parent_bone_tr != nullptr) {
				const Vector3& parent_pos = parent_bone_tr->GetWorldPosition();
				const Vector2  parent_ScreenPos = renderer.NDCToScreen(renderer.ClipToNDC(PV * Vector4(parent_pos, 1.f)), GetViewPort());

				const Vector2 bone2parent = (bone_ScreenPos - parent_ScreenPos);
				const Vector2 bone2parent_Dir = bone2parent.GetNormalized();
				const Vector2 bone2parent_right = Vector2(-bone2parent_Dir.y, bone2parent_Dir.x);

				const Vector2 arrow1_pos = parent_ScreenPos + (bone2parent_right * 5.f);
				const Vector2 arrow2_pos = parent_ScreenPos - (bone2parent_right * 5.f);

				Color boneColor = Color::Red;
				renderer.DrawLine(boneColor, bone_ScreenPos, parent_ScreenPos, GetViewPort());
				renderer.DrawLine(boneColor, bone_ScreenPos, arrow1_pos, GetViewPort());
				renderer.DrawLine(boneColor, bone_ScreenPos, arrow2_pos, GetViewPort());
				renderer.DrawLine(boneColor, parent_ScreenPos, arrow1_pos, GetViewPort());
				renderer.DrawLine(boneColor, parent_ScreenPos, arrow2_pos, GetViewPort());

				rectColor = Color::Blue;
			}
		}


		if (input.WasPressedThisFrame(KeyCode::Left_Mouse)) {
			const Vector2 mpos = input.GetMouseScreenPosition();

			bool checkUp   = (mpos.x > sp1.x && mpos.x < sp2.x) && (mpos.y > sp1.y && mpos.y > sp2.y);
			bool checkDown = (mpos.x > sp3.x && mpos.x < sp4.x) && (mpos.y < sp3.y && mpos.y < sp4.y);

			//해당 본의 버튼을 클릭했는가?
			if (checkUp && checkDown) {
				control_tr = bone_tr;
				control_name = bone.Name;
			}
		}

		if (control_tr.GetUUID() == bone_tr->GetUUID()) {
			rectColor = Color::Green;
		}

		renderer.DrawLine(rectColor, sp1, sp2, GetViewPort());
		renderer.DrawLine(rectColor, sp1, sp3, GetViewPort());
		renderer.DrawLine(rectColor, sp2, sp4, GetViewPort());
		renderer.DrawLine(rectColor, sp3, sp4, GetViewPort());
	}
}
