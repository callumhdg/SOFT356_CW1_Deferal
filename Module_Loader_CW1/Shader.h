#ifndef __SHADER_H__
#define __SHADER_H__

#include"GL/glew.h"

#ifdef __cplusplus
extern "C" {
#endif


	typedef struct {
		GLenum       type;
		const char * filename;
		GLuint       shader;
	} 
	
	ShaderInfo;
	GLuint LoadShaders(ShaderInfo*);


#ifdef __cplusplus
};
#endif 

#endif 