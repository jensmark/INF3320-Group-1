#include "RayTracer.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <sys/stat.h>
#include <stdexcept>

#include "CubeMap.hpp"

RayTracer::RayTracer(unsigned int width, unsigned int height, int num_rays, float focus_length, float aperture_radius) {
	const glm::vec3 camera_position(0.0f, 0.0f, 10.0f);

	//Initialize framebuffer and virtual screen
	fb = new FrameBuffer(width, height);
	float aspect = width/static_cast<float>(height);
	screen.top = 1.0f;
	screen.bottom = -1.0f;
	screen.right = aspect;
	screen.left = -aspect;

	this->aperture_radius = aperture_radius;
	this->focus_length = focus_length;
	this->num_rays = num_rays;

	//Initialize state
	state = new RayTracerState(camera_position);
}

RayTracer::~RayTracer(){
	delete fb;
	delete state;
}

void RayTracer::addSceneObject(SceneObject* o) {
	state->getScene().push_back(o);
}

void RayTracer::render() {
	float offsets[4][2] = {
			{0.25, 0.25},
			{-0.25, -0.25},
			{-0.25, 0.25},
			{0.25, -0.25}};

	//For every pixel
	#pragma omp parallel for
	for (int j=0; j< (int)(fb->getHeight()); ++j) {
		for (unsigned int i=0; i<fb->getWidth(); ++i) {
			glm::vec3 out_color(0.0, 0.0, 0.0);
			float x, y, z;
			
			float focus = this->focus_length;
			float r = this->aperture_radius;

			unsigned int rays = this->num_rays;
			unsigned int rays_fired = 0;

			while(rays_fired < rays){
				float du = (r / RAND_MAX) * (1.0f * rand());
				float dv = (r / RAND_MAX) * (1.0f * rand());

				glm::vec3 c;

				// Shoot 4 rays pr pixel
				for(int k = 0; k < 4; k++){
					// Create the ray using the view screen definition 
					x = ((float)i + offsets[k][0])*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
					y = ((float)j + offsets[k][1])*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
					z = -1.0f;

					glm::vec3 direction = glm::vec3(-x, -y, -z);
					glm::vec3 start = state->getCamPos() + direction;
					glm::vec3 aimed = start + focus * glm::vec3(x, y, z);

					glm::vec3 start0 = start + glm::vec3(du, dv, 0.0f);

					Ray r = Ray(start0, aimed - start0);
					c += state->rayTrace(r);
				}
				
				c *= 0.25f;
				out_color += c;
				rays_fired++;
			}
			
			out_color /= (float)rays_fired;
			fb->setPixel(i, j, out_color);
		}
	}
}

void RayTracer::save(std::string basename) {
	
	struct stat buffer;
	int i;
	std::stringstream filename;

	//Find a unique filename...
	for (i=0; i<10000; ++i) {
		filename.str("");
		filename << basename << std::setw(4) << std::setfill('0') << i << "." << "tga";
		if (stat(filename.str().c_str(), &buffer) != 0) break;
	}

	if (i == 10000) {
		std::stringstream log;
		log << "Unable to find unique filename for " << basename << "%d." << "tga";
		throw std::runtime_error(log.str());
	}
	
	std::vector<unsigned char> copy;
	copy.resize(fb->getWidth()* fb->getHeight()* 3);
	for(size_t i = 0; i < copy.size(); i++){
		copy[i] = (unsigned char)(fb->getData()[i] * 255.0f);
	}
	tgaInfo* img = tgaCreate(fb->getWidth(), fb->getHeight(), 3, copy.data());
	tgaSave(filename.str().c_str(), img);
	tgaDestroy(img);
}
