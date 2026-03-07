#ifndef _CHOCOLATE_KEEN_VORTICONS_OPENGL_
#define _CHOCOLATE_KEEN_VORTICONS_OPENGL_

#include "chocolate-keen_config.h"

#ifdef _CHOCOLATE_KEEN_ENABLE_OPENGL_
#ifndef NO_SDL_GLEXT
#define NO_SDL_GLEXT
#endif
#include "SDL_opengl.h"
#ifdef WIN32
#include "../windows/include/GL/glext.h"
#else
#include "GL/glext.h"
#endif

/****** OpenGL functions retrieved dynamically if required ******/

// Common (OpenGL 1.1 / ES 2.0 and compatible)
typedef const GLubyte * (APIENTRY * glGetString_Func_T)(GLenum);
typedef void (APIENTRY * glClearColor_Func_T)(GLclampf, GLclampf, GLclampf, GLclampf);
typedef void (APIENTRY * glViewport_Func_T)(GLint, GLint, GLsizei, GLsizei);
typedef void (APIENTRY * glGetIntegerv_Func_T)(GLenum, GLint*);
typedef void (APIENTRY * glEnable_Func_T)(GLenum);
typedef void (APIENTRY * glGenTextures_Func_T)(GLsizei, GLuint*);
typedef void (APIENTRY * glBindTexture_Func_T)(GLenum, GLuint);
typedef void (APIENTRY * glTexParameteri_Func_T)(GLenum, GLenum, GLint);
typedef void (APIENTRY * glTexImage2D_Func_T)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRY * glClear_Func_T)(GLbitfield);
typedef void (APIENTRY * glDisable_Func_T)(GLenum);

typedef void (APIENTRY * glDeleteTextures_Func_T)(GLsizei, GLuint*);

typedef void (APIENTRY * glTexSubImage2D_Func_T)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
typedef void (APIENTRY * glCopyTexSubImage2D_Func_T)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
typedef void (APIENTRY * glScissor_Func_T)(GLint, GLint, GLsizei, GLsizei);

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
// OpenGL 1.1 (fixed pipeline, not available in ES 2.0)
typedef void (APIENTRY * glMatrixMode_Func_T)(GLenum);
typedef void (APIENTRY * glLoadIdentity_Func_T)(void);
//typedef void (APIENTRY * glScalef_Func_T)(GLfloat, GLfloat, GLfloat);
typedef void (APIENTRY * glOrtho_Func_T)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
typedef GLuint (APIENTRY * glGenLists_Func_T)(GLsizei);
typedef void (APIENTRY * glNewList_Func_T)(GLuint, GLenum);
typedef void (APIENTRY * glBegin_Func_T)(GLenum);
typedef void (APIENTRY * glTexCoord2f_Func_T)(GLfloat, GLfloat);
typedef void (APIENTRY * glVertex3f_Func_T)(GLfloat, GLfloat, GLfloat);
typedef void (APIENTRY * glEnd_Func_T)(void);
typedef void (APIENTRY * glEndList_Func_T)(void);
typedef void (APIENTRY * glPushAttrib_Func_T)(GLbitfield);
typedef void (APIENTRY * glPopAttrib_Func_T)(void);
typedef void (APIENTRY * glDeleteLists_Func_T)(GLuint, GLsizei);
typedef void (APIENTRY * glCallList_Func_T)(GLuint);

// OpenGL 1.1 (misc. functions not available in ES 2.0)
//typedef void (APIENTRY * glDrawBuffer_Func_T) (GLenum);
//typedef void (APIENTRY * glReadBuffer_Func_T) (GLenum);
#endif

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
// OpenGL 1.1 / ES 2.0 (used for GL 2.0 renderer only)
typedef void (APIENTRY * glDrawElements_Func_T)(GLenum, GLsizei, GLenum, GLvoid*);

// OpenGL 1.3 / ES 2.0 (used for palette cycling, GL 2.0 renderer only)
typedef void (APIENTRY * glActiveTexture_Func_T) (GLenum);

// OpenGL 2.0 / ES 2.0 (shader related functions)
typedef GLuint (APIENTRY * glCreateShader_Func_T)(GLenum);
typedef void (APIENTRY * glShaderSource_Func_T)(GLuint, GLsizei, const GLchar*const*, const GLint*);
typedef void (APIENTRY * glCompileShader_Func_T)(GLuint);
typedef void (APIENTRY * glGetShaderiv_Func_T)(GLuint, GLenum, GLint*);
typedef void (APIENTRY * glGetShaderInfoLog_Func_T)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRY * glDeleteShader_Func_T)(GLuint);

typedef GLuint (APIENTRY * glCreateProgram_Func_T)(void);
typedef void (APIENTRY * glAttachShader_Func_T)(GLuint, GLuint);
typedef void (APIENTRY * glLinkProgram_Func_T)(GLuint);
typedef void (APIENTRY * glGetProgramiv_Func_T)(GLuint, GLenum, GLint*);
typedef void (APIENTRY * glGetProgramInfoLog_Func_T)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void (APIENTRY * glDeleteProgram_Func_T)(GLuint);
typedef void (APIENTRY * glUseProgram_Func_T)(GLuint);

typedef GLint (APIENTRY * glGetAttribLocation_Func_T)(GLuint, const GLchar*);
typedef GLint (APIENTRY * glGetUniformLocation_Func_T)(GLuint, const GLchar*);

typedef void (APIENTRY * glVertexAttribPointer_Func_T)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);
typedef void (APIENTRY * glEnableVertexAttribArray_Func_T)(GLuint);
typedef void (APIENTRY * glUniform1i_Func_T)(GLint, GLint);
#endif

// OpenGL 2.0 (not available in ES 2.0)
//typedef void (APIENTRY *glDrawBuffers_Func_T) (GLsizei, const GLenum *);

// Framebuffer object (EXT_framebuffer_object, use core for ES 2.0)
typedef void (APIENTRY * glBindFramebufferEXT_Func_T) (GLenum, GLuint);
typedef void (APIENTRY * glDeleteFramebuffersEXT_Func_T) (GLsizei, const GLuint *);
typedef void (APIENTRY * glGenFramebuffersEXT_Func_T) (GLsizei, GLuint *);
typedef GLenum (APIENTRY * glCheckFramebufferStatusEXT_Func_T) (GLenum);
typedef void (APIENTRY *glFramebufferTexture2DEXT_Func_T) (GLenum, GLenum, GLenum, GLuint, GLint);

// Framebuffer blitting (EXT_framebuffer_blit, requires ES 3.0 or other ES extension)
typedef void (APIENTRY *glBlitFramebufferEXT_Func_T)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);

// Common (OpenGL 1.1 / ES 2.0 and compatible)
extern glGetString_Func_T engine_glGetString;
extern glClearColor_Func_T engine_glClearColor;
extern glViewport_Func_T engine_glViewport;
extern glGetIntegerv_Func_T engine_glGetIntegerv;
extern glEnable_Func_T engine_glEnable;
extern glGenTextures_Func_T engine_glGenTextures;
extern glBindTexture_Func_T engine_glBindTexture;
extern glTexParameteri_Func_T engine_glTexParameteri;
extern glTexImage2D_Func_T engine_glTexImage2D;
extern glClear_Func_T engine_glClear;
extern glDisable_Func_T engine_glDisable;

extern glDeleteTextures_Func_T engine_glDeleteTextures;

extern glTexSubImage2D_Func_T engine_glTexSubImage2D;
extern glCopyTexSubImage2D_Func_T engine_glCopyTexSubImage2D;
extern glScissor_Func_T engine_glScissor;

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_1_1_
// OpenGL 1.1 (fixed pipeline, not available in ES 2.0)
extern glMatrixMode_Func_T engine_glMatrixMode;
extern glLoadIdentity_Func_T engine_glLoadIdentity;
//extern glScalef_Func_T engine_glScalef;
extern glOrtho_Func_T engine_glOrtho;
extern glGenLists_Func_T engine_glGenLists;
extern glNewList_Func_T engine_glNewList;
extern glBegin_Func_T engine_glBegin;
extern glTexCoord2f_Func_T engine_glTexCoord2f;
extern glVertex3f_Func_T engine_glVertex3f;
extern glEnd_Func_T engine_glEnd;
extern glEndList_Func_T engine_glEndList;
extern glPushAttrib_Func_T engine_glPushAttrib;
extern glPopAttrib_Func_T engine_glPopAttrib;
extern glDeleteLists_Func_T engine_glDeleteLists;
extern glCallList_Func_T engine_glCallList;

// OpenGL 1.1 (misc. functions not available in ES 2.0)
//extern glDrawBuffer_Func_T engine_glDrawBuffer;
//extern glReadBuffer_Func_T engine_glReadBuffer;
#endif

#ifdef _CHOCOLATE_KEEN_HAVE_OPENGL_2_0_ANY_
// OpenGL 1.1 / ES 2.0 (used for GL 2.0 renderer only)
extern glDrawElements_Func_T engine_glDrawElements;

// OpenGL 1.3 / ES 2.0 (used for palette cycling, GL 2.0 renderer only)
extern glActiveTexture_Func_T engine_glActiveTexture;

// OpenGL 2.0 / ES 2.0 (shader related functions)
extern glCreateShader_Func_T engine_glCreateShader;
extern glShaderSource_Func_T engine_glShaderSource;
extern glCompileShader_Func_T engine_glCompileShader;
extern glGetShaderiv_Func_T engine_glGetShaderiv;
extern glGetShaderInfoLog_Func_T engine_glGetShaderInfoLog;
extern glDeleteShader_Func_T engine_glDeleteShader;

extern glCreateProgram_Func_T engine_glCreateProgram;
extern glAttachShader_Func_T engine_glAttachShader;
extern glLinkProgram_Func_T engine_glLinkProgram;
extern glGetProgramiv_Func_T engine_glGetProgramiv;
extern glGetProgramInfoLog_Func_T engine_glGetProgramInfoLog;
extern glDeleteProgram_Func_T engine_glDeleteProgram;
extern glUseProgram_Func_T engine_glUseProgram;

extern glGetAttribLocation_Func_T engine_glGetAttribLocation;
extern glGetUniformLocation_Func_T engine_glGetUniformLocation;

extern glVertexAttribPointer_Func_T engine_glVertexAttribPointer;
extern glEnableVertexAttribArray_Func_T engine_glEnableVertexAttribArray;
extern glUniform1i_Func_T engine_glUniform1i;
#endif

// OpenGL 2.0 (not available in ES 2.0)
//extern glDrawBuffers_Func_T engine_glDrawBuffers;

// Framebuffer object (EXT_framebuffer_object in 2.0, core in ES 2.0)
extern glBindFramebufferEXT_Func_T engine_glBindFramebuffer_CoreEXT;
extern glDeleteFramebuffersEXT_Func_T engine_glDeleteFramebuffers_CoreEXT;
extern glGenFramebuffersEXT_Func_T engine_glGenFramebuffers_CoreEXT;
extern glCheckFramebufferStatusEXT_Func_T engine_glCheckFramebufferStatus_CoreEXT;
extern glFramebufferTexture2DEXT_Func_T engine_glFramebufferTexture2D_CoreEXT;

// Core/EXT compatibility aliases used by shared framebuffer paths.
#define ENGINE_GL_FRAMEBUFFER_COREEXT 0x8D40
#define ENGINE_GL_READ_FRAMEBUFFER_COREEXT 0x8CA8
#define ENGINE_GL_DRAW_FRAMEBUFFER_COREEXT 0x8CA9
#define ENGINE_GL_COLOR_ATTACHMENT0_COREEXT 0x8CE0
#define ENGINE_GL_FRAMEBUFFER_COMPLETE_COREEXT 0x8CD5

// Framebuffer blitting (EXT_framebuffer_blit, requires ES 3.0 or other ES extension)
extern glBlitFramebufferEXT_Func_T engine_glBlitFramebufferEXT;

#endif // _CHOCOLATE_KEEN_ENABLE_OPENGL_

#endif // _CHOCOLATE_KEEN_VORTICONS_OPENGL_
