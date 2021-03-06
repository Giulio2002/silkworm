/*
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
*/

#ifndef SILKWORM_TYPES_BLOOM_H_
#define SILKWORM_TYPES_BLOOM_H_

#include <stddef.h>
#include <stdint.h>

#include <array>
#include <silkworm/types/log.hpp>
#include <vector>

namespace silkworm {

constexpr size_t kBloomByteLength{256};

using Bloom = std::array<uint8_t, kBloomByteLength>;

inline ByteView full_view(const Bloom& bloom) { return {bloom.data(), kBloomByteLength}; }

Bloom logs_bloom(const std::vector<Log>& logs);

}  // namespace silkworm

#endif  // SILKWORM_TYPES_BLOOM_H_
