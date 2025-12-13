#pragma once
#include "IKSolver.h"
#include "../../MathModule/Vector.h"
#include <vector>

namespace hyunwoo {
	class CCD_IKSolver;
}


/*=========================================================================================================================================================
 *   CCD IK를 적용해주는 IKSolver입니다....
 **********/
class hyunwoo::CCD_IKSolver final : public hyunwoo::IKSolver
{
	//========================================================================================
	/////////////						   Defines...							//////////////
	//========================================================================================
public:

	/***************************************************
	 *  적용할 CCD IK를 서술하는 구조체입니다....
	 ******/
	struct ResolveDescriptor
	{
		int32_t Target_bone_idx = -1;
		int32_t EndEff_bone_idx = -1;
		int32_t Loop_count		= 0;
		int32_t Link_start_idx  = 0;
		int32_t Link_count		= 0;
		float   Limit_radian    = Math::PI;
	};


	/**************************************************
	 *  CCD IK의 각 링크들을 나타내는 구조체입니다....
	 ********/
	struct Link
	{
		/*---------------------------------------
		 *  각 링크들의 회전 제한을 나타내는 구조체..
		 *******/
		struct AngleLimits
		{
			Vector3 Limit_Min;
			Vector3 Limit_Max;
		};

		int32_t     Bone_idx   = -1;
		bool        Has_Limits = false;
		AngleLimits Limits;
	};




	//============================================================================================
	/////////////						   Properties...							//////////////
	//============================================================================================
public:
	std::vector<ResolveDescriptor> ResolveDescs;
	std::vector<Link>			   Links;




	//================================================================================================
	/////////////						   Override methods..							//////////////
	//================================================================================================
public:
	virtual void ResolveIK(const AnimateMesh& animateMesh) override;

};
