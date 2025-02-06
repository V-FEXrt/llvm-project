//===- DXILValidateResourceCounter.cpp - Resource Validation Pass ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "DXILValidateResourceCounter.h"

#include "DXILConstants.h"
#include "DXILIntrinsicExpansion.h"
#include "DXILOpBuilder.h"
#include "DXILResourceAnalysis.h"
#include "DXILShaderFlags.h"
#include "DirectX.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/DXILMetadataAnalysis.h"
#include "llvm/Analysis/DXILResource.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IntrinsicsDirectX.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/ErrorHandling.h"

#define DEBUG_TYPE "dxil-validate-resource-counter"

using namespace llvm;
using namespace llvm::dxil;

static bool isUpdateCounterIntrinsic(Function &F) {
  return F.getIntrinsicID() == Intrinsic::dx_resource_updatecounter;
}

namespace {
class CountValidator {
  Module &M;
  DXILBindingMap &DBM;
  DXILResourceTypeMap &DRTM;

public:
  CountValidator(Module &M, DXILBindingMap &DBM, DXILResourceTypeMap &DRTM)
      : M(M), DBM(DBM), DRTM(DRTM) {}

  bool validate() {
    for (Function &F : M.functions()) {
      if (isUpdateCounterIntrinsic(F))
        validateFunction(F);
    }

    return false;
  }

private:
  bool validateFunction(Function &F) {
    printf("fex: F: %s\n", F.getName());
    for (const User *U : F.users()) {
      printf("  fex: User\n");
      U->dump();

      const CallInst *CI = dyn_cast<CallInst>(U);
      assert(CI && "resource updateCounter use must be a call instruction");

      Value *HandleArg = CI->getArgOperand(0);
      auto *It = DBM.findByUse(HandleArg);
      assert(It != DBM.end() && "HandleArg is not a resource");

      Value *CountArg = CI->getArgOperand(1);
      ConstantInt *CountVal = dyn_cast<ConstantInt>(CountArg);
      printf("  fex: Count: %ld\n", CountVal->getSExtValue());
    }

    return false;
  }

};

} // namespace

PreservedAnalyses DXILValidateResourceCounter::run(Module &M, ModuleAnalysisManager &MAM) {
  DXILBindingMap &DBM = MAM.getResult<DXILResourceBindingAnalysis>(M);
  DXILResourceTypeMap &DRTM = MAM.getResult<DXILResourceTypeAnalysis>(M);

  CountValidator(M, DBM, DRTM).validate();

  return PreservedAnalyses::all();
}

namespace {
class DXILValidateResourceCounterLegacy : public ModulePass {
public:
  bool runOnModule(Module &M) override {
    DXILBindingMap &DBM =
        getAnalysis<DXILResourceBindingWrapperPass>().getBindingMap();
    DXILResourceTypeMap &DRTM =
        getAnalysis<DXILResourceTypeWrapperPass>().getResourceTypeMap();

    return CountValidator(M, DBM, DRTM).validate();
  }
  StringRef getPassName() const override { return "DXIL Validate Resource Counter"; }
  DXILValidateResourceCounterLegacy() : ModulePass(ID) {}

  static char ID; // Pass identification.
  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override {
    // TODO: review these

//    addPass(createDXILFinalizeLinkageLegacyPass());
//    addPass(createDXILIntrinsicExpansionLegacyPass());
  //  addPass(createDXILDataScalarizationLegacyPass());
//    addPass(createDXILFlattenArraysLegacyPass());
  //  addPass(createDXILResourceAccessLegacyPass());
    //addPass(createScalarizerPass(DxilScalarOptions));
//    addPass(createDXILTranslateMetadataLegacyPass());


    AU.addRequired<DXILResourceTypeWrapperPass>();
    AU.addRequired<DXILResourceBindingWrapperPass>();
    AU.addPreserved<DXILResourceBindingWrapperPass>();
    AU.addPreserved<DXILResourceMDWrapper>();
    AU.addPreserved<DXILMetadataAnalysisWrapperPass>();
    AU.addPreserved<ShaderFlagsAnalysisWrapper>();
  }
};
char DXILValidateResourceCounterLegacy::ID = 0;
} // end anonymous namespace

// TODO: review these
INITIALIZE_PASS_BEGIN(DXILValidateResourceCounterLegacy, DEBUG_TYPE, "DXIL Validate Resource Counter",
                      false, false)
INITIALIZE_PASS_DEPENDENCY(DXILResourceTypeWrapperPass)
INITIALIZE_PASS_DEPENDENCY(DXILResourceBindingWrapperPass)
INITIALIZE_PASS_END(DXILValidateResourceCounterLegacy, DEBUG_TYPE, "DXIL Validate Resource Counter", false,
                    false)

ModulePass *llvm::createDXILValidateResourceCounterLegacyPass() {
  return new DXILValidateResourceCounterLegacy();
}
