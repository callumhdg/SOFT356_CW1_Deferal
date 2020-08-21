

#include"Shader.h"
#include<iostream>
#include<cstdlib>
#include"GL\glew.h"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

	static const GLchar* readInShader(const char* fileName) {

		FILE* file;
		fopen_s(&file, fileName, "rb");

		if (!file) {
			cerr << "File " << fileName << "is unavailable" << endl;
			return NULL;
		}

		fseek(file, 0, SEEK_END);//file stream, ofset, origin
		int len = ftell(file);
		fseek(file, 0, SEEK_SET);
		GLchar* source = new GLchar[len + 1];

		fread(source, 1, len, file);//buffer, element size, element count, file stream
		fclose(file);//close reader
		source[len] = 0;

		return const_cast <const GLchar*> (source);

	}


	GLuint LoadShaders(ShaderInfo* shaders)
	{
		if (shaders == NULL) { 
			return 0; 
		}

		GLuint program = glCreateProgram();

		ShaderInfo* entry = shaders;

		while (entry->type != GL_NONE) {

			GLuint shader = glCreateShader(entry->type);
			entry->shader = shader;

			const GLchar* source = readInShader(entry->filename);
			if (source == NULL) {

				for (entry = shaders; entry->type != GL_NONE; ++entry) {

					glDeleteShader(entry->shader);
					entry->shader = 0;

				}

				return 0;
			}

			glShaderSource(shader, 1, &source, NULL);
			delete[] source;

			glCompileShader(shader);

			GLint compiled;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

			if (!compiled) {

#ifdef _DEBUG

				GLsizei length;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
				
				GLchar* log = new GLchar[length + 1];
				glGetShaderInfoLog(shader, length, &length, log);
				cerr << "Shader compiling unsucsessful" << log << endl;
				delete[] log;

#endif // DEBUG 

				return 0;
			}
			
			glAttachShader(program, shader);

			entry++;
		}

		glLinkProgram(program);

		GLint linked;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);

		if (!linked) {

#ifdef _DEBUG

			GLsizei length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

			GLchar* log = new GLchar[length + 1];
			glGetProgramInfoLog(program, length, &length, log);
			cerr << "Shader linking unsucsessful " << log << endl;
			delete[] log;

#endif // DEBUG 

			for (entry = shaders; entry->type != GL_NONE; entry++) {
				glDeleteShader(entry->shader);
				entry->shader = 0;
			}

			return 0;
		}

		return program;
	}


	
#ifdef __cplusplus
}
#endif