#pragma once
#include "../../EngineModule/UniqueableObject.h"
#include "../AnimateMesh.h"

namespace hyunwoo {
	class IKSolver;
}


/*=========================================================================================================================================================
 *   Animation Update 후에 적용되는 모든 IKSolver들의 기반 클래스입니다....
 **********/
class hyunwoo::IKSolver : public hyunwoo::UniqueableObject
{
public:
	virtual void ResolveIK(const AnimateMesh& animateMesh) = 0;
};