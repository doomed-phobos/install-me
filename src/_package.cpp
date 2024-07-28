#include "package.hpp"

#include "fs.hpp"
#include "output.hpp"
#include "string.hpp"
#include "package_exception.hpp"

namespace app {
   Package::Package(const fs::path& output, const std::string& pkgName, bool didOutputExists) :
      m_outDir(output),
      m_name(pkgName),
      m_didOutputExists(didOutputExists) {}
   
   void Package::addRelativePath(const fs::path& path) {
      m_paths.push_back(path);
   }

   const std::string& Package::name() const {return m_name;}
   const fs::path& Package::outDir() const {return m_outDir;}
   const Package::PathList& Package::relativePaths() const {return m_paths;}
   bool Package::didOutputExists() const {return m_didOutputExists;}
} // namespace app