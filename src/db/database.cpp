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

#include "database.hpp"

#include "bucket.hpp"
#include "history_index.hpp"
#include "types/block.hpp"
#include "util.hpp"

namespace silkworm::db {
std::optional<BlockWithHash> Database::get_block(uint64_t block_number) {
  BlockWithHash bh{};
  auto txn{begin_ro_transaction()};

  auto header_bucket{txn->get_bucket(bucket::kBlockHeader)};
  std::optional<std::string_view> hash_val{header_bucket->get(header_hash_key(block_number))};
  if (!hash_val) return {};

  std::memcpy(bh.hash.bytes, hash_val->data(), kHashLength);
  std::string key{block_key(block_number, *hash_val)};

  std::optional<std::string_view> header_rlp{header_bucket->get(key)};
  if (!header_rlp) return {};

  auto header_stream{string_view_as_stream(*header_rlp)};
  rlp::decode(header_stream, bh.block.header);

  auto body_bucket{txn->get_bucket(bucket::kBlockBody)};
  std::optional<std::string_view> body_rlp{body_bucket->get(key)};
  if (!body_rlp) return {};

  auto body_stream{string_view_as_stream(*body_rlp)};
  rlp::decode<BlockBody>(body_stream, bh.block);

  return bh;
}

std::optional<Account> Database::get_account(const evmc::address& address, uint64_t block_number) {
  auto key{address_as_string_view(address)};
  auto txn{begin_ro_transaction()};
  std::optional<std::string_view> encoded = find_in_history(*txn, false, key, block_number);
  if (!encoded) {
    auto bucket{txn->get_bucket(bucket::kPlainState)};
    encoded = bucket->get(key);
  }
  if (!encoded || encoded->empty()) return {};
  return decode_account_from_storage(*encoded);
}

std::optional<AccountChanges> Database::get_account_changes(uint64_t block_number) {
  auto txn{begin_ro_transaction()};
  auto bucket{txn->get_bucket(bucket::kPlainAccountChanges)};
  std::optional<std::string_view> val{bucket->get(encode_timestamp(block_number))};
  if (!val) return {};
  return decode_account_changes(*val);
}

std::optional<std::string_view> Database::find_in_history(Transaction& txn, bool storage,
                                                          std::string_view key,
                                                          uint64_t block_number) {
  auto history_name{storage ? bucket::kStorageHistory : bucket::kAccountHistory};
  auto history_bucket{txn.get_bucket(history_name)};
  auto cursor{history_bucket->cursor()};
  std::optional<Entry> entry{cursor->seek(key)};
  if (!entry) return {};

  std::string_view k{entry->key};
  if (storage) {
    if (k.substr(0, kAddressLength) != key.substr(0, kAddressLength) ||
        k.substr(kAddressLength, kHashLength) != key.substr(kAddressLength + kIncarnationLength)) {
      return {};
    }
  } else if (!k.starts_with(key)) {
    return {};
  }

  std::optional<history_index::SearchResult> res{history_index::find(entry->value, block_number)};
  if (!res) return {};

  if (res->new_record && !storage) return std::string_view{};

  auto change_name{storage ? bucket::kPlainStorageChanges : bucket::kPlainAccountChanges};
  auto change_bucket{txn.get_bucket(change_name)};

  uint64_t change_block{res->change_block};
  std::optional<std::string_view> changes{change_bucket->get(encode_timestamp(change_block))};
  if (!changes) return {};

  if (storage) {
    // TODO(Andrew) implement
    return {};
  } else {
    return change::find_account(*changes, key);
  }
}
}  // namespace silkworm::db