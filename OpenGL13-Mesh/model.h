#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.h"
#include "mesh.h"

class Model {
public:
	std::vector<Texture> textures_loaded;
	// model data
	std::vector<Mesh> meshes;
	std::string directory;
	bool gamma_correction;
	Model(const char* path, bool gamma = false) : gamma_correction(gamma) {
		LoadModel(path);
	}
	void Draw(Shader& shader);
private:
	// functions
	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string type_name);
};
