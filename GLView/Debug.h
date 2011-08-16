/*
     File: Debug.h
 Abstract: 
  Version: 1.2

 
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <assert.h>
#include <stdio.h>

// These functions should only be used in Debug builds, because querying GL state affects
// performance. To use them, define ASSERT and DEBUG in your project's Debug configuration
// (i.e. with Xcode's GCC_PREPROCESSOR_DEFINITIONS Build setting.)
// By default, ASSERT and DEBUG are not defined, so these functions compile to no-ops.

// Run-time assertion
#if ASSERT
#define rt_assert(expression) assert(expression)
#else
#define rt_assert(expression)
#endif

// Catch run-time GL errors
#if DEBUG
#define glCheckError() { \
	GLenum err = glGetError(); \
	if (err != GL_NO_ERROR) { \
		fprintf(stderr, "glCheckError: %04x caught at %s:%u\n", err, __FILE__, __LINE__); \
		rt_assert(0); \
	} \
}
#else
#define glCheckError()
#endif


// Debug utilities
void validateTexEnv(void);

#endif /* DEBUG_H */
