#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/types.h>
#include "shader.h"

#define MAX_BONE_INFLUENCE 4

namespace mesh {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 tex_coords;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		//bone indexes which will influence this vertex
		int bone_ids[MAX_BONE_INFLUENCE];
		//weights from each bone
		float weights[MAX_BONE_INFLUENCE];
	};

	struct Texture {
		unsigned int id { 0 };
		std::string  type;
		std::string  path;
	};

	class Mesh {
	public:
		// mesh data
		std::vector<Vertex>  vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		// functions
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
		void Draw(Shader& shader);

	private:
		// render data
		unsigned int VAO_, VBO_, EBO_;
		// init
		void SetupMesh();
	};
};

