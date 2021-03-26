/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "Utility/String.h"
#include "Utility/LogSys.h"

namespace QBDI {
namespace String {

bool startsWith(const char *prefix, const char *str) {
  QBDI_REQUIRE_ACTION(prefix != nullptr, return false);
  QBDI_REQUIRE_ACTION(str != nullptr, return false);

  while (*prefix && *str) {
    // Wildcard matching : "*" until the character following the wildcard
    // matches a character in the str
    if (*prefix == '*') {
      if (toupper(*(prefix + 1)) == toupper(*str++)) {
        prefix++;
      }
      continue;
    }
    // Compare char
    if (toupper(*prefix++) != toupper(*str++)) {
      return false;
    }
  }
  // check pending character after wildcard
  if (*prefix && *(prefix + 1)) {
    return false;
  }
  // prefix checking, if next char is not either \0, "_" or [0-9] then do not
  // match
  if (!((*str >= '0' && *str <= '9') || *str == '_' || !*str)) {
    return false;
  }
  return true;
}

} // namespace String
} // namespace QBDI
