//===----------------------------------------------------------------------===//
//
// Part of the Eter Project, under the Apache License v2.0 with LLVM Exceptions.
// See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "eter/Base/Debug.h"
#include "eter/Parser/Parser.h"

#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/raw_ostream.h>

#define DEBUG_TYPE "parser-pat"

namespace eter::parser {

NodeIndex Parser::parsePat() {
  ETER_DEBUG(llvm::dbgs() << "[" DEBUG_TYPE "] parsePat\n");
  llvm::report_fatal_error("TODO: implement Parser::parsePat");
}

NodeIndex Parser::parseStructPat(InternedStr Name, Span Start) {
  ETER_DEBUG(llvm::dbgs() << "[" DEBUG_TYPE "] parseStructPat\n");
  (void)Name;
  (void)Start;
  llvm::report_fatal_error("TODO: implement Parser::parseStructPat");
}

NodeIndex Parser::parseTuplePat(InternedStr Name, Span Start) {
  ETER_DEBUG(llvm::dbgs() << "[" DEBUG_TYPE "] parseTuplePat\n");
  (void)Name;
  (void)Start;
  llvm::report_fatal_error("TODO: implement Parser::parseTuplePat");
}

} // namespace eter::parser
