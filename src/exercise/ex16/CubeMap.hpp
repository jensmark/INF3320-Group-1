#ifndef _CUBEMAP_HPP__
#define _CUBEMAP_HPP__

#include <string>
#include <limits>
#include <sstream>
#include <stdexcept>

#include <glm/glm.hpp>

#include "TGALoader.h"

class CubeMap : public SceneObject {
public:
	CubeMap(std::string posx, std::string negx, 
			std::string posy, std::string negy,
			std::string posz, std::string negz) {
		loadImage(posx, this->posx);
		loadImage(negx, this->negx);
		loadImage(posy, this->posy);
		loadImage(negy, this->negy);
		loadImage(posz, this->posz);
		loadImage(negz, this->negz);
	}
	
	/**
	  * Ray-trace function that returns what texel you hit in the
	  * cube map, since any ray will hit some point in the cube map
	  */
	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		glm::vec3 out_color(1.0f);
		glm::vec3 contact = ray.getDirection();

		float s_uv = 0.0f;
		float t_uv = 0.0f;

		if(glm::abs(contact.x) >= glm::abs(contact.y) &&
			glm::abs(contact.x) >= glm::abs(contact.z)){

			if(contact.x > 0.0f){
				s_uv = 1.0f - (contact.z / contact.x+ 1.0f) * 0.5f;
				t_uv = 1.0f - (contact.y / contact.x + 1.0f) * 0.5f;

				out_color = readTexture(posx, s_uv, t_uv);
			}else if(contact.x < 0.0f){ 
				s_uv = 1.0f - (contact.z / contact.x+ 1.0f) * 0.5f;
				t_uv = (contact.y / contact.x+ 1.0f) * 0.5f;

				out_color = readTexture(negx, s_uv, t_uv);
			}
		}
		else if(glm::abs(contact.y) >= glm::abs(contact.x) &&
			glm::abs(contact.y) >= glm::abs(contact.z)){

			if(contact.y > 0.0f){
				s_uv = (contact.x / contact.y + 1.0f) * 0.5f;
				t_uv = (contact.z / contact.y + 1.0f) * 0.5f;

				out_color = readTexture(posy, s_uv, t_uv);
			}else if(contact.y < 0.0f){ 
				s_uv =  1.0f - (contact.x / contact.y + 1.0f) * 0.5f;
				t_uv = (contact.z/contact.y + 1.0f) * 0.5f;

				out_color = readTexture(negy, s_uv, t_uv);
			}
		}
		else if(glm::abs(contact.z) >= glm::abs(contact.x) &&
			glm::abs(contact.z) >= glm::abs(contact.y)){

			if(contact.z > 0.0f){
				s_uv = (contact.x / contact.z + 1.0f) * 0.5f;
				t_uv = 1.0f - (contact.y /contact.z+1.0f) * 0.5f;

				out_color = readTexture(posz, s_uv, t_uv);
			}else if(contact.z < 0.0f){ 
				s_uv = (contact.x / contact.z + 1.0f) * 0.5f;
				t_uv = (contact.y / contact.z + 1.0f) * 0.5f;
				
				out_color = readTexture(negz, s_uv, t_uv);
			}
		}

		
		return out_color;
	}
	
	/**
	  * A ray will always hit the cube map by definition, but the point of intersection
	  * is as far away as possible
	  */
	float intersect(const Ray& r) {
		return std::numeric_limits<float>::max();
	}

private:
	struct texture {
		std::vector<float> data;
		unsigned int width;
		unsigned int height;
	};

	/**
	  * Returns the texel at texture coordinate [s, t] in texture tex
	  */
	static glm::vec3 readTexture(texture& tex, float s, float t) {
		glm::vec3 out_color;

		float xf = std::min(s*tex.width, tex.width-1.0f);
		float yf = std::min(t*tex.height, tex.height-1.0f);

		unsigned int xm = static_cast<unsigned int>(xf);
		unsigned int ym = static_cast<unsigned int>(yf);

		unsigned int i0 = (ym*tex.width + xm)*3;

		for (int k=0; k<3; ++k) {
			float c0 = tex.data.at(i0+k);
			out_color[k] = c0;
		}

		return out_color;
	}

	/**
	  * Loads an image into memory from file
	  */
	static void loadImage(std::string filename, texture& tex) {
		tgaInfo* img = tgaLoad(filename.c_str()); 
		
		if(img->status != 0){
			std::stringstream log;
			log << "Error loading image: " << filename;
			throw std::runtime_error(log.str());
		}
		
		tex.width = img->width;
		tex.height = img->height;
		tex.data.resize(tex.width*tex.height*3);
		
		for(size_t i =0; i < tex.data.size(); i++){
			tex.data[i] = (float)img->imageData[i] /255.0f;
		}
		
		tgaDestroy(img);		
	}

	texture posx, negx, posy, negy, posz, negz;
};

#endif
