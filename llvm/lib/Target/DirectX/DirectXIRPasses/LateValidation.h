//===- Target/DirectX/LateValidation.h - Late staged DXIL analysis --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Analysis pass to run DXIL validations that require full resolution and
// optimization
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_DIRECTX_LATEVALIDATION_H
#define LLVM_TARGET_DIRECTX_LATEVALIDATION_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"

namespace llvm {

namespace dxil {

namespace LateValidation {

bool run(const Module &M);
} // namespace LateValidation

} // namespace dxil

} // namespace llvm

#endif // LLVM_TARGET_DIRECTX_LATEVALIDATION_H
