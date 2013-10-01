/* $Id: NoiseSampler.h,v 1.5 2011/09/29$
 *
 * Author: 2005 Christopher Dyken, <dyken@cma.uio.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 * 
 * Distributed under the GNU GPL.
 * 
 * 
 * Inspired by the implementation as given by Peachey in [1], page 67.
 *   
 * [1] Ebert, D. S., Musgrave, F. K., Peachey, D., Perlin, K., Worley, S.,
 *     Texturing and Modeling, second edition, Academic Press, 1998.
 * 
 * 
 * NOISE AND TURBULENCE
 * --------------------
 * 
 * A problem with defining textures procedurally is that simply using
 * regular mathematical expressions yields very clean and structured
 * textures, and this poses a problem when we want an ``organic'' look.
 * Noise and turbulence are two fractal-like functions which can be used
 * to overcome this.
 * 
 * The noise used in texturing is a quasi-noise; it is not a real noise
 * function, since in texturing, we want a noise function that is
 * band-limited, has limited amplitude, and reproducibility. Real noise
 * doesn't have these properties.
 * 
 * Also, we want the noise to be stationary and isotropic such that the
 * character of the noise doesn't change with direction and position.
 * 
 * Finally, to avoid structural artifacts, we want the noise to
 * have a minimum amount of periodicity.
 * 
 * We will now look at how a 1D noise is made.
 * 
 * We use an array of random numbers to define the ``randomness'' of the
 * noise function, that is, let V be a sequence of N random numbers between
 * -1 and 1, V={ v_0, v_1, .., v_{N-1} }. Thus, we have associated each
 * integer in [0, N-1] with a random number. We extend this to all integers
 * by defining v'_i to be
 *
 *                           v'_i = v_{i%N},
 * 
 * where % is the modulo-operator. That is, the sequence of random numbers
 * repeats itself for each N-th integer such that v'_0 = v'_N = v'_{2N}= ...
 * 
 * The noise function is a sum over several layers, called octaves. Each
 * octave have most of its energy in a very small frequency band, and by
 * selecting which octaves to include, we can band-limit the noise function.
 * 
 * We use v'_0 to define the octave function over the integers, and we use
 * an interpolating function to define the octave function between the
 * integers. We want the octave function to be at least continuous, and
 * maybe a few continuous derivatives as well. However, for simplicity, we use
 * linear interpolation, called 'lerp' in gfx-lingo,
 * 
 *                      lerp(a,b,t) = (1-t)a + tb
 * 
 * even though it has discontinuities in the first derivative. A cubic
 * polynomial could be a better choice since it has continuity in both
 * first and second derivative, and thus, is smoother and more
 * band-limited.
 * 
 * Thus, given a real number $x$, the octave function is defined as
 * 
 *        octave(x) = lerp(v'_{floor(x)}, v'_{ceil(x)}, x-floor(x))
 * 
 * where floor returns the largest integer smaller than x, and similarily,
 * ceil returns the smnallest integer larger than x.
 * 
 * The noise function, as mentioned earlier, is a sum of octaves. We
 * scale each octave by 1/f, where f is the peak frequency of the
 * octave. We start with f=1 and double the frequency for each octave
 * included, until g, which is the max frequency, that is,
 * 
 *         noise(x) = sum_{f=1,2,4,\dots,g} 1/f*octave(f*x)
 * 
 * A variation of the noise function is the turbulence function. It has
 * the same anatomy as the noise function, but instead of summing the
 * octave, we sum the absolute value of each octave,
 * 
 *      turbulence(x) = sum_{f=1,2,4,\dots,g} 1/f*abs(octave(f*x))
 * 
 * We see that this function never becomes negative, and the amplitude is
 * usually within $[0,2]$. The absolute values introduce some
 * discontinuities in the first-derivative, which results in a more
 * ``cloud-like'' function.
 * 
 */

#ifndef NOISESAMPLER_H_
#define NOISESAMPLER_H_

#include <glm/glm.hpp>

namespace GfxUtil {

/** Implementation of R^3 Perlin noise and turbulence. */
class NoiseSampler {
 public:
  NoiseSampler();

  /** Returns the fractal sum at p. */
  float fractalSum(const glm::vec3& p);

  /** Returns the turbulence at p. */
  float turbulence(const glm::vec3& p);

 protected:
  /** Sets up the permutation table. */
  void initializePermutationTable();

  /** Sets up the value table. */
  void initializeValueTable();

  /** Calculates one octave of noise, tri-cubic interpolation of lattice values. */
  float vnoise(const glm::vec3& p);

  /** 1D Catmull-Rom interpolation of four values. */
  float catmull_rom4(float x, float *p);

  static bool           m_is_initialized; ///< Static initialization flag.
  static unsigned char* m_perm_table;     ///< Permutation table.
  static float*         m_value_table;    ///< Value table
};

}  // namespace GfxUtil

#endif  // NOISESAMPLER_H_
