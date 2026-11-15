/******************************************************************************
 * Freak deterministic integer noise
 *
 * Derived from the small value-noise hash used by Phony, but expressed in
 * integer form so it is useful on the fixed-point DOS path.
 ******************************************************************************/

#ifndef __freak_noise_h_
#define __freak_noise_h_

/* Returns deterministic signed value noise in the approximate range
   -32768..32767. */
int freak_noise_2d(int x, int y);

/* A weighted 3x3 neighbourhood filter for less harsh procedural textures. */
int freak_noise_smooth_2d(int x, int y);

/* Smooth value noise sampled from a lattice whose cells are scale pixels
   wide. A scale of one is equivalent to the raw lattice noise. */
int freak_noise_value_2d(int x, int y, int scale);

/* Fractal Brownian motion: layered value noise. persistence is 0..256,
   where 128 halves the contribution of every successive octave. */
int freak_noise_fractal_2d(int x, int y, int scale, int octaves,
                           unsigned int persistence);

#endif /* __freak_noise_h_ */
