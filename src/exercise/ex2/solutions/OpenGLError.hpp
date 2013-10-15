/* $Id: OpenGLError.hpp, v1.2 2011/09/05$
 *
 * Author: Atgeirr F Rasmussen, <atgeirr@sintef.no>
 * Reviewed by: Bartlomiej Siwek, <bartloms@ifi.uio.no>
 *
 * Distributed under the GNU GPL.
 */

#ifndef OPENGL_ERROR_H_
#define OPENGL_ERROR_H_

#include <stdexcept>
#include <sstream>
#include <string>

#include <GL/glut.h>

// Exception class for OpenGL errors.
class opengl_error : public std::runtime_error {
 public:
  opengl_error(GLenum error, const char* file = "", int line = 0)
      : std::runtime_error(getErrorMessage(error, file, line)) {
  }

 protected:
  static std::string getErrorMessage(GLenum error, const char* file, int line) {
    const char *str_enum = "Unknown error";
    const char *str_desc = "None";
    const char *str_hint = "None";

    switch(error) {
      case GL_INVALID_ENUM:
        str_enum = "GL_INVALID_ENUM";
        str_desc = "enum argument out of range. Offending command ignored.";
        str_hint = "You probably tried to pass an enum value that is not "
                   "specified as allowed for that command. ";
        break;
      case GL_INVALID_VALUE:
        str_enum = "GL_INVALID_VALUE";
        str_desc = "Numeric argument out of range. Offending command ignored.";
        str_hint = "E.g. this error is generated when you pass a negative "
                   "value to a command that takes a sizei as parameter.";
        break;
      case GL_INVALID_OPERATION:
        str_enum = "GL_INVALID_OPERATION";
        str_desc = "Operation is illegal in current state. Offending command ignored.";
        str_hint = "Some commands are not allowed between glBegin()/glEnd() pairs.";
        break;
      case GL_STACK_OVERFLOW:
        str_enum = "GL_STACK_OVERFLOW";
        str_desc = "Command would cause a stack overflow. Offending command ignored.";
        str_hint = "Calling glPushMatrix()/glPushAttrib()/glPushClientAttrib() "
                   "to a full matrix causes this error.";
        break;
      case GL_STACK_UNDERFLOW:
        str_enum = "GL_STACK_UNDERFLOW";
        str_desc = "Command would cause a stack underflow. Offending command ignored.";
        str_hint = "Calling glPopMatrix()/glPopAttrib()/glPopClientAttrib() "
                   "to an empty matrix causes this error.";
        break;
      case GL_OUT_OF_MEMORY:
        str_enum = "GL_OUT_OF_MEMORY";
        str_desc = "Not enough memory left to execute command.";
        str_hint = "If memory is exhausted as a side-effect of the execution of "
                   "a command, this error may be generated. I.e., it is not definite "
                   "whether or not the command was ignored or not.";
        break;
      case GL_TABLE_TOO_LARGE:
        str_enum = "GL_TABLE_TOO_LARGE";
        str_desc = "The specified table is too large. Offending command ignored.";
        break;
    }
    
    std::ostringstream out;
    out << str_enum;
    out << "\n** Where:       " << file << " at line " << line;
    out << "\n** Description: " << str_desc;
    out << "\n** Hint:        " << str_hint;
    
    return out.str();
  }
};

/** Macro that checks OpenGL state. If an error is present, it throws an exception.
 * 
 * You cannot check for OpenGL errors between glBegin()/glEnd()-pairs, if you do,
 * you get an GL_INVALID_OPERATION error.
 * 
 * If using g++, you can add a verbose termiate handler that prints out the exception info:
 * int
 * main(int argc, char **argv)
 * {
 *     std::set_terminate(__gnu_cxx::__verbose_terminate_handler);
 *     // ...
 *     return 0;
 * }
 */
#define CHECK_OPENGL \
do { \
  GLenum error = glGetError(); \
  if(error != GL_NO_ERROR) \
    throw opengl_error(error, __FILE__, __LINE__); \
} while (0)

#endif  // OPENGL_ERROR_H_
