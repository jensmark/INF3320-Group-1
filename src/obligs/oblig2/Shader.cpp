#include "Shader.h"

#include <stdexcept>
#include <fstream>
#include <sstream>

namespace GfxUtil {

Shader::Shader() : m_program_id_(0), m_shaders_ids_(), m_attribute_map_(), m_uniform_map_() {
}

Shader::~Shader() {
  m_attribute_map_.clear();  
  m_uniform_map_.clear();
  
  std::map<GLenum, GLuint>::iterator it = m_shaders_ids_.begin();
  std::map<GLenum, GLuint>::iterator end = m_shaders_ids_.end();
  while(it != end) {
    glDeleteShader(it->second);
    ++it;
  }
  m_shaders_ids_.clear();
  
  if(m_program_id_ != 0) {
    glDeleteProgram(m_program_id_);
  }
}

bool Shader::CompileShader(GLenum shader_type, const std::string source) {  
  // Check if don't have this type of shader already - if so delete it
  if(m_shaders_ids_[shader_type] != 0) {
    glDeleteShader(m_shaders_ids_[shader_type]);
  }
  
  // Create new shader and store its id
  GLuint shader = glCreateShader(shader_type);
  m_shaders_ids_[shader_type] = shader;
  
  // Provide source
  const char *source_ptr = source.c_str();
  glShaderSource(shader, 1, &source_ptr, NULL);
  
  // Compile
  glCompileShader(shader);
  
  // Check compilation results
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    // Signal the failure
    return false;
  }
  
  // Signal the success
  return true;
}

std::string Shader::GetCompileLog(GLenum shader_type) {
  // Get shader id
  GLuint shader = m_shaders_ids_[shader_type];
  
  // Get the log length
  GLint infoLogLength;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
  
  // Get the log itself
  GLchar *infoLog = new GLchar[infoLogLength];
  glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
 
  // Compose the output
  std::stringstream ss;
  ss << "Compile log: "<< std::endl << infoLog << std::endl;
  
  // Clean up
  delete [] infoLog;
  
  // Return result
  return ss.str();
}

bool Shader::LinkProgram(std::string fragment_output) {
  // Create program (only if we haven't got one)
  if(m_program_id_ == 0) {
    m_program_id_ = glCreateProgram();
  }
  
  // Attach shaders
  if (m_shaders_ids_[GL_VERTEX_SHADER] != 0) {
    glAttachShader(m_program_id_, m_shaders_ids_[GL_VERTEX_SHADER]);
  }
  if (m_shaders_ids_[GL_FRAGMENT_SHADER] != 0) {
    glAttachShader(m_program_id_, m_shaders_ids_[GL_FRAGMENT_SHADER]);
  }
  
  // Specify output variable
  glBindFragDataLocation(m_program_id_, 0, fragment_output.c_str());
  
  // Link
  glLinkProgram(m_program_id_);
  
  // Check link results
  GLint status;
  glGetProgramiv(m_program_id_, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {   
    // Signal the failure
    return false;
  }
  
  // Build attribute and uniform maps
  BuildAttributeMap();
  BuildUniformLocationMap();
  
  // Signal the success
  return true;
}

bool Shader::LinkProgram() {
  // Create program (only if we haven't got one)
  if(m_program_id_ == 0) {
    m_program_id_ = glCreateProgram();
  }
  
  // Attach shaders
  if (m_shaders_ids_[GL_VERTEX_SHADER] != 0) {
    glAttachShader(m_program_id_, m_shaders_ids_[GL_VERTEX_SHADER]);
  }
  if (m_shaders_ids_[GL_FRAGMENT_SHADER] != 0) {
    glAttachShader(m_program_id_, m_shaders_ids_[GL_FRAGMENT_SHADER]);
  }
  
  // Link
  glLinkProgram(m_program_id_);
  
  // Check link results
  GLint status;
  glGetProgramiv(m_program_id_, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {   
    // Signal the failure
    return false;
  }
  
  // Build attribute and uniform maps
  BuildAttributeMap();
  BuildUniformLocationMap();
  
  // Signal the success
  return true;
}

std::string Shader::GetLinkLog() {
  // Get the log length
  GLint infoLogLength;
  glGetProgramiv (m_program_id_, GL_INFO_LOG_LENGTH, &infoLogLength);
  
  // Get the log itself
  GLchar *infoLog = new GLchar[infoLogLength];
  glGetProgramInfoLog (m_program_id_, infoLogLength, NULL, infoLog);
  
  // Compose the output
  std::stringstream ss;
  ss << "Link log: " << std::endl << infoLog << std::endl;
  
  // Clean up
  delete [] infoLog;
  
  // Return the result
  return ss.str();
}

void Shader::Activate() {
  glUseProgram(m_program_id_);
}

void Shader::Deactivate() {
  glUseProgram(0);
}

GLuint Shader::GetAttribute(const std::string attribute_name) {
  if(m_attribute_map_.find(attribute_name) == m_attribute_map_.end()) {
    throw std::runtime_error("Unable to find requested attribute. Maybe it was unused?");
  }
  return m_attribute_map_[attribute_name];
}

GLuint Shader::GetUniform(const std::string uniform_name) {
  if(m_uniform_map_.find(uniform_name) == m_uniform_map_.end()) {
    throw std::runtime_error("Unable to find requested uniform. Maybe it was unused?"); 
  }
  return m_uniform_map_[uniform_name];
}

void Shader::BuildAttributeMap() {
  // Clear old info
  m_attribute_map_.clear();
  
  // Get data about active attributes
  GLint active_attribute_count;
  glGetProgramiv(m_program_id_, GL_ACTIVE_ATTRIBUTES, &active_attribute_count);
  
  GLint active_attribute_max_name_length;
  glGetProgramiv(m_program_id_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &active_attribute_max_name_length);
    
  // Allocate the memory needed
  GLchar *attribute_name = new GLchar[active_attribute_max_name_length];
  GLint attribute_size;  // Unused
  GLenum attribute_type;  // Unused
  
  for(GLuint attribute_index = 0;attribute_index < active_attribute_count;++attribute_index) {
    // Get the info about the attribute
    glGetActiveAttrib(m_program_id_, attribute_index, active_attribute_max_name_length,
                      NULL, &attribute_size, &attribute_type, attribute_name);
    
    // Get the attribute location
    GLuint attibute_location = glGetAttribLocation(m_program_id_, attribute_name);
    m_attribute_map_[attribute_name] = attibute_location;
  }
    
  delete [] attribute_name;
}

void Shader::BuildUniformLocationMap() {
  // Clear old info
  m_uniform_map_.clear();
  
  // Get data about active attributes
  GLint active_uniform_count;
  glGetProgramiv(m_program_id_, GL_ACTIVE_UNIFORMS, &active_uniform_count);
  
  GLint active_uniform_max_name_length;
  glGetProgramiv(m_program_id_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &active_uniform_max_name_length);
  
  // Allocate the memory needed
  GLchar *uniform_name = new GLchar[active_uniform_max_name_length];
  GLint uniform_size;  // Unused
  GLenum uniform_type;  // Unused
  
  for(GLuint uniform_index = 0;uniform_index < active_uniform_count;++uniform_index) {
    // Get the info about the uniform
    glGetActiveUniform(m_program_id_, uniform_index, active_uniform_max_name_length,
                       NULL, &uniform_size, &uniform_type, uniform_name);
    
    // Get the attribute location
    GLuint uniform_location = glGetUniformLocation(m_program_id_, uniform_name);
    m_uniform_map_[uniform_name] = uniform_location;
  }
    
  delete [] uniform_name;
}

std::string Shader::DumpAttributes() {
  std::stringstream ss;
  std::map<std::string, GLuint>::iterator it = m_attribute_map_.begin();
  std::map<std::string, GLuint>::iterator end = m_attribute_map_.end();
  while(it != end) {
    ss << it->first << ": " << it->second << std::endl;
    ++it;
  }
  return ss.str();
}
  
std::string Shader::DumpUniforms() {
  std::stringstream ss;
  std::map<std::string, GLuint>::iterator it = m_uniform_map_.begin();
  std::map<std::string, GLuint>::iterator end = m_uniform_map_.end();
  while(it != end) {
    ss << it->first << ": " << it->second << std::endl;
    ++it;
  }
  return ss.str();  
}

}  // GfxUtil
