#include "Bones.h"
#include <assimp/postprocess.h>     // Post processing flags

std::shared_ptr<AssimpImport> LoadFileFromPath(std::string &f)
{
	return LoadFileFromPath(std::move(f));
}

std::shared_ptr<AssimpImport> LoadFileFromPath(std::string f)
{
	auto returnable = std::make_shared<AssimpImport>();

	Assimp::Importer importer;
	returnable->Scene = returnable->Importer.ReadFile(f, 0);// aiProcess_PopulateArmatureData);

	return returnable;
}

#include <iostream>

glm::vec3 UnAssimpVector(aiVector3t<float>& in)
{
	glm::vec3 ret;

	ret.x = in.x;
	ret.y = in.y;
	ret.z = in.z;

	return ret;
}

bool IsTranslateNode(std::string& in)
{
	return !(-1 == in.find("_$AssimpFbx$_Translation"));
}

bool IsRotateNode(std::string& in)
{
	return !(-1 == in.find("_$AssimpFbx$_PreRotation"));
}

bool IsBanned(std::string& in)
{
	return !(-1 == in.find("DHIbody"));
}

std::string CleanName(std::string in)
{
	auto loc = in.find(":");
	if (-1 != loc)
	{
		in = in.substr(loc + 1);
	}

	// Strip
	loc = in.find("_$AssimpFbx$_Translation");
	if (-1 != loc)
	{
		in = in.substr(0, loc);
	}

	// Strip
	loc = in.find("_$AssimpFbx$_PreRotation");
	if (-1 != loc)
	{
		in = in.substr(0, loc);
	}
	
	return in;
}




void BoneRecursiveDescent(std::shared_ptr<std::unordered_map<std::string, Bone>>& Bones, aiNode* pNode)
{
	if (pNode)
	{
		// Convert
		aiVector3t<float> pScaling, pRotation, pPosition;
		pNode->mTransformation.Decompose(pScaling, pRotation, pPosition);

		auto nameStr = std::string(pNode->mName.C_Str());
		auto cleanStr = CleanName(nameStr);

		if (cleanStr.compare("spine_04") == 0)
		{
			std::cout << "Skipping Spine04. The head is rebased onto the body's version of this bone." << std::endl;
		}

		if (IsBanned(nameStr))
		{
			// Okay. DHIbody was left in the file
			// Bin this tree
			return;
		}

		if (!Bones->contains(cleanStr))
		{
			// Add the node
			Bone skelingtonBone{};
			skelingtonBone.BoneName = nameStr;
			skelingtonBone.BonePosition = UnAssimpVector(pPosition);
			skelingtonBone.BoneOrientation = UnAssimpVector(pRotation);
			Bones->try_emplace(cleanStr, std::move(skelingtonBone));
		}

		if (IsTranslateNode(nameStr))
		{
			(*Bones)[cleanStr].BonePosition = UnAssimpVector(pPosition);
		}

		if (IsRotateNode(nameStr))
		{
			(*Bones)[cleanStr].BoneOrientation = UnAssimpVector(pRotation);
		}

		


		//Bones->push_back(std::move(skelingtonBone));

		
		
		// List Kids
		for (uint32_t i = 0; i < pNode->mNumChildren; ++i)
		{
			BoneRecursiveDescent(Bones, pNode->mChildren[i]);
		}
	}
}

std::shared_ptr<std::unordered_map<std::string, Bone>> GetBones(std::shared_ptr<AssimpImport> AssimpScene)
{
	auto localScenePtr = AssimpScene->Scene;

	std::shared_ptr<std::unordered_map<std::string, Bone>> Bones = std::make_shared< std::unordered_map<std::string, Bone>>();

	if (localScenePtr->mRootNode)
	{
		// GO
		BoneRecursiveDescent(Bones, localScenePtr->mRootNode);
	}

	return Bones;
}
