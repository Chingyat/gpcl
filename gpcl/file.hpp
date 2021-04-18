#ifndef GPCL_FILE_HPP
#define GPCL_FILE_HPP

#include <gpcl/detail/config.hpp>

#if defined GPCL_POSIX
#  include <gpcl/detail/posix_file.hpp>
#elif defined GPCL_WINDOWS
#  include <gpcl/detail/win_file.hpp>
#endif

namespace gpcl {

#if defined GPCL_POSIX
typedef detail::posix_file file;

#elif defined GPCL_WINDOWS
typedef detail::win_file file;

#endif

} // namespace gpcl

#endif // GPCL_FILE_HPP
