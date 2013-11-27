#include <iostream>
#include <string>

#include "RayTracer.h"
#include "Sphere.hpp"
#include "CubeMap.hpp"

/**
 * Simple program that starts our game manager
 */
int main(int argc, char *argv[]) {
	try {
		RayTracer* rt = new RayTracer(800, 600, 100, 7.0f, 0.003f);

		// materials
		const float eta_air = 1.000293f;
		const float eta_carbondioxide = 1.00045f;
		const float eta_water = 1.3330f;
		const float eta_ethanol = 1.361f;
		const float eta_pyrex = 1.470f;
		const float eta_diamond = 2.419f;

		SceneObjectEffect* color = new ColorEffect(glm::vec3(0.0, 1.0, 0.0));
		SceneObjectEffect* reflective = new ReflectiveEffect();
		SceneObjectEffect* fresnel = new FresnelEffect(eta_air, eta_diamond);
		
		SceneObject* s1 = new Sphere(glm::vec3(-3.0f, 0.0f, 6.0f), 2.0f, reflective);
		rt->addSceneObject(s1);
		SceneObject* s2 = new Sphere(glm::vec3(3.0f, 0.0f, 3.0f), 2.0f, fresnel);
		rt->addSceneObject(s2);
		SceneObject* s3 = new Sphere(glm::vec3(0.0f, 3.0f, 2.0f), 2.0f, fresnel);
		rt->addSceneObject(s3);

		SceneObject* s5 = new CubeMap(
			"cubemap/posx.tga", 
			"cubemap/negx.tga", 
			"cubemap/posy.tga", 
			"cubemap/negy.tga", 
			"cubemap/posz.tga", 
			"cubemap/negz.tga");
		rt->addSceneObject(s5);

		rt->render();
		std::cout << "Image rendered" << std::endl;
		
		rt->save("test");
		std::cout << "Image saved" << std::endl;

		delete rt;
		delete color;
		delete reflective;
	} catch (std::exception &e) {
		std::string err = e.what();
		std::cout << err.c_str() << std::endl;
		return -1;
	}
	return 0;
}
