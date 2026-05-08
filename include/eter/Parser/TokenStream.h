//===----------------------------------------------------------------------===//
//
// Part of the Eter Project, under the Apache License v2.0 with LLVM Exceptions.
// See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef ETER_PARSER_TOKENSTREAM_H
#define ETER_PARSER_TOKENSTREAM_H

#include "eter/Base/Span.h"
#include "eter/Lexer/Lexer.h"
#include "eter/Lexer/Token.h"

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/StringRef.h>

#include <vector>

namespace eter::parser {

/// A flat, indexed token sequence consumed by the parser.
///
/// The lexer returns a heterogeneous `std::vector<LexerItem>` mixing tokens
/// and errors. `TokenStream` separates them: errors are preserved for the
/// caller to emit, while the token array gives the parser a uniform,
/// index-addressable view. `LexerError` positions are replaced with
/// `Token::Kind::unknown` so the parser can apply error recovery without
/// branching on the variant type.
class TokenStream {
public:
  /// Build a TokenStream by consuming a vector of LexerItems produced by the
  /// Eter lexer. LexerErrors are stripped into `LexErrors`; their positions
  /// become `unknown` tokens. `Source` must be the same buffer that was passed
  /// to the Lexer — it is used by `textOf` to recover identifier/literal text.
  explicit TokenStream(std::vector<lexer::LexerItem> Items,
                       llvm::StringRef Source);

  /// Return the kind of the token at `Cursor + Offset` without advancing.
  /// Returns `Token::Kind::eof` when out of bounds.
  [[nodiscard]] lexer::Token::Kind peek(uint32_t Offset = 0) const;

  /// Return the full token at `Cursor + Offset` without advancing.
  [[nodiscard]] lexer::Token peekToken(uint32_t Offset = 0) const;

  /// Return the token most recently returned by `advance()`.
  [[nodiscard]] lexer::Token previous() const;

  /// Return true if the current token has kind `K`.
  [[nodiscard]] bool check(lexer::Token::Kind K) const;

  /// Return true if the stream is at end-of-file.
  [[nodiscard]] bool atEof() const;

  /// Consume and return the current token, advancing the cursor.
  lexer::Token advance();

  /// If the current token has kind `K`, consume it and return true.
  /// Otherwise return false and leave the cursor unchanged.
  bool consume(lexer::Token::Kind K);

  /// Extract the source text covered by span `S`.
  /// The span must reference the same buffer passed to the constructor.
  [[nodiscard]] llvm::StringRef textOf(Span S) const {
    return Source.substr(S.Start, S.End - S.Start);
  }

  /// Errors collected from the original LexerItem stream. These are returned
  /// inside ParseResult and emitted by the Driver — not by the parser itself.
  [[nodiscard]] llvm::ArrayRef<lexer::LexerError> lexErrors() const {
    return LexErrors;
  }

private:
  std::vector<lexer::Token> Tokens;
  std::vector<lexer::LexerError> LexErrors;
  llvm::StringRef Source;
  uint32_t Cursor = 0;
};

} // namespace eter::parser

#endif // ETER_PARSER_TOKENSTREAM_H
