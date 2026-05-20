//===----------------------------------------------------------------------===//
//
// Part of the Eter Project, under the Apache License v2.0 with LLVM Exceptions.
// See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef ETER_BASE_STRINGINTERNER_H
#define ETER_BASE_STRINGINTERNER_H

#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Allocator.h>

#include <cstdint>
#include <vector>

namespace eter::parser {

/// A handle to a canonicalized string in a `StringInterner`.
///
/// Two `InternedStr` values are equal if and only if they represent the same
/// string content, enabling O(1) identity comparison (integer equality) instead
/// of O(n) string comparison. The null value `NullStr` is reserved and never
/// returned by `StringInterner::intern`.
using InternedStr = uint32_t;

/// The null / invalid interned string sentinel.
inline constexpr InternedStr NullStr = 0;

/// A string interning table that assigns a stable `InternedStr` ID to each
/// unique string it sees.
///
/// All character data is owned by the `BumpPtrAllocator` embedded in `Table`:
/// `StringMap` allocates each entry (including its inline key bytes) from that
/// allocator, and `StringRef`s in `Strings` point into that storage.
/// The `StringInterner` must outlive any code that calls `get()` to retrieve
/// the raw `StringRef` text; the integer IDs themselves remain valid after
/// destruction (just numbers), but dereferencing them is UB.
///
/// ## Lifetime
/// The `StringInterner` is owned by the Driver and lives for the entire
/// compilation session, across all source files. This is intentional:
/// `InternedStr` IDs must remain comparable between different `ParseResult`
/// objects (e.g. checking whether two identifiers from different files are
/// the same name is an O(1) integer comparison). By contrast, each
/// `ParseResult` (and its `NodePool`) is freed as soon as the Driver no
/// longer needs the AST for that file.
///
/// Thread-safety: not thread-safe. The parser runs on a single thread.
class StringInterner {
public:
  StringInterner();

  /// Intern `S` and return its canonical ID. If `S` was already interned,
  /// return the existing ID. IDs are stable for the lifetime of this interner.
  [[nodiscard]] InternedStr intern(llvm::StringRef S);

  /// Return the string content for a previously interned ID.
  /// Behaviour is undefined if `Id == NullStr` or `Id` was not produced by
  /// this interner.
  [[nodiscard]] llvm::StringRef get(InternedStr Id) const;

  /// Return the number of unique strings currently interned.
  [[nodiscard]] uint32_t size() const;

private:
  llvm::StringMap<InternedStr, llvm::BumpPtrAllocator> Table; ///< content -> id
  std::vector<llvm::StringRef>
      Strings; ///< id -> StringRef into Table's bump storage
};

} // namespace eter::parser

#endif // ETER_BASE_STRINGINTERNER_H
