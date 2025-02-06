//===- llvm/unittests/Target/DirectX/PointerTypeAnalysisTests.cpp ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "DirectXIRPasses/PointerTypeAnalysis.h"
#include "llvm/Analysis/DXILResource.h"
#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/TypedPointerType.h"
#include "llvm/Support/SourceMgr.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::Contains;
using ::testing::Pair;

using namespace llvm;
using namespace llvm::dxil;

template <typename T> struct IsA {
  friend bool operator==(const Value *V, const IsA &) { return isa<T>(V); }
};

TEST(UniqueResourceFromUse, DigressToi8) {
  StringRef Assembly = R"(
    define i64 @test(ptr %p) {
      store i32 0, ptr %p
      %v = load i64, ptr %p
      ret i64 %v
    }
  )";

  LLVMContext Context;
  SMDiagnostic Error;
  auto M = parseAssemblyString(Assembly, Error, Context);
  ASSERT_TRUE(M) << "Bad assembly?";

  ModuleAnalysisManager MAM;
  const DXILBindingMap &DBM = MAM.getResult<DXILResourceBindingAnalysis>(*M);

  //Pass.getBindingMap().fi

  /*
  PointerTypeMap Map = PointerTypeAnalysis::run(*M);
  ASSERT_EQ(Map.size(), 2u);
  Type *I8Ptr = TypedPointerType::get(Type::getInt8Ty(Context), 0);
  Type *FnTy = FunctionType::get(Type::getInt64Ty(Context), {I8Ptr}, false);

  EXPECT_THAT(Map,
              Contains(Pair(IsA<Function>(), TypedPointerType::get(FnTy, 0))));
  EXPECT_THAT(Map, Contains(Pair(IsA<Argument>(), I8Ptr)));
  */
}

