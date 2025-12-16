#pragma once
#include "../RenderModule/RenderMesh.h"
#include "AnimationClip.h"


namespace hyunwoo {
	class AnimateMesh;
}


/*=========================================================================================================================================================
 *   주어진 AnimationClip을 통해, 매 프레임 업데이트 때마다 RenderMesh의 BoneTransform을 보간해주는 기능을 제공하는 TransformComponent입니다....
 **********/
class hyunwoo::AnimateMesh final : public hyunwoo::RenderMesh
{
	//==========================================================================================
	////////////							Defines...								////////////
	//==========================================================================================
public:
	/**************************************************
	 *   유효하지 않은 키프레임의 인덱스를 나타내는 상수..
	 ******/
	constexpr static inline uint32_t m_invalid_idx = UINT32_MAX;


	/**************************************************
	 *  두개의 키프레임의 인덱스를 담는 구조체입니다...
	 *******/
	struct KeyFramePair
	{
		uint32_t left, right;
	};



	//==========================================================================================
	////////////							Fields...								////////////
	//==========================================================================================
private:
	float				   m_time		  = 0.f;
	float				   m_totalTime    = 0.f;
	float				   m_totalTimeDiv = 0.f;
	WeakPtr<AnimationClip> m_cur_clip;
	std::vector<uint32_t>  m_prop2BoneIdx;




	//==================================================================================================
	////////////						 Override methods...								////////////
	//==================================================================================================
protected:
	virtual void OnUpdateBoneTransforms() override;



	//==================================================================================================
	////////////						 Public methods...								    ////////////
	//==================================================================================================
public:
	void Update(float deltaTime);

	/************************************
	 *  정규화된 애니메이션 시간을 얻거나, 
	 *  설정하는 메소드들..
	 ********/
	float GetNormalizedTime() const;
	void  SetNormalizedTime(float new_time);


	/******************************
	 *  AnimationClip을 얻거나, 
	 *  설정하는 메소드들...
	 *******/
	void				   SetCurrentClip(AnimationClip* new_clip);
	WeakPtr<AnimationClip> GetCurrentClip() const;




	//==================================================================================================
	////////////						 Private methods...								    ////////////
	//==================================================================================================
private:
	float        GetCurveY(const CurveVariant& curve_variant, float normalizedTime) const;
	uint32_t     GetCurveCount(const AnimationClip::PropertyType propType);
	KeyFramePair GetKeyFramePairByTime(const std::vector<AnimationClip::KeyFrame>& keyFrames, float time);
};