//===-- include/flang/Runtime/derived-api.h ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// API for lowering to use for operations on derived type objects.
// Initialiaztion and finalization are implied for pointer and allocatable
// ALLOCATE()/DEALLOCATE() respectively, so these APIs should be used only for
// local variables.  Whole allocatable assignment should use AllocatableAssign()
// instead of this Assign().

#ifndef FORTRAN_RUNTIME_DERIVED_API_H_
#define FORTRAN_RUNTIME_DERIVED_API_H_

#include "flang/Runtime/entry-names.h"

namespace Fortran::runtime {
class Descriptor;

namespace typeInfo {
class DerivedType;
}

extern "C" {

// Initializes and allocates an object's components, if it has a derived type
// with any default component initialization or automatic components.
// The descriptor must be initialized and non-null.
void RTDECL(Initialize)(
    const Descriptor &, const char *sourceFile = nullptr, int sourceLine = 0);

// Initializes an object clone from the original object.
// Each allocatable member of the clone is allocated with the same bounds as
// in the original object, if it is also allocated in it.
// The descriptor must be initialized and non-null.
void RTDECL(InitializeClone)(const Descriptor &, const Descriptor &,
    const char *sourceFile = nullptr, int sourceLine = 0);

// Finalizes an object and its components.  Deallocates any
// allocatable/automatic components.  Does not deallocate the descriptor's
// storage.
void RTDECL(Destroy)(const Descriptor &);

// Finalizes the object and its components.
void RTDECL(Finalize)(
    const Descriptor &, const char *sourceFile = nullptr, int sourceLine = 0);

/// Deallocates any allocatable/automatic components.
/// Does not deallocate the descriptor's storage.
/// Does not perform any finalization.
void RTDECL(DestroyWithoutFinalization)(const Descriptor &);

// Intrinsic or defined assignment, with scalar expansion but not type
// conversion.
void RTDECL(Assign)(const Descriptor &, const Descriptor &,
    const char *sourceFile = nullptr, int sourceLine = 0);

// Perform the test of the CLASS IS type guard statement of the SELECT TYPE
// construct.
bool RTDECL(ClassIs)(const Descriptor &, const typeInfo::DerivedType &);

// Perform the test of the SAME_TYPE_AS intrinsic.
bool RTDECL(SameTypeAs)(const Descriptor &, const Descriptor &);

// Perform the test of the EXTENDS_TYPE_OF intrinsic.
bool RTDECL(ExtendsTypeOf)(const Descriptor &, const Descriptor &);

} // extern "C"
} // namespace Fortran::runtime
#endif // FORTRAN_RUNTIME_DERIVED_API_H_
