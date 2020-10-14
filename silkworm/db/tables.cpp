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

#include "tables.hpp"

namespace silkworm::db::table {

void create_all(lmdb::Transaction& txn) {
    for (const auto& table : kTables) {
        txn.open(table, MDB_CREATE);
    }
}

void create_all(mdbx::txn& txn) {
    for (const auto& table : kTables) {
        txn.create_map(table.name, mdbx::key_mode::usual,
                       table.multi_val ? mdbx::value_mode::multi : mdbx::value_mode::single);
    }
}

}  // namespace silkworm::db::table
