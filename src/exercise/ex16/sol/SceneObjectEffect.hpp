#ifndef SCENEOBJECTEFFECT_HPP__
#define SCENEOBJECTEFFECT_HPP__

#include "Ray.hpp"
#include "RayTracerState.hpp"

/**
  * Abstract class that defines what it means to be an effect for a scene object
  */
class SceneObjectEffect {
public:
	/**
	  * This function "shades" an intersection point between a scene object
	  * and a ray. It can also fire new rays etc.
	  */
	virtual glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) = 0;
private:
};

/**
  * The color effect simply colors any point with the same color
  */
class ColorEffect : public SceneObjectEffect {
public:
	ColorEffect(glm::vec3 color) {
		this->color = color;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		return color;
	}

private:
	glm::vec3 color;
};


class ReflectiveEffect : public SceneObjectEffect {
public:
	ReflectiveEffect() {}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		glm::vec3 out_color(0.0f);
		//skip	
		
		//Create the reflection vector and spawn a new ray in that direction.
		
		//You can use Ray r = ray.spawn(..) to spawn a new ray.
		//Then use color = state.rayTrace(r) to continue the raytracing 

		Ray r = ray.spawn(t, glm::reflect(ray.getDirection(),normal));
		out_color = state.rayTrace(r);		

		//unskip
		return out_color;
	}
};

class FresnelEffect : public SceneObjectEffect {
public:
	FresnelEffect(float eta0, float eta1) {
		this->eta0 = eta0;
		this->eta1 = eta1;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		glm::vec3 out_color(0.0f);
		
		glm::vec3 v = glm::normalize(ray.getDirection());
		glm::vec3 n = normal;

		float R0;
		float eta;
		float fresnel;
		glm::vec3 refr, refl;

		// check if we are leaving or entering a object
		if(glm::dot(v, n) < 0.0f){
			R0 = glm::pow((eta1 - eta0) / (eta1 + eta0), 2.0f);
			eta = eta0/eta1;
			fresnel = R0 + (1.0f - R0) * pow((1.0f-glm::dot(-v, n)), 5.0f);

			Ray r0 = ray.spawn(t-1e-6, glm::refract(v, n, eta));
			Ray r1 = ray.spawn(t+1e-6, glm::reflect(v, n));
			refr = state.rayTrace(r0);
			refl = state.rayTrace(r1);
		}else{
			R0 = glm::pow((eta0 - eta1) / (eta0 + eta1), 2.0f);
			eta = eta1/eta0;
			fresnel = R0 + (1.0f - R0) * pow((1.0f-glm::dot(v, n)), 5.0f);

			Ray r0 = ray.spawn(t-1e-6, glm::refract(-v, n, eta));
			Ray r1 = ray.spawn(t+1e-6, glm::reflect(-v, n));
			refr = state.rayTrace(r0);
			refl = state.rayTrace(r1);
		}
		
		// blend colors
		out_color = glm::mix(refr, refl, fresnel);
		return out_color;
	}
private:
	float eta0, eta1; //< materials
};


#endif
