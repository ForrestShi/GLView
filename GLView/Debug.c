/*
     File: Debug.c
 Abstract: 
  Version: 1.2
 
 
 */

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include "Debug.h"


// Debug utility: validate the current texture environment against MBX errata.
// Assert if we use a state combination not supported by the MBX hardware.
void validateTexEnv(void)
{
	#if DEBUG
	typedef struct {
		GLint combine;
		GLint src[3];
		GLint op[3];
		GLint scale;
	} Channel;

	typedef struct {
		GLint enabled;
		GLint binding;
		GLint mode;
		Channel rgb;
		Channel a;
		GLfloat color[4];
	} TexEnv;

	// MBX supports two texture units
	TexEnv unit[2];
	GLint active;
	int i, prev = -1;

	glGetIntegerv(GL_ACTIVE_TEXTURE, &active);
	for (i = 0; i < 2; i++)
	{
		glActiveTexture(GL_TEXTURE0+i);
		unit[i].enabled = glIsEnabled(GL_TEXTURE_2D);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &unit[i].binding);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  &unit[i].mode);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_COMBINE_RGB,       &unit[i].rgb.combine);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_SRC0_RGB,          &unit[i].rgb.src[0]);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_SRC1_RGB,          &unit[i].rgb.src[1]);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_SRC2_RGB,          &unit[i].rgb.src[2]);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_OPERAND0_RGB,      &unit[i].rgb.op[0]);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_OPERAND1_RGB,      &unit[i].rgb.op[1]);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_OPERAND2_RGB,      &unit[i].rgb.op[2]);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_RGB_SCALE,         &unit[i].rgb.scale);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_COMBINE_ALPHA,     &unit[i].a.combine);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_SRC0_ALPHA,        &unit[i].a.src[0]);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_SRC1_ALPHA,        &unit[i].a.src[1]);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_SRC2_ALPHA,        &unit[i].a.src[2]);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA,    &unit[i].a.op[0]);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA,    &unit[i].a.op[1]);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA,    &unit[i].a.op[2]);
		glGetTexEnviv(GL_TEXTURE_ENV, GL_ALPHA_SCALE,       &unit[i].a.scale);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, &unit[i].color[0]);

		if (unit[i].enabled == 0) continue;
		if (unit[i].mode != GL_COMBINE) continue;

		// PREVIOUS on unit 0 means PRIMARY_COLOR.
		if (i == 0)
		{
			int j;

			for (j = 0; j < 3; j++)
			{
				if (unit[i].rgb.src[j] == GL_PREVIOUS)
					unit[i].rgb.src[j] = GL_PRIMARY_COLOR;
				if (unit[i].a.src[j] == GL_PREVIOUS)
					unit[i].a.src[j] = GL_PRIMARY_COLOR;
			}
		}

		// If the value of COMBINE_RGB is MODULATE, only one of the two multiplicands can use an ALPHA operand.
		rt_assert(!(unit[i].rgb.combine == GL_MODULATE &&
		           (unit[i].rgb.op[0] == GL_SRC_ALPHA || unit[i].rgb.op[0] == GL_ONE_MINUS_SRC_ALPHA) &&
		           (unit[i].rgb.op[1] == GL_SRC_ALPHA || unit[i].rgb.op[1] == GL_ONE_MINUS_SRC_ALPHA)));

		// If the value of COMBINE_RGB is INTERPOLATE and either SRC0 or SRC1 uses an ALPHA operand, SRC2 can not be CONSTANT or PRIMARY_COLOR or use an ALPHA operand.
		rt_assert(!(unit[i].rgb.combine == GL_INTERPOLATE &&
		           (unit[i].rgb.op[0] == GL_SRC_ALPHA || unit[i].rgb.op[0] == GL_ONE_MINUS_SRC_ALPHA ||
		            unit[i].rgb.op[1] == GL_SRC_ALPHA || unit[i].rgb.op[1] == GL_ONE_MINUS_SRC_ALPHA) &&
		           (unit[i].rgb.op[2] == GL_SRC_ALPHA || unit[i].rgb.op[2] == GL_ONE_MINUS_SRC_ALPHA ||
		            unit[i].rgb.src[2] == GL_CONSTANT || unit[i].rgb.src[2] == GL_PRIMARY_COLOR)));

		// If the value of COMBINE_RGB is INTERPOLATE and SRC0 and SRC1 are CONSTANT or PRIMARY COLOR, SRC2 can not be CONSTANT or PRIMARY_COLOR or use an ALPHA operand.
		rt_assert(!(unit[i].rgb.combine == GL_INTERPOLATE &&
		          ((unit[i].rgb.src[0] == GL_CONSTANT      && unit[i].rgb.src[1] == GL_CONSTANT) ||
		           (unit[i].rgb.src[0] == GL_PRIMARY_COLOR && unit[i].rgb.src[1] == GL_PRIMARY_COLOR)) &&
		           (unit[i].rgb.op[2] == GL_SRC_ALPHA || unit[i].rgb.op[2] == GL_ONE_MINUS_SRC_ALPHA ||
		            unit[i].rgb.src[2] == GL_CONSTANT || unit[i].rgb.src[2] == GL_PRIMARY_COLOR)));

		// If the value of COMBINE_RGB is DOT3_RGB or DOT3_RGBA, only one of the sources can be PRIMARY_COLOR or use an ALPHA operand.
		rt_assert(!((unit[i].rgb.combine == GL_DOT3_RGB || unit[i].rgb.combine == GL_DOT3_RGBA) &&
		            (unit[i].rgb.src[0] == GL_PRIMARY_COLOR || unit[i].rgb.op[0] == GL_SRC_ALPHA || unit[i].rgb.op[0] == GL_ONE_MINUS_SRC_ALPHA) &&
		            (unit[i].rgb.src[1] == GL_PRIMARY_COLOR || unit[i].rgb.op[1] == GL_SRC_ALPHA || unit[i].rgb.op[1] == GL_ONE_MINUS_SRC_ALPHA)));

		// If the value of COMBINE_RGB is SUBTRACT, SCALE_RGB must be 1.0.
		rt_assert(!(unit[i].rgb.combine == GL_SUBTRACT && unit[i].rgb.scale != 1));

		if (unit[i].rgb.combine != GL_DOT3_RGBA)
		{
			// If the value of COMBINE_ALPHA is MODULATE or INTERPOLATE, only one of the two multiplicands can be CONSTANT.
			rt_assert(!(unit[i].a.combine == GL_MODULATE    &&  unit[i].a.src[0] == GL_CONSTANT && unit[i].a.src[1] == GL_CONSTANT));
			rt_assert(!(unit[i].a.combine == GL_INTERPOLATE && (unit[i].a.src[0] == GL_CONSTANT || unit[i].a.src[1] == GL_CONSTANT) && unit[i].a.src[2] == GL_CONSTANT));
	
			// If the value of COMBINE_ALPHA is SUBTRACT, SCALE_ALPHA must be 1.0.
			rt_assert(!(unit[i].a.combine == GL_SUBTRACT && unit[i].a.scale != 1));
		}
		
		// The value of TEXTURE_ENV_COLOR must be the same for all texture units that CONSTANT is used on.
		if (unit[i].rgb.src[0] == GL_CONSTANT ||
		   (unit[i].rgb.src[1] == GL_CONSTANT && unit[i].rgb.combine != GL_REPLACE) ||
		   (unit[i].rgb.src[2] == GL_CONSTANT && unit[i].rgb.combine == GL_INTERPOLATE) ||
		   (unit[i].rgb.combine != GL_DOT3_RGBA &&
		   (unit[i].a.src[0] == GL_CONSTANT ||
		   (unit[i].a.src[1] == GL_CONSTANT && unit[i].a.combine != GL_REPLACE) ||
		   (unit[i].a.src[2] == GL_CONSTANT && unit[i].a.combine == GL_INTERPOLATE))))
		{
			if (prev >= 0)
				rt_assert(!(unit[prev].color[0] != unit[i].color[0] ||
							unit[prev].color[1] != unit[i].color[1] ||
							unit[prev].color[2] != unit[i].color[2] ||
							unit[prev].color[3] != unit[i].color[3]));
			prev = i;
		}
	}

	glActiveTexture(active);
	glCheckError();
	#endif /* DEBUG */
}
