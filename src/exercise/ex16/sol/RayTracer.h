#ifndef _RAYTRACER_H__
#define _RAYTRACER_H__

#include <memory>
#include <string>
#include <vector>

#include "FrameBuffer.hpp"
#include "SceneObject.hpp"
#include "RayTracerState.hpp"

/**
  * The RayTracer class is the main entry point for raytracing
  * our scene, saving results to file, etc.
  */
class RayTracer {
public:
	RayTracer(unsigned int width, unsigned int height, int num_rays, float focus_length, float aperture_radius);
	~RayTracer();
	
	/**
	  * Adds an object to the scene
	  */
	void addSceneObject(SceneObject* o);

	/**
	  * Renders the current scene
	  */
	void render();

	/**
	  * Saves the currently rendered frame as an image file
	  */
	void save(std::string basename);

private:
	FrameBuffer* fb;
	RayTracerState* state;

	float focus_length;
	float aperture_radius;
	int num_rays;
	/**
	  * Defines the virtual screen we project our rays through
	  */
	struct {
		float left;
		float right;
		float top;
		float bottom;
	} screen;
};

#endif
