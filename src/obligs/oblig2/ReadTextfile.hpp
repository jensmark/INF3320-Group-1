#ifndef READ_TEXTFILE_HEADER_H_
#define READ_TEXTFILE_HEADER_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace GfxUtil {

inline std::string ReadTextfile(const std::string& filename) {
  // Return value
  std::string ret;

  // Open file.
  std::ifstream is(filename.c_str(), std::ios::binary);
  if (!is) {
	  std::cerr << "Could not open file " << filename << '\n';
	  return ret;
  }

  // Find its length.
  is.seekg(0, std::ios::end);
  int length = is.tellg();
  is.seekg(0, std::ios::beg);

  // Make a big enough vector and read.
  std::vector<char> data(length);
  is.read(&data[0], length);
  ret.assign(data.begin(), data.end());
  return ret;
}

}  // GfxUtil

#endif  // READ_TEXTFILE_HEADER_H_
