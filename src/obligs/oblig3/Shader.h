#ifndef SHADER_H_
#define SHADER_H_

#include <string>
#include <map>

#include <GL/glew.h>

namespace GfxUtil {

class Shader {
 public:
  // Constructors
  Shader();
  
  // Destructor
  ~Shader();
  
  // Member functions
  bool CompileShader(GLenum shader_type, const std::string source);
  std::string GetCompileLog(GLenum shader_type);
  bool LinkProgram(std::string fragment_output);
  bool LinkProgram();
  std::string GetLinkLog();
  void Activate();
  void Deactivate();
  GLuint GetAttribute(const std::string attribute);
  GLuint GetUniform(const std::string uniform);
  std::string DumpAttributes();
  std::string DumpUniforms();
  
 private:
  // Member functions
  void BuildAttributeMap();
  void BuildUniformLocationMap();
  
  // Members
  GLuint m_program_id_;
  std::map<GLenum, GLuint> m_shaders_ids_;
  std::map<std::string, GLuint> m_attribute_map_;
  std::map<std::string, GLuint> m_uniform_map_;
};  

}  // GfxUtil

#endif  // SHADER_H_
