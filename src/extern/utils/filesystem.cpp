/**
 * @file
 * @date 9 juin 2010
 * @todo comment
 */

#include <glob.h>
#include <sys/stat.h>

#include <boost/filesystem.hpp>

#include "extern/utils/filesystem.hpp"
#include "intern/utils/filesystem.hpp"

#include <logging/logging.hpp>

void find_files_by_pattern(char const* pattern, stringvectst& filenames) {
  ::std::vector< ::std::string > files;
  find_files_by_glob(pattern, files);

  filenames = files;
}

void find_directories_by_pattern(char const* pattern, stringvectst& filenames) {
  ::std::vector< ::std::string > folders;
  find_folders_by_glob(pattern, folders);

  filenames = folders;
}

void find_files_by_pattern_with_exception(char const* pattern, svector< char* >& filenames, char const* exception) {
  ::std::vector< ::std::string > files;
  find_files_by_glob(pattern, files, exception);

  filenames = files;
}

void find_directories_by_pattern_with_exception(char const* pattern, stringvectst& filenames, char const* exception) {
  ::std::vector< ::std::string > folders;
  find_folders_by_glob(pattern, folders, exception);

  filenames = folders;
}
