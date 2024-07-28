#include "file.hpp"

#include "fs.hpp"
#include "output.hpp" 

#include <cassert>
#include <cstdio>
#include <cstring>

namespace utils {
  namespace priv {
    void File_base::Deleter::operator()(FILE* file) const {
      if(file)
        fclose(file);          
    }

    File_base::File_base(const fs::path& filepath, const char* mode) :
        m_file{fopen(filepath.c_str(), mode)},
        m_filepath{filepath} {
        if(!m_file)
          throw std::runtime_error(std::format("Unable to read/write '{}': {}", filepath.string(), std::strerror(errno)));
    }

    void File_base::close() {
      if(m_file) m_file.reset();
    }    
  } // namespace priv

  WFile::WFile(const fs::path& filepath) :
    priv::File_base(filepath, "wb") {
    VERBOSE("Writing file in '{}'", filepath.string());
  }

  bool WFile::open(const fs::path& filepath) {
    auto f = fopen(filepath.c_str(), "wb");
    if(!f)
      return false;
    file().reset(f);
    m_filepath = filepath;
    return f;
  }

  bool WFile::write8(uint8_t value)        {return write(&value, 1);}
  bool WFile::write16(uint16_t value)      {return write(&value, 2);}
  bool WFile::write32(uint32_t value)      {return write(&value, 4);}
  bool WFile::writeText(const char text[]) {return write(text, strlen(text));}
  bool WFile::writePath(const fs::path& path) {
    bool res = write32(path.string().size());
    if(res) writeText(path.c_str());
    return res;
  }
  bool WFile::write(const void* buffer, size_t size) {
    if(!file())
        return false;
    
    if(fwrite(buffer, 1, size, file().get()) != size) {
        file().reset();
        return false;
    }

    return true;
  }

  void WFile::flush() {
    if(file())
        fflush(file().get());
  }

  File::File(const fs::path& filepath) :
    priv::File_base(filepath, "rb") {
    m_current = 0;
    m_end = fgetsize(file().get());
  }

  bool File::open(const fs::path& filepath) {
    auto f = fopen(filepath.c_str(), "rb");
    if(!f)
      return false;
    file().reset(f);
    m_filepath = filepath;
    return true;
  }

  bool File::readS8(int8_t* i) {
    return read(i, sizeof(*i)) == sizeof(*i);
  }
  bool File::readS16(int16_t* i) {
    return read(i, sizeof(*i)) == sizeof(*i);
  }
  bool File::readS32(int32_t* i) {
    return read(i, sizeof(*i)) == sizeof(*i);
  }
  bool File::readPath(fs::path* path) {
    unsigned szPath;
    bool res = readU32(&szPath);
    if(res) {
      char buf[szPath];
      if(read(buf, szPath))
        path->assign(buf, buf+szPath);
    }
    return res;
  }

  size_t File::read(void* buffer, size_t size) {
    assert(buffer);
    if(size > m_end - m_current)
        size = m_end - m_current;
    
    size_t bytesRead = size;
    if(buffer) {
        bytesRead = fread(buffer, 1, size, file().get());
    }

    if(bytesRead == SIZE_MAX)
        return 0;
    m_current += bytesRead;
    return bytesRead;
  }

  bool File::isAtEnd() const {
    if(m_current == m_end)
        return true;
    
    return m_current >= fgetsize(file().get());
  }

  size_t File::fgetsize(FILE* f) {
    assert(f != nullptr);

    long curr = ftell(f);
    if(curr < 0)
        return 0;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    if(size < 0)
        size = 0;
    
    fseek(f, curr, SEEK_SET);
    return size;
  }
} // namespace utils