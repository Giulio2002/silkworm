#[[
   Copyright 2020 The Silkworm Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
]]

find_package(absl CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)

file(GLOB_RECURSE SILKWORM_CORE_SRC CONFIGURE_DEPENDS "*.cpp" "*.hpp" "*.c" "*.h")
list(FILTER SILKWORM_CORE_SRC EXCLUDE REGEX "_test\.cpp$")

add_library(silkworm_core ${SILKWORM_CORE_SRC})
get_filename_component(SILKWORM_MAIN_DIR ../ ABSOLUTE)
target_include_directories(silkworm_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${SILKWORM_MAIN_DIR})

set(SILKWORM_CORE_PUBLIC_LIBS absl::flat_hash_map absl::flat_hash_set
                              evmc intx::intx ff Microsoft.GSL::GSL)
set(SILKWORM_CORE_PRIVATE_LIBS evmone secp256k1 keccak Boost::boost)

target_link_libraries(silkworm_core PUBLIC ${SILKWORM_CORE_PUBLIC_LIBS} PRIVATE ${SILKWORM_CORE_PRIVATE_LIBS})
