#include"Model.h"

Model::Model(const char* file, glm::vec3 scale, glm::mat4 initTransform, bool should_compute_center){
	// Make a JSON object
	std::string text = get_file_contents(file);
	JSON = json::parse(text);

	// Get the binary data
	Model::file = file;
	data = getData();
	modelScale = scale;

	// Traverse all nodes
	traverseNode(0);

	initTransformMat = initTransform;
	compute_center = should_compute_center;
	if(compute_center){
		findBindingBox();
		computeCenter();
		glm::vec4 transformedCenter = initTransform * myTotalTransformation * glm::scale(glm::mat4(1.0f), scale) * matricesMeshes[0] * glm::vec4(glm::vec3(centerX, centerY, centerZ), 1.0f);
		centerX = transformedCenter.x;
		centerY = transformedCenter.y;
		centerZ = transformedCenter.z;
	}
}

void Model::Draw(Shader& shader, Camera& camera, glm::vec2 screenResolution, glm::vec3 translation){
	//Draw all meshes
	for(unsigned int i = 0; i < meshes.size(); i++){
		shader.Activate();
		//This is to pass the the screen resolution and the current time into the shader
		//for potential animation purposes.
		glUniform2f(glGetUniformLocation(shader.ID, "iResolution"), screenResolution.x, screenResolution.y);
		glUniform1f(glGetUniformLocation(shader.ID, "iTime"), glfwGetTime());

		//Pass the transformation matrix re
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "myTotalTransformation"), 1, GL_FALSE, glm::value_ptr(myTotalTransformation));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "initTransform"), 1, GL_FALSE, glm::value_ptr(initTransformMat));
		meshes[i].Mesh::Draw(shader, camera, matricesMeshes[i], translation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), modelScale);
	}

	if(compute_center){
		glm::vec4 transformedCenter = initTransformMat * myTotalTransformation * glm::scale(glm::mat4(1.0f), modelScale) * matricesMeshes[0] * glm::vec4(glm::vec3(centerX, centerY, centerZ), 1.0f);
		centerX = transformedCenter.x;
		centerY = transformedCenter.y;
		centerZ = transformedCenter.z;
	}
}

void Model::handleKeyboardInputs(GLFWwindow* window, Shader& shader){
	
	//Reset all the rotation and the translations done.
	if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
		myRotation = myRotation * glm::inverse(myRotation);
		myTranslation = myTranslation * glm::inverse(myTranslation);
	}

	if(glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS){
		rotationAngleYaw = 1;
		if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
			myRotation = glm::rotate(myRotation, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}else{
			myRotation = glm::rotate(myRotation, glm::radians(rotationAngleYaw), glm::vec3(0.0f, 1.0f, 0.0f));
		}
	}

	if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
		rotationAngleYaw = -1;
		
		if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
			myRotation = glm::rotate(myRotation, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}else{	
			myRotation = glm::rotate(myRotation, glm::radians(rotationAngleYaw), glm::vec3(0.0f, 1.0f, 0.0f));
		}
	}

	if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
		rotationAnglePitch = 1;

		if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
			myRotation = glm::rotate(myRotation, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}else{
			myRotation = glm::rotate(myRotation, glm::radians(rotationAnglePitch), glm::vec3(1.0f, 0.0f, 0.0f));
		}
	}

	if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
		rotationAnglePitch = -1;

		if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
			myRotation = glm::rotate(myRotation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}else{
			myRotation = glm::rotate(myRotation, glm::radians(rotationAnglePitch), glm::vec3(1.0f, 0.0f, 0.0f));
		}
	}

	if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
		rotationAngleRoll = 1;

		if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
			myRotation = glm::rotate(myRotation, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}else{
			myRotation = glm::rotate(myRotation, glm::radians(rotationAngleRoll), glm::vec3(0.0f, 0.0f, 1.0f));
		}
	}

	if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
		rotationAngleRoll = -1;

		if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
			myRotation = glm::rotate(myRotation, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}else{
			myRotation = glm::rotate(myRotation, glm::radians(rotationAngleRoll), glm::vec3(0.0f, 0.0f, 1.0f));
		}
	}

	if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		translationY = 0.1;

		if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
			//This is actually a Z translation. Just using the Y value
			myTranslation = glm::translate(myTranslation, glm::vec3(0.0f, 0.0f, translationY));
		}else{
			myTranslation = glm::translate(myTranslation, glm::vec3(0.0f, translationY, 0.0f));
		}
	}

	if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		translationY = -0.1;
		
		if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
			//This is actually a Z translation. Just using the Y value
			myTranslation = glm::translate(myTranslation, glm::vec3(0.0f, 0.0f, translationY));
		}else{
			myTranslation = glm::translate(myTranslation, glm::vec3(0.0f, translationY, 0.0f));
		}
	}

	if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		translationX =  -0.1;
		myTranslation = glm::translate(myTranslation, glm::vec3(translationX, 0.0f, 0.0f));
	}

	if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		translationX = 0.1;
		myTranslation = glm::translate(myTranslation, glm::vec3(translationX, 0.0f, 0.0f));
	}

	myTotalTransformation = myRotation * myTranslation;
}

void Model::loadMesh(unsigned int indMesh, glm::mat4 modelMatrix, glm::vec3 translation, glm::quat rotation, glm::vec3 scale){
	// Get all accessor indices

	for(int i = 0; i <  JSON["meshes"][indMesh]["primitives"].size(); i++){
		unsigned int posAccInd    = JSON["meshes"][indMesh]["primitives"][i]["attributes"]["POSITION"];
		unsigned int normalAccInd = JSON["meshes"][indMesh]["primitives"][i]["attributes"]["NORMAL"];
		unsigned int texAccInd    = JSON["meshes"][indMesh]["primitives"][i]["attributes"]["TEXCOORD_0"];
		unsigned int indAccInd    = JSON["meshes"][indMesh]["primitives"][i]["indices"];

		// Use accessor indices to get all vertices components
		std::vector<float> posVec        = getFloats(JSON["accessors"][posAccInd]);
		std::vector<glm::vec3> positions = groupFloatsVec3(posVec);
		std::vector<float> normalVec     = getFloats(JSON["accessors"][normalAccInd]);
		std::vector<glm::vec3> normals   = groupFloatsVec3(normalVec);
		std::vector<float> texVec        = getFloats(JSON["accessors"][texAccInd]);
		std::vector<glm::vec2> texUVs    = groupFloatsVec2(texVec);

		// Combine all the vertex components and also get the indices and textures
		std::vector<Vertex> vertices  = assembleVertices(positions, normals, texUVs);
		std::vector<GLuint> indices   = getIndices(JSON["accessors"][indAccInd]);
		std::vector<Texture> textures = getTextures();

		// Combine the vertices, indices, and textures into a mesh
		meshes.push_back(Mesh(vertices, indices, textures));
	
		//If there is more than one primitive, I push the same modelMatrix for that primitive
		//this is fine since it should all be part of the same mesh.
		if(i >= 1){
			matricesMeshes.push_back(modelMatrix);
			translationsMeshes.push_back(translation);
			rotationsMeshes.push_back(rotation);
			scalesMeshes.push_back(scale);
		}
	}
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix){
	//This is the current node
	json node = JSON["nodes"][nextNode];

	//We get the translation stored at that node
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	if(node.find("translation") != node.end()){
		float transValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++)
			transValues[i] = (node["translation"][i]);
		translation = glm::make_vec3(transValues);
	}

	//We get the quaternion at that node
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if(node.find("rotation") != node.end()){
		float rotValues[4] =
		{
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		rotation = glm::make_quat(rotValues);
	}

	//We get the scale at that node
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	if(node.find("scale") != node.end()){
		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++)
			scaleValues[i] = (node["scale"][i]);
		scale = glm::make_vec3(scaleValues);
	}

	//We get the matrix at that node
	glm::mat4 matNode = glm::mat4(1.0f);
	if(node.find("matrix") != node.end()){
		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
			matValues[i] = (node["matrix"][i]);
		matNode = glm::make_mat4(matValues);
	}

	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	//We convert the vec3, quats and floats got from the node
	//into a mat4
	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	//Get the meshMatrix
	glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

	//Load the mesh
	if(node.find("mesh") != node.end()){
		translationsMeshes.push_back(translation);
		rotationsMeshes.push_back(rotation);
		scalesMeshes.push_back(scale);
		matricesMeshes.push_back(matNextNode);
		loadMesh(node["mesh"], matNextNode, translation, rotation, scale);
	}

	// Check if the node has children, and if it does, apply this function to them with the matNextNode
	if(node.find("children") != node.end()){
		for (unsigned int i = 0; i < node["children"].size(); i++)
			traverseNode(node["children"][i], matNextNode);
	}
}

std::vector<unsigned char> Model::getData(){
	// Create a place to store the raw text, and get the uri of the .bin file
	std::string bytesText;
	std::string uri = JSON["buffers"][0]["uri"];

	// Store raw text data into bytesText
	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
	bytesText = get_file_contents((fileDirectory + uri).c_str());

	// Transform the raw text data into bytes and put them in a vector
	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
	return data;
}

std::vector<float> Model::getFloats(json accessor){
	std::vector<float> floatVec;

	// Get properties from the accessor
	unsigned int buffViewInd = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	std::string type = accessor["type"];

	// Get properties from the bufferView
	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	// Interpret the type and store it into numPerVert
	unsigned int numPerVert;
	if (type == "SCALAR") numPerVert = 1;
	else if (type == "VEC2") numPerVert = 2;
	else if (type == "VEC3") numPerVert = 3;
	else if (type == "VEC4") numPerVert = 4;
	else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");

	// Go over all the bytes in the data at the correct place using the properties from above
	unsigned int beginningOfData = byteOffset + accByteOffset;
	unsigned int lengthOfData = count * 4 * numPerVert;
	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i){
		unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floatVec.push_back(value);
	}
	return floatVec;
}

std::vector<GLuint> Model::getIndices(json accessor){
	std::vector<GLuint> indices;

	// Get properties from the accessor
	unsigned int buffViewInd = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	// Get properties from the bufferView
	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView.value("byteOffset", 0);

	// Get indices with regards to their type: unsigned int, unsigned short, or short
	unsigned int beginningOfData = byteOffset + accByteOffset;	
	if(componentType == 5125){
		for(unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i){
			unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back((GLuint)value);
		}
	}else if(componentType == 5123){
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i)
		{
			unsigned char bytes[] = { data[i++], data[i++] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((GLuint)value);
		}
	}else if(componentType == 5122){
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i){
			unsigned char bytes[] = { data[i++], data[i++] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((GLuint)value);
		}
	}
	return indices;
}

std::vector<Texture> Model::getTextures(){
	std::vector<Texture> textures;

	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	// Go over all images
	for (unsigned int i = 0; i < JSON["images"].size(); i++){
		// uri of current texture
		std::string texPath = JSON["images"][i]["uri"];

		// Check if the texture has already been loaded
		bool skip = false;
		for(unsigned int j = 0; j < loadedTexName.size(); j++){
			if(loadedTexName[j] == texPath){
				textures.push_back(loadedTex[j]);
				skip = true;
				break;
			}
		}

		// If the texture has been loaded, skip this
		if (!skip){
			if (texPath.find("baseColor") != std::string::npos){
				// Load diffuse texture
				Texture diffuse = Texture((fileDirectory + texPath).c_str(), "diffuse", loadedTex.size());
				textures.push_back(diffuse);
				loadedTex.push_back(diffuse);
				loadedTexName.push_back(texPath);
			}else if(texPath.find("metallicRoughness") != std::string::npos){
				// Load specular texture
				Texture specular = Texture((fileDirectory + texPath).c_str(), "specular", loadedTex.size());
				textures.push_back(specular);
				loadedTex.push_back(specular);
				loadedTexName.push_back(texPath);
			}
		}
	}
	return textures;
}

std::vector<Vertex> Model::assembleVertices(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texUVs){
	std::vector<Vertex> vertices;
	for (int i = 0; i < positions.size(); i++){
		vertices.push_back(
			Vertex{
				positions[i],
				normals[i],
				glm::vec3(1.0f, 1.0f, 1.0f),
				texUVs[i]
			}
		);
	}
	return vertices;
}

std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVec){
	std::vector<glm::vec2> vectors;
	for (int i = 0; i < floatVec.size(); i){
		vectors.push_back(glm::vec2(floatVec[i++], floatVec[i++]));
	}
	return vectors;
}

std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec){
	std::vector<glm::vec3> vectors;
	for (int i = 0; i < floatVec.size(); i){
		vectors.push_back(glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]));
	}
	return vectors;
}

std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVec){
	std::vector<glm::vec4> vectors;
	for (int i = 0; i < floatVec.size(); i){
		vectors.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
	}
	return vectors;
}


void Model::findBindingBox(){
	for(unsigned int i = 0; i < meshes.size(); i++){
		for(unsigned int j = 0; j < meshes[i].vertices.size(); j++){
			if(meshes[i].vertices[j].position.x < minX) minX = meshes[i].vertices[j].position.x;
			if(meshes[i].vertices[j].position.y < minY) minY = meshes[i].vertices[j].position.y;
			if(meshes[i].vertices[j].position.z < minZ) minZ = meshes[i].vertices[j].position.z;

			if(meshes[i].vertices[j].position.x > maxX) maxX = meshes[i].vertices[j].position.x;
			if(meshes[i].vertices[j].position.y > maxY) maxY = meshes[i].vertices[j].position.y;
			if(meshes[i].vertices[j].position.z > maxZ) maxZ = meshes[i].vertices[j].position.z;
		}
	}
}

void Model::computeCenter(){
	centerX = (minX + maxX)/2;
	centerY = (minY + maxY)/2;
	centerZ = (minZ + maxZ)/2;
}

void Model::computeSize(){
	sizeX = maxX - minX;
	sizeY = maxY - minY;
	sizeZ = maxZ - minZ;
}

void Model::drawBoundingBox(Shader& shader, Camera& camera){
	std::vector<Vertex> vertices;

	vertices.push_back(Vertex{glm::vec3(minX, minY, minZ)});
	vertices.push_back(Vertex{glm::vec3(maxX, minY, minZ)});
	vertices.push_back(Vertex{glm::vec3(maxX, maxY, minZ)});
	vertices.push_back(Vertex{glm::vec3(minX, maxY, minZ)});
	vertices.push_back(Vertex{glm::vec3(minX, minY, maxZ)});
	vertices.push_back(Vertex{glm::vec3(maxX, minY, maxZ)});
	vertices.push_back(Vertex{glm::vec3(maxX, maxY, maxZ)});
	vertices.push_back(Vertex{glm::vec3(minX, maxY, maxZ)});

	std::vector<GLuint> indices = {	0, 1, 3,
									1, 3, 2,
									//Left Side
									0, 4, 3,
									4, 3, 7,
									//Right Side
									1, 5, 2,
									5, 2, 6,
									//Back Side
									5, 4, 7,
									5, 7, 6,
									//Top Side
									3, 2, 7,
									2, 7, 6,
									//Bottom Side
									0, 1, 4,
									1, 4, 5};

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "myTotalTransformation"), 1, GL_FALSE, glm::value_ptr(myTotalTransformation));
	std::vector<Texture> textures;
	Mesh boundingBox = Mesh(vertices, indices, textures);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	boundingBox.Draw(shader, camera, glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(0.0f, 1.0f, 0.0f, 0.0f), modelScale);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}