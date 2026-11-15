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

#endif /* __freak_noise_h_ */
