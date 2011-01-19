/**
 * @file
 * @date 9 juin 2010
 * @todo comment
 */

#include <logging/logging.hpp>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "intern/utils/filesystem.hpp"

namespace bfs = ::boost::filesystem;

template< bool( predicate_t)(const bfs::path&) >
inline void find_by_regexp(const ::std::string& regex, ::std::vector< ::std::string >& paths, const ::std::string& exception);

template< bool( predicate_t)(const bfs::path&) >
inline void find_by_glob(const ::std::string& glob, ::std::vector< ::std::string >& paths, const ::std::string& exception);

template< bool( predicate_t)(const bfs::path&) >
inline void find_by_regexp(const ::std::string& regex, ::std::vector< ::std::string >& paths, const ::std::string& exception) {
  bfs::path root = bfs::current_path();
  ::std::size_t root_len = root.string().length();

  ::boost::regex expression(regex, ::boost::regex::extended);
  ::boost::regex exception_expr(exception, ::boost::regex::extended);
  ::boost::sregex_iterator end;

  if (bfs::is_directory(root)) {
    for (bfs::recursive_directory_iterator it(root); it != bfs::recursive_directory_iterator(); ++it) {

      if (predicate_t(it->path())) {
        ::std::string path = it->path().string();
        path.erase(0, root_len + 1);

        ::boost::sregex_iterator regex_it(path.begin(), path.end(), expression);

        if (regex_it != end) {
          ::boost::sregex_iterator except_it(path.begin(), path.end(), exception_expr);

          if (except_it == end) {
            paths.push_back(path);
          }
        }
      }
    }
  }

  ::std::sort(paths.begin(), paths.end());
}

template< bool( predicate_t)(const bfs::path&) >
inline void find_by_glob(const ::std::string& glob, ::std::vector< ::std::string >& paths, const ::std::string& exception) {
  ::std::size_t star_pos = glob.find_first_of('*');
  ::std::size_t slash_pos = glob.find_last_of('/', star_pos);

  bfs::path root = bfs::current_path();
  root /= ::std::string(glob.begin(), glob.begin() + slash_pos);

  if (bfs::is_directory(root)) {
    for (bfs::directory_iterator it(root); it != bfs::directory_iterator(); ++it) {
      if (predicate_t(it->path())) {
        ::std::string filename = it->path().filename().string();

        if (exception.compare(filename) != 0) {
          ::std::size_t compare_start_len = star_pos - slash_pos - 1;
          ::std::size_t compare_end_len = glob.length() - star_pos - 1;

          bool match_start = glob.compare(slash_pos + 1, compare_start_len, filename, 0, compare_start_len) == 0;
          bool match_end = glob.compare(star_pos + 1, compare_end_len, filename, filename.length() - compare_end_len, compare_end_len) == 0;

          if (match_start && match_end) {
            paths.push_back(it->path().filename().string());
          }
        }
      }
    }
  }

  ::std::sort(paths.begin(), paths.end());
}

void find_files_by_regexp(const ::std::string& regex, ::std::vector< ::std::string >& files, const ::std::string& exception) {
  find_by_regexp< bfs::is_regular_file > (regex, files, exception);
}

void find_folders_by_regexp(const ::std::string& regex, ::std::vector< ::std::string >& folders, const ::std::string& exception) {
  find_by_regexp< bfs::is_directory > (regex, folders, exception);
}

void find_files_by_glob(const ::std::string& glob, ::std::vector< ::std::string >& files, const ::std::string& exception) {
  find_by_glob< bfs::is_regular_file > (glob, files, exception);
}

void find_folders_by_glob(const ::std::string& glob, ::std::vector< ::std::string >& folders, const ::std::string& exception) {
  find_by_glob< bfs::is_directory > (glob, folders, exception);
}

void copy_file(const ::std::string& source, const ::std::string& dest) {
  bfs::copy_file(source, dest);
}

void replace_file(const ::std::string& source, const ::std::string& dest) {
  bfs::remove(dest);
  bfs::rename(source, dest);
}
