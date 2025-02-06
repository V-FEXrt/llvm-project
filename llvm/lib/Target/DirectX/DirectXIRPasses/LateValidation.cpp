//===- Target/DirectX/LateValidation.cpp - Late staged DXIL analysis ------===//
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


#include "LateValidation.h"
#include "DXILConstants.h"
#include "DXILResource.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/DXILABI.h"

using namespace llvm;
using namespace llvm::dxil;

namespace llvm {

namespace dxil {

namespace {
bool validateDxilExternalFunction(const Module &M, const Function &F) {
    std::string Message("Up or down but not updown");
    for (const User *U : F.users()) {
        U->dump();
        const CallInst *CI = dyn_cast<CallInst>(U);
        if (!CI) continue;

        Value *argOpcode = CI->getArgOperand(0);
        ConstantInt *constOpcode = dyn_cast<ConstantInt>(argOpcode);
        if (!constOpcode) continue;

        unsigned opcode = constOpcode->getLimitedValue();

        OpCode DxilOp = static_cast<OpCode>(opcode);

        if (DxilOp == OpCode::UpdateCounter) {
            Value *argResource = CI->getArgOperand(1);
            UniqueResourceKey key(argResource);

            Value *argCount = CI->getArgOperand(2);
            ConstantInt *argCountVal = dyn_cast<ConstantInt>(argCount);
            // printf("fex: val %d\n", argCountVal->getLimitedValue());
            // DiagnosticInfoUnsupported Diag(F, Message, CI->getDebugLoc());
            // M.getContext().diagnose(Diag);
        }
    }

    return false;
}

bool validateDxilModule(const Module &M) {
  bool AnyFailed = false;
  for (const Function &F : M.functions()) {
      printf("fex: Function: %s\n", F.getName());
      if (F.getName() == "dx.op.bufferUpdateCounter") {
          F.dump();
          AnyFailed |= validateDxilExternalFunction(M, F);
      }
  }
  return AnyFailed;
}

} // namespace

bool LateValidation::run(const Module &M) {
    return validateDxilModule(M);
}

} // namespace dxil
} // namespace llvm
