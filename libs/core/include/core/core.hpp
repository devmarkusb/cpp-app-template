#ifndef MB_CPP_APP_TEMPLATE_CORE_HPP_INCLUDED
#define MB_CPP_APP_TEMPLATE_CORE_HPP_INCLUDED

#include <cstdint>

namespace mb::cpp_app_template {
/// Sum of \p a and \p b. Overflow follows signed 32-bit arithmetic rules (undefined if out of range).
int32_t sum(int32_t a, int32_t b);
} // namespace mb::cpp_app_template

#endif
