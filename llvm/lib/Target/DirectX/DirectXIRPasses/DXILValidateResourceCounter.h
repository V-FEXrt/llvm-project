//===- DXILValidateResourceCounter.h - Resource Validation Pass -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// \file Pass for validating a resource counter is only incremented or
// decremented, but not both.
//===----------------------------------------------------------------------===//

#ifndef LLVM_DIRECTX_DIRECTXIRPASSES_DXILVALIDATERESOURCECOUNTER_H
#define LLVM_DIRECTX_DIRECTXIRPASSES_DXILVALIDATERESOURCECOUNTER_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class DXILValidateResourceCounter : public PassInfoMixin<DXILValidateResourceCounter> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
};

} // namespace llvm

#endif // LLVM_DIRECTX_DIRECTXIRPASSES_DXILVALIDATERESOURCECOUNTER_H
