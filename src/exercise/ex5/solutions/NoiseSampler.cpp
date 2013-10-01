/* $Id: NoiseSampler.cpp,v 1.5 2011/09/29$
 * 
 * Author: 2005 Christopher Dyken, <dyken@cma.uio.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 * 
 * Distributed under the GNU GPL.
 * 
 * See NoiseSampler.h for details
 * 
 */

#include <cmath>
#include <float.h>
#include <cstdlib>

#include "NoiseSampler.hpp"

// TABLE_SIZE must be a power of two
#define TABLE_SIZE 256

// Minimum and maximum frequency of the spectral synthesis. For simplicity
// a preprocessor constant is used, however, these numbers should depend
// on sampling frequency.
#define MIN_FREQ 1.0f
#define MAX_FREQ 16.0f

// Random number seeds
#define PERM_TABLE_SEED  1
#define VALUE_TABLE_SEED 2

// Misc. macros
#define TABLE_MASK (TABLE_SIZE-1)
#define PERM(x) (m_perm_table[(x)&TABLE_MASK])
#define INDEX_3(ix,iy,iz) (PERM((ix)+PERM((iy)+PERM(iz))))
#define FLOOR(x) ((int)(x)-((x)<0&&(x)!=(int)(x)))
#define CEIL(x) ((int)(x)+((x)>0&&(x)!=(int)(x)))

namespace GfxUtil {

/* class-static variables. */
bool           NoiseSampler::m_is_initialized = false;
unsigned char* NoiseSampler::m_perm_table = NULL;
float*         NoiseSampler::m_value_table = NULL;

NoiseSampler::NoiseSampler() {
  // Static initialization
  if (!m_is_initialized) {
    initializePermutationTable();
    initializeValueTable();
    m_is_initialized = true;
  }
}

void NoiseSampler::initializePermutationTable() {
  m_perm_table = new unsigned char[TABLE_SIZE];
  for (int i = 0; i < TABLE_SIZE; i++) {
    m_perm_table[i] = i;
  }
  srand(PERM_TABLE_SEED);
  for (int i = 0; i < TABLE_SIZE; i++) {
    int j = i+(int)((double)(TABLE_SIZE-i-1.0)*rand()/(RAND_MAX+1.0));
    int t = m_perm_table[j];
    m_perm_table[j] = m_perm_table[i];
    m_perm_table[i] = t;
  }
}

void NoiseSampler::initializeValueTable() {
  m_value_table = new float[TABLE_SIZE];
  srand(VALUE_TABLE_SEED);
  for (int i = 0; i < TABLE_SIZE; i++) {
    m_value_table[i] = 1.0 - 2.0*rand()/(RAND_MAX+1.0);
  }
}

float NoiseSampler::catmull_rom4(float x, float *p) {
  float c3 = -0.5*p[0] +1.5*p[1] -1.5*p[2] +0.5*p[3];
  float c2 =  1.0*p[0] -2.5*p[1] +2.0*p[2] -0.5*p[3];
  float c1 = -0.5*p[0] +0.0*p[1] +0.5*p[2] +0.0*p[3];
  float c0 =  0.0*p[0] +1.0*p[1] +0.0*p[2] +0.0*p[3];
  return ((c3*x+c2)*x+c1)*x+c0;
}

float NoiseSampler::vnoise(const glm::vec3& p) {
  int ix = FLOOR(p[0]);
  int iy = FLOOR(p[1]);
  int iz = FLOOR(p[2]);
  float fy = p[1]-iy;
  float fx = p[0]-ix;
  float fz = p[2]-iz;

  float xp[4], yp[4], zp[4];

  for(int k = -1; k <= 2; k++) {
    for(int j = -1; j <= 2; j++) {
      for(int i = -1; i <= 2; i++) {
        xp[i+1] = m_value_table[INDEX_3(ix+i, iy+j, iz+k)];
      }
      yp[j+1] = catmull_rom4(fx, xp);
    }
    zp[k+1] = catmull_rom4(fy, yp);
  }
  return catmull_rom4(fz, zp);
}

float NoiseSampler::fractalSum(const glm::vec3& p) {
  float v = 0.0f;
  for(float f = MIN_FREQ; f < MAX_FREQ; f *= 1.97f) {
    v += (1.0/f)* vnoise(f*p);
  }
  return v;
}

float NoiseSampler::turbulence(const glm::vec3& p) {
  float v = 0.0f;
  for(float f = MIN_FREQ; f < MAX_FREQ; f *= 1.97f) {
    float t = (1.0/f)* vnoise(f*p);
    v+= t<0.0 ? -t : t;
  }
  return v;
}

} // namespace GfxUtil
