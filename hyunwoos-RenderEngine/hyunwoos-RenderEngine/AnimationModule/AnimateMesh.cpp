#include "AnimateMesh.h"


/*============================================================================================================================================
 *   Super 클래스인 RenderMesh에서 BoneTransform 목록의 갱신이 발생했을 때, AnimateMesh의 Property들의 룩업테이블을 갱신합니다...
 *********/
void hyunwoo::AnimateMesh::OnUpdateBoneTransforms()
{
	/*******************************************************************
	 *   Mesh가 유효하지 않거나, 혹은 본이 존재하지 않으면 함수를 종료한다..
	 **********/
	Mesh* mesh = GetMesh().Get();

	if (mesh==nullptr || mesh->Bones.size()==0) {
		return;
	}


	/*************************************************************
	 *   AnimationClip이 유효하면, 해당 AnimationClip의 Property
	 *   들이 수정해야할 Bone의 Idx를 찾아 캐싱한다....
	 *********/
	AnimationClip*	   clip		 = m_cur_clip.Get();
	std::vector<Bone>& bone_list = mesh->Bones;

	if (clip!=nullptr) 
	{
		m_prop2BoneIdx.resize(clip->Properties.size());

		for (uint32_t i = 0; i < m_prop2BoneIdx.size(); i++) {
			const AnimationClip::Property& prop = clip->Properties[i];

			/*------------------------------------------
			 *   본 이름이 일치하면, 해당 본의 인덱스를
			 *   삽입한다....
			 ******/
			m_prop2BoneIdx[i] = m_invalid_idx;

			for (uint32_t j = 0; j < bone_list.size(); j++) 
			{
				Bone& bone = bone_list[j];
				if (bone.Name==prop.Name) {
					m_prop2BoneIdx[i] = j;
					break;
				}
			}
		}
	}

}











/*============================================================================================================================================
 *   주어진 deltaTime만큼, 애니메이션을 재생합니다....
 *********/
void hyunwoo::AnimateMesh::Update(float deltaTime)
{
	/**************************************************************************************
	 *  애니메이션 재생에 필요한 리소스들이 유효하지 않다면, 함수를 종료한다..
	 *******/
	Mesh*		   mesh	= GetMesh().Get();
	AnimationClip* clip = m_cur_clip.Get();

	if (clip==nullptr || mesh==nullptr || m_prop2BoneIdx.size()!=clip->Properties.size()) {
		return;
	}



	/****************************************************************************************
	 *  AnimationClip의 모든 Property들을 순회하여, 각 Property에 대응되는 Bone Transform을
	 *  갱신한다....
	 *******/
	const uint32_t property_count = clip->Properties.size();
	m_time += deltaTime;

	if (m_time > clip->TotalTime) {
		m_time = (m_time - clip->TotalTime);
	}

	for (uint32_t i = 0; i < property_count; i++) 
	{
		const AnimationClip::Property& prop = clip->Properties[i];

		/*------------------------------------------------
		 *  해당 Property에 대응되는 Bone이 없다면 넘어간다..
		 ********/
		if (m_prop2BoneIdx[i]==m_invalid_idx) {
			continue;
		}


		/*------------------------------------------------
		 *  해당 Property에 대응되는 Bone Transform이
		 *  유효하지 않다면 넘어간다...
		 ********/
		Transform* bone_tr = GetBoneTransformAt(m_prop2BoneIdx[i]).Get();
		if (bone_tr==nullptr) {
			continue;
		}


		/*------------------------------------------------
		 *  주어진 시간값이 포함되는 두 키프레임 사이에서,
		 *  주어진 시간값의 정규화된 시간값을 구한다....
		 *******/
		const Bone&  bone      = mesh->Bones[m_prop2BoneIdx[i]];
		KeyFramePair pair      = GetKeyFramePairByTime(prop.KeyFrames, m_time);

		const AnimationClip::KeyFrame& prev = prop.KeyFrames[pair.left];
		const AnimationClip::KeyFrame& nxt  = prop.KeyFrames[pair.right];

		const float prev_to_nxt_dst = (nxt.Time - prev.Time);
		const float normalizedTime  = ((m_time - prev.Time) * (prev_to_nxt_dst==0.f? 0.f:(1.f/prev_to_nxt_dst)));


		/*-----------------------------------------------
		 *  해당 프로퍼티의 타입에 알맞게 적절히 보간한다...
		 *******/
		switch (prop.Type)
		{
			//LocalPosition인 경우.....
			case(AnimationClip::PropertyType::Local_Position): {
				Vector3 new_localPos = prev.Vec3;
				new_localPos.x += (nxt.Vec3.x - prev.Vec3.x) * GetCurveY(prop.Curves[prev.CurveStartIdx], normalizedTime);
				new_localPos.y += (nxt.Vec3.y - prev.Vec3.y) * GetCurveY(prop.Curves[prev.CurveStartIdx+1], normalizedTime);
				new_localPos.z += (nxt.Vec3.z - prev.Vec3.z) * GetCurveY(prop.Curves[prev.CurveStartIdx+2], normalizedTime);

				bone_tr->SetLocalPosition(new_localPos);
				break;
			}


			//LocalScale인 경우....
			case(AnimationClip::PropertyType::Local_Scale): {
				Vector3 new_localScale = prev.Vec3;
				new_localScale.x += (nxt.Vec3.x - prev.Vec3.x) * GetCurveY(prop.Curves[prev.CurveStartIdx], normalizedTime);
				new_localScale.y += (nxt.Vec3.y - prev.Vec3.y) * GetCurveY(prop.Curves[prev.CurveStartIdx+1], normalizedTime);
				new_localScale.z += (nxt.Vec3.z - prev.Vec3.z) * GetCurveY(prop.Curves[prev.CurveStartIdx+2], normalizedTime);

				bone_tr->SetLocalScale(new_localScale);
				break;
			}


			//LocalRotation Quaternion인 경우....
			case(AnimationClip::PropertyType::Local_Rotation_Quat): {
				const float t			  = GetCurveY(prop.Curves[prev.CurveStartIdx], normalizedTime);
				Quaternion  new_local_rot = (prev.Quat * (1.f - t)) + (nxt.Quat * t);

				bone_tr->SetLocalRotation(new_local_rot.GetNormalized());
				break;
			}


			//LocalRotation Euler인 경우....
			case(AnimationClip::PropertyType::Local_Rotation_Euler): {
				Vector3 new_Euler = prev.Vec3;
				new_Euler.x += (nxt.Vec3.x - prev.Vec3.x) * GetCurveY(prop.Curves[prev.CurveStartIdx], normalizedTime);
				new_Euler.y += (nxt.Vec3.y - prev.Vec3.y) * GetCurveY(prop.Curves[prev.CurveStartIdx + 1], normalizedTime);
				new_Euler.z += (nxt.Vec3.z - prev.Vec3.z) * GetCurveY(prop.Curves[prev.CurveStartIdx + 2], normalizedTime);

				bone_tr->SetLocalRotation(Quaternion::Euler(new_Euler.y, new_Euler.x, new_Euler.z));
				break;
			}
		}

	}
}












/*============================================================================================================================================
 *   현재 애니메이션의 재생 시간을 정규화 된 값으로 얻거나/설정합니다....
 *********/
float hyunwoo::AnimateMesh::GetNormalizedTime() const
{
	return (m_time * m_totalTimeDiv);
}

void hyunwoo::AnimateMesh::SetNormalizedTime(float new_time)
{
	m_time = (m_totalTime * Math::Clamp01(new_time));
}














/*============================================================================================================================================
 *   AnimationClip의 약참조를 얻거나, 설정합니다....
 *********/
void hyunwoo::AnimateMesh::SetCurrentClip(AnimationClip* new_clip)
{
	m_cur_clip = new_clip;

	/*****************************************************
	 *  인자로 주어진 AnimationClip이 유효하지 않는가?
	 *******/
	if (new_clip==nullptr) {
		m_totalTime    = 0.f;
		m_totalTimeDiv = 1.f;
	}


	/*****************************************************
	 *  인자로 주어진 AnimationClip이 유효한가?
	 *******/
	else {
		m_totalTime = new_clip->TotalTime;
		m_totalTimeDiv = (1.f / new_clip->TotalTime);
		OnUpdateBoneTransforms();
	}
}

hyunwoo::WeakPtr<hyunwoo::AnimationClip> hyunwoo::AnimateMesh::GetCurrentClip() const
{
	return m_cur_clip;
}















/*============================================================================================================================================
 *   인자로 받은 CurveVariant 객체로부터, 적절한 y값을 얻습니다....
 *********/
float hyunwoo::AnimateMesh::GetCurveY(const CurveVariant& curve_variant, float normalizedTime) const
{
	switch (curve_variant.Type)
	{
		//상수 커브일 경우....
		case(CurveVariant::VariantType::Constant): {
			return curve_variant.Constant.Yvalue;
		}

		//1차 베지어일 경우...
		case(CurveVariant::VariantType::Linear_Bazier): {
			return curve_variant.LinearBezier.Evaluate(normalizedTime);
		}

		//2차 베지어 커브일 경우....
		case(CurveVariant::VariantType::Quadratic_Bazier): {
			return curve_variant.QuadraticBezier.Evaluate(normalizedTime);
		}

		//3차 베지어 커브일 경우....
		case(CurveVariant::VariantType::Cubic_Bezier): {
			return curve_variant.CubicBezier.Evaluate(normalizedTime);
		}
	}

	return 0.0f;
}










/*============================================================================================================================================
 *   인자로 받은 PropertyType이 사용하는 커브 개수를 얻습니다....
 *********/
uint32_t hyunwoo::AnimateMesh::GetCurveCount(const AnimationClip::PropertyType propType)
{
	switch (propType)
	{
		//벡터 성분은 3개의 커브를 사용한다....
		case(AnimationClip::PropertyType::Local_Position):
		case(AnimationClip::PropertyType::Local_Rotation_Euler):
		case(AnimationClip::PropertyType::Local_Scale): {
			return 3;
		}

		//사원수를 사용하는 회전의 경우, 1개의 커브를 사용한다...
		case(AnimationClip::PropertyType::Local_Rotation_Quat): {
			return 1;
		}
	}

	return 0;
}




















/*============================================================================================================================================
 *   주어진 시간값을 포함하는 시작 키프레임의 참조를 얻습니다....
 *********/
hyunwoo::AnimateMesh::KeyFramePair hyunwoo::AnimateMesh::GetKeyFramePairByTime(const std::vector<AnimationClip::KeyFrame>& keyFrames, float time)
{
	/*************************************************************************
	 *   KeyFrame이 존재하지 않는다면, 해당값을 반환하고 함수를 종료한다...
	 ********/
	if (keyFrames.size()==0) {
		return KeyFramePair{ m_invalid_idx, m_invalid_idx };
	}


	/*************************************************************************
	 *   주어진 시간이, 마지막 키프레임의 시간값보다 크다면, 해당값을 반환하고 함수를
	 *   종료한다....
	 ********/
	else if (keyFrames.back().Time < time) {
		const uint32_t back_idx = (keyFrames.size() - 1);
		return KeyFramePair{ back_idx, back_idx };
	}


	/*************************************************************************
	 *   주어진 시간이, 마지막 키프레임의 시간값보다 작다면, 해당값을 반환하고 함수를
	 *   종료한다....
	 ********/
	else if (keyFrames.front().Time > time) {
		return KeyFramePair{ 0, 0 };
	}



	/**************************************************************************
	 *  이진 탐색을 통해, 주어진 시간이 포함되는 키프레임 두개를 찾는다...
	 *********/
	uint32_t low  = 0;
	uint32_t high = (keyFrames.size() - 1);

	while (low <= high) 
	{
		const uint32_t mid = (low + high) >> 1; 

		if (keyFrames[mid].Time <= time) {

			/*-----------------------------------------------
			 *   두 키프레임 사이에 주어진 시간값이 포함된다면,
			 *   값을 반환하고 함수를 종료한다...
			 *******/
			const uint32_t mid_nxt = (mid + 1);

			if (keyFrames[mid_nxt].Time >= time) {
				return KeyFramePair{ mid, mid_nxt };
			}

			/*--------------------------------------------
			 *  중간값의 시간값이, 주어진 시간값보다 작다면,
			 *  low를 mid_nxt로 설정해 우측 범위로 좁힌다...
			 ******/
			else low = mid_nxt;
		}

		/*------------------------------------------
		 *  중간값의 시간값이, 주어진 시간값보다 크다면,
		 *  high를 mid-1로 설정해, 좌측 범위로 좁힌다..
		 *******/
		else high = (mid - 1);

	}

	return KeyFramePair{ low, high };
}
