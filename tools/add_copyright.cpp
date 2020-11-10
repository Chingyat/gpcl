//
// add_copyright.cpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#if defined(_MSC_VER) || __has_include(<filesystem>)
#include <filesystem>
using namespace std::filesystem;
#else
#include <experimental/filesystem>
using namespace std::experimental::filesystem;
#endif

using namespace std;

namespace tool {

auto is_c_cpp(const std::string &extension) -> bool {
  const static std::vector<std::string> source_extensions{".cpp", ".hpp",
      ".ipp", ".cxx", ".hxx", ".tpp", ".ixx", ".txx", ".cc", ".hh", ".c", ".h"};
  const auto pos =
      std::find(source_extensions.begin(), source_extensions.end(), extension);
  return pos != source_extensions.end();
}

auto read_file_content(const std::string &path) -> std::string {
  std::stringstream ss;
  ifstream str(path);
  ss << str.rdbuf();
  return ss.str();
}

auto process_file(const path &p) -> void {
  std::clog << "processing " << p << std::endl;

  std::string content = read_file_content(p.string());
  const char *copyright = "Copyright";
  if (content.find(copyright) != std::string::npos)
    return;

  ofstream out(p.string(), std::ios_base::trunc | std::ios_base::out);
  out.exceptions(std::ios_base::failbit | std::ios_base::badbit);
  out << "//\n"
      << "// " << p.filename().string() << "\n"
      << "// " << std::string(p.filename().string().length(), '~') << "\n"
      << "//\n"
      << "// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot "
         "com)\n"
      << "//\n"
      << "// Distributed under the Boost Software License, Version "
         "1.0. (See accompanying\n"
      << "// file LICENSE_1_0.txt or copy at "
         "http://www.boost.org/LICENSE_1_0.txt)\n"
      << "//\n\n"
      << content;

  if (content.empty() || content.back() != '\n')
    out << '\n';
}

auto iterate(directory_iterator iter) -> void {
  std::for_each(std::move(iter), directory_iterator{}, [](const path &p) {
    if (is_directory(p))
      iterate(directory_iterator{p});

    else if (is_regular_file(p) && is_c_cpp(p.extension().string()))
      process_file(p);
  });
}

extern "C" auto main(int argc, const char **argv) -> int {
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " srcdir\n";
    return 1;
  }

  std::for_each(&argv[1], &argv[argc], [](const char *arg) {
    auto iter = directory_iterator{path{arg}};
    iterate(std::move(iter));
  });

  return 0;
}

} // namespace tool
