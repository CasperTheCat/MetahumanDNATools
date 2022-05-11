#pragma once

#include <glm/vec3.hpp>
#include <string>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>
#include <unordered_map>

// Create a bone structure in a way we can understand
struct Bone
{
	std::string BoneName;
	glm::dvec3 BonePosition;
	glm::dvec3 BoneOrientation;

};

struct AssimpImport
{
	Assimp::Importer Importer;
	const aiScene* Scene;
};


// Load from FBX
std::shared_ptr<AssimpImport> LoadFileFromPath(std::string& f);
std::shared_ptr<AssimpImport> LoadFileFromPath(std::string f);

std::shared_ptr<std::unordered_map<std::string, Bone>> GetBones(std::shared_ptr < AssimpImport> AssimpScene);

