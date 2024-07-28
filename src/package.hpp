#pragma once
#include "fs.hpp"

#include <vector>

namespace app {
   class Package {
   public:
      typedef std::vector<fs::path> PathList;
      template<typename Base>
      class absolute_path_iterator {
      public:
         typedef std::forward_iterator_tag     iterator_category;
         typedef std::ptrdiff_t                difference_type;
         typedef fs::path                      value_type;
         typedef typename Base::const_iterator const_base_iterator;

         absolute_path_iterator(const_base_iterator end) :
            m_it(end) {}
         absolute_path_iterator(const fs::path& output, const_base_iterator beg) :
            m_output(output),
            m_it(beg) {}

         value_type operator*() const {
            return m_output/(*m_it);
         }

         absolute_path_iterator& operator++() {
            m_it++;
            return *this;
         }

         bool operator!=(const absolute_path_iterator& that) const {
            return m_it != that.m_it;
         }
      private:
         fs::path m_output;
         const_base_iterator m_it;
      };

      void addRelativePath(const fs::path& path);

      const std::string& name() const;
      const fs::path& outDir() const;
      const PathList& relativePaths() const;

      auto begin() const {return absolute_path_iterator<PathList>(m_outDir, m_paths.begin());}
      auto end() const {return absolute_path_iterator<PathList>(m_paths.end());}
   private:
      friend class PackageManager;

      Package(const fs::path& output, const std::string& pkgName, bool didOutputExists);
      bool didOutputExists() const;

      bool m_didOutputExists;
      fs::path m_outDir;
      PathList m_paths;
      std::string m_name;
   };
} // namespace app