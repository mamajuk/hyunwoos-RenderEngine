#include "CCD_IKSolver.h"

/*===========================================================================================================================================
 *   설정된 정보를 바탕으로, CCD IK를 적용한다.....
 ***********/
void hyunwoo::CCD_IKSolver::ResolveIK(const AnimateMesh& animateMesh)
{
	/*****************************************************************************************
	 *  해당 작업에 필요한 모든 리소스를 읽어들이고, 유효성을 확인한다...
	 *********/

	/*-----------------------------------------
	 *  로컬 포즈를 얻어올 메시 리소스는 유효한가?
	 *******/
	Mesh* mesh = animateMesh.GetMesh().Get();
	if (mesh==nullptr) {
		return;
	}

	/*------------------------------------------
	 *  CCD IK에 사용될 target/endEff bone 
	 *  transform이 유효한가?
	 *******/
	for (uint32_t i = 0; i < ResolveDescs.size(); i++) 
	{
		const ResolveDescriptor& desc = ResolveDescs[i];

		Transform* target_tr = animateMesh.GetBoneTransformAt(desc.Target_bone_idx).Get();
		Transform* endEff_tr = animateMesh.GetBoneTransformAt(desc.EndEff_bone_idx).Get();

		if (target_tr==nullptr || endEff_tr==nullptr) {
			return;
		}
	}

	/*-----------------------------------------
	 *  모든 링크 본들이 유효한가?
	 *******/
	for (uint32_t i=0; i<Links.size(); i++)
	{
		Link&	   link    = Links[i];
		Transform* link_tr = animateMesh.GetBoneTransformAt(link.Bone_idx).Get();

		if (link_tr==nullptr) {
			return;
		}
	}



	/******************************************************************************************
	 *   모든 ResolveDescriptor를 순회하여, IK를 적용한다....
	 *********/
	for (uint32_t i=0; i<ResolveDescs.size(); i++)
	{
		const ResolveDescriptor& desc = ResolveDescs[i];

		Transform* target_tr = animateMesh.GetBoneTransformAt(desc.Target_bone_idx).Get();
		Transform* endEff_tr = animateMesh.GetBoneTransformAt(desc.EndEff_bone_idx).Get();


		/*-----------------------------------------------------------------------
		 *   정해진 반복수만큼, CCD IK를 적용한다....
		 ********/
		for (uint32_t j = 0; j<desc.Loop_count; j++) 
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			 * endEff가 target에 충분히 가까워졌다면, 작업을 
			 * 중단한다......
			 ************/
			const float endEff2Target_dst = (target_tr->GetWorldPosition() - endEff_tr->GetWorldPosition()).GetMagnitude();
			if (endEff2Target_dst < std::numeric_limits<float>::epsilon()) {
				break;
			}


			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			 *  해당 resolveDesc에서 사용되는 모든 링크들을 순회
			 *  하여, ccd ik를 적용한다. CCD IK가 반복 적용되면서
			 *  발생하는 뒤틀림을 완화하기 위해서, CCD IK적용 전
			 *  최초 Local Rotation값으로 회전을 초기화한다...
			 ***********/
			for (uint32_t k = 0; k < desc.Link_count; k++) 
			{
				const Link& link      = Links[desc.Link_start_idx + k];
				const Bone& link_bone = mesh->Bones[link.Bone_idx];
				Transform*  link_tr   = animateMesh.GetBoneTransformAt(link.Bone_idx).Get();


				link_tr->SetLocalPosition(link_bone.BindingPose.LocalPosition);
				link_tr->SetLocalRotation(link_bone.BindingPose.LocalRotation);

				const Vector3 link2EndEffDir = (endEff_tr->GetWorldPosition() - link_tr->GetWorldPosition()).GetNormalized();
				const Vector3 link2TargetDir = (target_tr->GetWorldPosition() - link_tr->GetWorldPosition()).GetNormalized();

				const Quaternion fromTo = Quaternion::FromTo(link2EndEffDir, link2TargetDir);
				link_tr->SetWorldRotation((fromTo * link_tr->GetWorldRotation()).GetNormalized());

				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				 *  각 회전축당 회전량 제한이 있는가?
				 ******/
				if (link.Has_Limits){
					const Link::AngleLimits& angleLimits = link.Limits;
					const Vector3			 link_euler  = Quaternion::ToEuler_XYZ(link_tr->GetLocalRotation());

					const Vector3 limits_angles = Vector3(
						-Math::Clamp(link_euler.x, angleLimits.Limit_Min.x, angleLimits.Limit_Max.x) * Math::Rad2Angle,
						-Math::Clamp(link_euler.y, angleLimits.Limit_Min.y, angleLimits.Limit_Max.y) * Math::Rad2Angle,
						-Math::Clamp(link_euler.z, angleLimits.Limit_Min.z, angleLimits.Limit_Max.z) * Math::Rad2Angle
					);

					const Quaternion final_euler = Quaternion::Euler(limits_angles.y, limits_angles.x, limits_angles.z);
					link_tr->SetLocalRotation(final_euler.GetNormalized());
				}
			}

		}


	}
}
