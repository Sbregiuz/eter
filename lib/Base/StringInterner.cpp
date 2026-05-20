//===----------------------------------------------------------------------===//
//
// Part of the Eter Project, under the Apache License v2.0 with LLVM Exceptions.
// See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "eter/Base/Debug.h"
#include "eter/Base/StringInterner.h"

#include <llvm/Support/raw_ostream.h>

#define DEBUG_TYPE "base-intern"

namespace eter::parser {

StringInterner::StringInterner() {
  // ID 0 is reserved as NullStr; push an empty sentinel so that any accidental
  // use of NullStr is detectable (get(0) returns an empty StringRef).
  Strings.push_back(llvm::StringRef());
}

InternedStr StringInterner::intern(llvm::StringRef S) {
  auto [It, Inserted] = Table.insert({S, 0});
  if (!Inserted)
    return It->second;

  // StringMap copies S into StringMapEntry's inline storage on insert.
  // It->first() is a stable StringRef into that storage for the lifetime
  // of Table.
  auto Id = static_cast<InternedStr>(Strings.size());
  It->second = Id;
  Strings.push_back(It->first());

  ETER_DEBUG(llvm::dbgs() << "[" DEBUG_TYPE "] interned \"" << S << "\" → "
                          << Id << "\n");
  return Id;
}

llvm::StringRef StringInterner::get(InternedStr Id) const {
  assert(Id < Strings.size() && "InternedStr out of range");
  return Strings[Id];
}

uint32_t StringInterner::size() const {
  // Subtract 1 to exclude the NullStr sentinel.
  return static_cast<uint32_t>(Strings.size()) - 1;
}

} // namespace eter::parser
