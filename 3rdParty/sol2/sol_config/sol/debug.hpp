#pragma once

// sol2 uses std::cout for debug logging by default.
// We want to use SDL logging instead for better compatibility.

#include <cstddef>
#include <string>

#include <sol/stack.hpp>

namespace devilutionx {
void Sol2DebugPrintStack(lua_State *L);
void Sol2DebugPrintSection(const std::string &message, lua_State *L);
} // namespace devilutionx

namespace sol::detail::debug {

inline std::string dump_types(lua_State *L) {
  std::string visual;
  std::size_t size = lua_gettop(L) + 1;
  for (std::size_t i = 1; i < size; ++i) {
    if (i != 1) {
      visual += " | ";
    }
    visual += type_name(L, stack::get<type>(L, static_cast<int>(i)));
  }
  return visual;
}

inline void print_stack(lua_State *L) { ::devilutionx::Sol2DebugPrintStack(L); }

inline void print_section(const std::string &message, lua_State *L) {
  ::devilutionx::Sol2DebugPrintSection(message, L);
}

} // namespace sol::detail::debug
