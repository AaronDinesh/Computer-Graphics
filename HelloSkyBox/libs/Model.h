#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <nlohmann/json.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Mesh.h"


using json = nlohmann::json;


class Model
{
public:
	// All the meshes and transformations
	std::vector<Mesh> meshes;
	std::vector<glm::vec3> translationsMeshes;
	std::vector<glm::quat> rotationsMeshes;
	std::vector<glm::vec3> scalesMeshes;
	std::vector<glm::mat4> matricesMeshes;

	// Prevents textures from being loaded twice
	std::vector<std::string> loadedTexName;
	std::vector<Texture> loadedTex;


	int oldLeftShiftState = GLFW_RELEASE; 
	float rotationAngleYaw, rotationAnglePitch, rotationAngleRoll = 0;
	float translationX, translationY, translationZ = 0;
	float minX, minY, minZ, maxX, maxY, maxZ = 0.0f;
	float centerX, centerY, centerZ = 0.0f;
	float sizeX, sizeY, sizeZ = 0.0f;
	float modelScale;
	glm::mat4 combinedModelMatrix = glm::mat4(1.0f);

	glm::mat4 initModelMatrix = glm::mat4(1.0f);
	glm::mat4 myRotation = glm::mat4(1.0f);
	glm::mat4 myTranslation = glm::mat4(1.0f);
	glm::mat4 myTotalTransformation = glm::mat4(1.0f);

	// Loads in a model from a file and stores tha information in 'data', 'JSON', and 'file'
	Model(const char* file, float scale, glm::mat4 initTransform = glm::mat4(1.0f));

	void Draw(Shader& shader, Camera& camera, glm::vec2 screenResolution);
	void handleKeyboardInputs(GLFWwindow* window, Shader& shader);
	void drawBoundingBox(Shader& shader, Camera& camera);

private:
	// Variables for easy access
	const char* file;
	std::vector<unsigned char> data;
	json JSON;



	// Loads a single mesh by its index
	void loadMesh(unsigned int indMesh);

	// Traverses a node recursively, so it essentially traverses all connected nodes
	void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

	// Gets the binary data from a file
	std::vector<unsigned char> getData();
	// Interprets the binary data into floats, indices, and textures
	std::vector<float> getFloats(json accessor);
	std::vector<GLuint> getIndices(json accessor);
	std::vector<Texture> getTextures();

	// Assembles all the floats into vertices
	std::vector<Vertex> assembleVertices
	(
		std::vector<glm::vec3> positions, 
		std::vector<glm::vec3> normals, 
		std::vector<glm::vec2> texUVs
	);

	// Helps with the assembly from above by grouping floats
	std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
	std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
	std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);

	void findBindingBox();
	void computeCenter();
	void computeSize();
};
#endif