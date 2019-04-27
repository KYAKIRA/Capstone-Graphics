#include "PerlinMaterial.h"

#include "Core/Time.h"
#include "Graphics/RenderEngine.h"
#include "Resource/ResourceManager.h"
#include "Tools/Random.h"

using namespace PizzaBox;

bool PerlinMaterial::start;
int PerlinMaterial::B;
int PerlinMaterial::BM;

int PerlinMaterial::p[514];
float PerlinMaterial::g1[514];
Vector2 PerlinMaterial::g2[514];
Vector3 PerlinMaterial::g3[514];

GLubyte* PerlinMaterial::Noise3DTexPtr;

PerlinMaterial::PerlinMaterial(const std::string& diffMap_, const std::string& specMap_, const std::string& normalMap_, float shiny_, float speedMult_, float offsetMult_, bool textureIsMoving_) : MeshMaterial("PerlinShader"), diffuseMapName(diffMap_), specularMapName(specMap_), specularMap(nullptr), diffuseMap(nullptr), normalMapName(normalMap_), normalMap(nullptr), shininess(shiny_), speedMult(speedMult_), offsetMult(offsetMult_), textureIsMoving(textureIsMoving_), time(0.0f), textureScale(1.0f), noiseTexture(0){
	_ASSERT(!diffuseMapName.empty());

#ifdef _DEBUG
	if(offsetMult == 0.0f){
		Debug::LogWarning("Do not instatiate offsetMult to zero, defaulting to 25");
		offsetMult = 25.0f;
	}

	if(speedMult == 0.0f){
		Debug::LogWarning("Do not instatiate speedMult to zero, defaulting to 5");
		speedMult = 5.0f;
	}
#endif //_DEBUG


	if(specMap_.empty()){
		specularMapName = diffuseMapName;
	}

	if(normalMap_.empty()){
		normalMapName = diffuseMapName;
	}
}

PerlinMaterial::~PerlinMaterial(){
}

bool PerlinMaterial::Initialize(){
	shader = ResourceManager::LoadResource<Shader>(shaderName);
	if(shader == nullptr){
		Debug::LogError(shaderName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	diffuseMap = ResourceManager::LoadResource<Texture>(diffuseMapName);
	if(diffuseMap == nullptr){
		Debug::LogError(diffuseMapName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	specularMap = ResourceManager::LoadResource<Texture>(specularMapName);
	if(specularMap == nullptr){
		Debug::LogError(specularMapName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	normalMap = ResourceManager::LoadResource<Texture>(normalMapName);
	if(normalMap == nullptr){
		Debug::LogError(normalMapName + " could not be loaded!", __FILE__, __LINE__);
		return false;
	}

	noiseTexture = CreateNoise3D();

	return true;
}

void PerlinMaterial::Destroy(){
	//TODO - Cleanup noise texture

	if(diffuseMap != nullptr){
		ResourceManager::UnloadResource(diffuseMapName);
		diffuseMap = nullptr;
	}

	if(specularMap != nullptr){
		ResourceManager::UnloadResource(specularMapName);
		specularMap = nullptr;
	}

	if(normalMap != nullptr){
		ResourceManager::UnloadResource(normalMapName);
		normalMap = nullptr;
	}

	if(shader != nullptr){
		ResourceManager::UnloadResource(shaderName);
		shader = nullptr;
	}
}

void PerlinMaterial::Update(){
	time += Time::DeltaTime();
}

void PerlinMaterial::Render(const Camera* camera_, const Matrix4& model_, const std::vector<DirectionalLight*>& dirLights, const std::vector<PointLight*>& pointLights, const std::vector<SpotLight*>& spotLights) const{
	shader->Use();

	shader->BindMatrix4("projectionMatrix", camera_->GetProjectionMatrix());
	shader->BindMatrix4("viewMatrix", camera_->GetViewMatrix());
	shader->BindMatrix4("modelMatrix", model_);
	shader->BindMatrix3("normalMatrix", Matrix3(model_));
	shader->BindVector3("viewPos", camera_->GetGameObject()->GlobalPosition());

	//Light
	shader->BindColor("baseAmbient", RenderEngine::baseAmbient); //Base Lighting on all Object so they appear 
	
	//Directional Lights
	for(unsigned int i = 0; i < dirLights.size(); i++){
		shader->BindDirectionalLight(i, dirLights[i]);
	}
	shader->BindInt("numDirLights", static_cast<int>(dirLights.size()));

	//Point Lights
	for(unsigned int i = 0; i < pointLights.size(); i++){
		shader->BindPointLight(i, pointLights[i]);
	}
	shader->BindInt("numPointLights", static_cast<int>(pointLights.size()));

	//Spot Lights
	for(unsigned int i = 0; i < spotLights.size(); i++){
		shader->BindSpotLight(i, spotLights[i]);
	}
	shader->BindInt("numSpotLights", static_cast<int>(spotLights.size()));

	shader->BindTexture3D("noiseTexture", noiseTexture);
	shader->BindFloat("time", time);
	shader->BindFloat("speedMult", speedMult);
	shader->BindFloat("offsetMult", offsetMult);
	shader->BindInt("textureIsMoving", textureIsMoving);

	shader->BindTexture("material.diffuseMap", diffuseMap->TextureID());
	shader->BindTexture("material.specularMap", specularMap->TextureID());
	//shader->BindTexture("material.normalMap", normalMap->TextureID());
	shader->BindFloat("material.shininess", shininess);
	shader->BindInt("material.receivesShadows", receivesShadows);
	shader->BindFloat("material.textureScale", textureScale);
}

//EVERYTHING BELOW HERE WILL HANDLE PERLIN NOISE

GLuint PerlinMaterial::CreateNoise3D(){
	Make3DNoiseTexture();
	return Init3DNoiseTexture(noiseTextureSize, Noise3DTexPtr);
}

GLuint PerlinMaterial::Init3DNoiseTexture(int texSize, GLubyte* texPtr){
	GLuint texture;
	glGenTextures(1, &texture);

	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, texSize, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texPtr);

	free(texPtr);

	return texture;
}

void PerlinMaterial::SetNoiseFrequency(int frequency){
	start = true;
	B = frequency;
	BM = B - 1;
}

float PerlinMaterial::Noise1(float arg){
	if(start){
		start = false;
		InitNoise();
	}

	int bx0, bx1;
	float rx0, rx1, t;

	Setup(t, arg, bx0, bx1, rx0, rx1);

	float sx = SCurve(rx0);
	float u = rx0 * g1[p[bx0]];
	float v = rx1 * g1[p[bx1]];

	return Lerp(sx, u, v);
}

float PerlinMaterial::Noise2(const Vector2& vec){
	if(start){
		start = false;
		InitNoise();
	}

	int bx0, bx1, by0, by1;
	float rx0, rx1, ry0, ry1, t;

	Setup(t, vec.x, bx0, bx1, rx0, rx1);
	Setup(t, vec.y, by0, by1, ry0, ry1);

	int b00 = p[p[bx0] + by0];
	int b10 = p[p[bx1] + by0];
	int b01 = p[p[bx0] + by1];
	int b11 = p[p[bx1] + by1];

	float sx = SCurve(rx0);
	float sy = SCurve(ry0);

	float u = Vector2::Dot(g2[b00], Vector2(rx0, ry0));
	float v = Vector2::Dot(g2[b10], Vector2(rx1, ry0));
	float a = Lerp(sx, u, v);

	u = Vector2::Dot(g2[b01], Vector2(ry0, ry1));
	v = Vector2::Dot(g2[b11], Vector2(rx1, ry1));
	float b = Lerp(sx, u, v);

	return Lerp(sy, a, b);
}

float PerlinMaterial::Noise3(const Vector3& vec){
	if(start){
		start = false;
		InitNoise();
	}

	int bx0, bx1, by0, by1, bz0, bz1;
	float rx0, rx1, ry0, ry1, rz0, rz1, t;

	Setup(t, vec[0], bx0, bx1, rx0, rx1);
	Setup(t, vec[1], by0, by1, ry0, ry1);
	Setup(t, vec[2], bz0, bz1, rz0, rz1);

	int b00 = p[p[bx0] + by0];
	int b10 = p[p[bx1] + by0];
	int b01 = p[p[bx0] + by1];
	int b11 = p[p[bx1] + by1];

	t = SCurve(rx0);
	float sy = SCurve(ry0);
	float sz = SCurve(rz0);

	float u = Vector3::Dot(g3[b00 + bz0], Vector3(rx0, ry0, rz0));
	float v = Vector3::Dot(g3[b10 + bz0], Vector3(rx1, ry0, rz0));
	float a = Lerp(t, u, v);

	u = Vector3::Dot(g3[b01 + bz0], Vector3(rx0, ry1, rz0));
	v = Vector3::Dot(g3[b11 + bz0], Vector3(rx1, ry1, rz0));
	float b = Lerp(t, u, v);

	float c = Lerp(sy, a, b);

	u = Vector3::Dot(g3[b00 + bz1], Vector3(rx0, ry0, rz1));
	v = Vector3::Dot(g3[b10 + bz1], Vector3(rx1, ry0, rz1));
	a = Lerp(t, u, v);

	u = Vector3::Dot(g3[b01 + bz1], Vector3(rx0, ry1, rz1));
	v = Vector3::Dot(g3[b11 + bz1], Vector3(rx1, ry1, rz1));
	b = Lerp(t, u, v);

	float d = Lerp(sy, a, b);

	return Lerp(sz, c, d);
}

float PerlinMaterial::PerlinNoise2D(const Vector2& v, float alpha, float beta, int n){
	float sum = 0;
	float scale = 1;
	Vector2 p = v;

	for(int i = 0; i < n; i++){
		float val = Noise2(p);
		sum += val / scale;
		scale *= alpha;
		p *= beta;
	}

	return sum;
}

float PerlinMaterial::PerlinNoise3D(const Vector3& v, float alpha, float beta, int n){
	float sum = 0;
	float scale = 1;
	Vector3 p = v;

	for(int i = 0; i < n; i++){
		float val = Noise3(p);
		sum += val / scale;
		scale *= alpha;
		p *= beta;
	}

	return sum;
}

void PerlinMaterial::Make3DNoiseTexture(){
	int startFrequency = 4;
	int numOctaves = 4;
	Vector3 ni;
	float inci, incj, inck;
	int frequency = startFrequency;
	GLubyte* ptr;
	float amp = 0.5;

	Noise3DTexPtr = (GLubyte*)malloc(noiseTextureSize * noiseTextureSize * noiseTextureSize * 4);
	if(Noise3DTexPtr == nullptr){
		Debug::DisplayFatalErrorMessage("Memory Allocation Error", "Could not allocate memory for Perlin Noise texture!");
		return;
	}

	for(int f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= 0.5f){

		SetNoiseFrequency(frequency);
		ptr = Noise3DTexPtr;
		ni[0] = ni[1] = ni[2] = 0;

		inci = 1.0f / (noiseTextureSize / frequency);
		for(int i = 0; i < noiseTextureSize; ++i, ni[0] += inci){
			incj = 1.0f / (noiseTextureSize / frequency);

			for(int j = 0; j < noiseTextureSize; ++j, ni[1] += incj){
				inck = 1.0f / (noiseTextureSize / frequency);

				for(int k = 0; k < noiseTextureSize; ++k, ni[2] += inck, ptr += 4){
					*(ptr + inc) = (GLubyte)(((Noise3(ni) + 1.0f) * amp) * 128.0f);
				}
			}
		}
	}
}

void PerlinMaterial::InitNoise(){
	for(int i = 0; i < B; i++){
		p[i] = i;
		g1[i] = Random::Range(-1.0f, 1.0f);
		g2[i] = Vector2(Random::Range(-1.0f, 1.0f), Random::Range(-1.0f, 1.0f)).Normalized();
		g3[i] = Vector3(Random::Range(-1.0f, 1.0f), Random::Range(-1.0f, 1.0f), Random::Range(-1.0f, 1.0f)).Normalized();
	}

	for(int i = B; i > 0; i--){
		int k = p[i];
		int j = Random::Range(0, B);

		p[i] = p[j];
		p[j] = k;
	}

	for(int i = 0; i < B + 2; i++){
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		g2[B + i] = g2[i];
		g3[B + i] = g3[i];
	}
}

constexpr float PerlinMaterial::SCurve(float t_){
	return t_ * t_ * (3.0f - 2.0f * t_);
}

constexpr float PerlinMaterial::Lerp(float t_, float a_, float b_){
	return a_ + t_ * (b_ - a_);
}

const void PerlinMaterial::Setup(float& t_, float vec, int& b0, int& b1, float& r0, float& r1){
	static constexpr int N = 0x1000;

	t_ = vec + N;
	b0 = static_cast<int>(t_) & BM;
	b1 = (b0 + 1) & BM;
	r0 = t_ - static_cast<int>(t_);
	r1 = r0 - 1.0f;
}