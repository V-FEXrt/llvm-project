//===-- unittests/Runtime/Transformational.cpp ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "flang/Runtime/transformational.h"
#include "tools.h"
#include "gtest/gtest.h"
#include "flang-rt/runtime/type-code.h"
#include "flang/Common/float128.h"
#include <vector>

using namespace Fortran::runtime;
using Fortran::common::TypeCategory;

template <int KIND>
using BesselFuncType = std::function<void(Descriptor &, int32_t, int32_t,
    CppTypeFor<TypeCategory::Real, KIND>, CppTypeFor<TypeCategory::Real, KIND>,
    CppTypeFor<TypeCategory::Real, KIND>, const char *, int)>;

template <int KIND>
using BesselX0FuncType =
    std::function<void(Descriptor &, int32_t, int32_t, const char *, int)>;

template <int KIND>
constexpr CppTypeFor<TypeCategory::Real, KIND>
    besselEpsilon = CppTypeFor<TypeCategory::Real, KIND>(1e-4);

template <int KIND>
static void testBesselJn(BesselFuncType<KIND> rtFunc, int32_t n1, int32_t n2,
    CppTypeFor<TypeCategory::Real, KIND> x,
    const std::vector<CppTypeFor<TypeCategory::Real, KIND>> &expected) {
  StaticDescriptor desc;
  Descriptor &result{desc.descriptor()};
  unsigned len = expected.size();

  CppTypeFor<TypeCategory::Real, KIND> anc0 = len > 0 ? expected[len - 1] : 0.0;
  CppTypeFor<TypeCategory::Real, KIND> anc1 = len > 1 ? expected[len - 2] : 0.0;

  rtFunc(result, n1, n2, x, anc0, anc1, __FILE__, __LINE__);

  EXPECT_EQ(result.type(), (TypeCode{TypeCategory::Real, KIND}));
  EXPECT_EQ(result.rank(), 1);
  EXPECT_EQ(
      result.ElementBytes(), sizeof(CppTypeFor<TypeCategory::Real, KIND>));
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), len);

  for (size_t j{0}; j < len; ++j) {
    EXPECT_NEAR(
        (*result.ZeroBasedIndexedElement<CppTypeFor<TypeCategory::Real, KIND>>(
            j)),
        expected[j], besselEpsilon<KIND>);
  }
}

template <int KIND>
static void testBesselJnX0(
    BesselX0FuncType<KIND> rtFunc, int32_t n1, int32_t n2) {
  StaticDescriptor desc;
  Descriptor &result{desc.descriptor()};

  rtFunc(result, n1, n2, __FILE__, __LINE__);

  EXPECT_EQ(result.type(), (TypeCode{TypeCategory::Real, KIND}));
  EXPECT_EQ(result.rank(), 1);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), n2 >= n1 ? n2 - n1 + 1 : 0);

  if (n2 < n1) {
    return;
  }

  EXPECT_NEAR(
      (*result.ZeroBasedIndexedElement<CppTypeFor<TypeCategory::Real, KIND>>(
          0)),
      (n1 == 0) ? 1.0 : 0.0, 1e-5);

  for (int j{1}; j < (n2 - n1 + 1); ++j) {
    EXPECT_NEAR(
        (*result.ZeroBasedIndexedElement<CppTypeFor<TypeCategory::Real, KIND>>(
            j)),
        0.0, besselEpsilon<KIND>);
  }
}

template <int KIND> static void testBesselJn(BesselFuncType<KIND> rtFunc) {
  testBesselJn<KIND>(rtFunc, 1, 0, 1.0, {});
  testBesselJn<KIND>(rtFunc, 0, 0, 1.0, {0.765197694});
  testBesselJn<KIND>(rtFunc, 0, 1, 1.0, {0.765197694, 0.440050572});
  testBesselJn<KIND>(
      rtFunc, 0, 2, 1.0, {0.765197694, 0.440050572, 0.114903487});
  testBesselJn<KIND>(rtFunc, 1, 5, 5.0,
      {-0.327579111, 0.046565145, 0.364831239, 0.391232371, 0.261140555});
}

template <int KIND> static void testBesselJnX0(BesselX0FuncType<KIND> rtFunc) {
  testBesselJnX0<KIND>(rtFunc, 1, 0);
  testBesselJnX0<KIND>(rtFunc, 0, 0);
  testBesselJnX0<KIND>(rtFunc, 1, 1);
  testBesselJnX0<KIND>(rtFunc, 0, 3);
  testBesselJnX0<KIND>(rtFunc, 1, 4);
}

static void testBesselJn() {
  testBesselJn<4>(RTNAME(BesselJn_4));
  testBesselJn<8>(RTNAME(BesselJn_8));
#if HAS_FLOAT80
  testBesselJn<10>(RTNAME(BesselJn_10));
#endif
#if HAS_LDBL128 || HAS_FLOAT128
  testBesselJn<16>(RTNAME(BesselJn_16));
#endif

  testBesselJnX0<4>(RTNAME(BesselJnX0_4));
  testBesselJnX0<8>(RTNAME(BesselJnX0_8));
#if HAS_FLOAT80
  testBesselJnX0<10>(RTNAME(BesselJnX0_10));
#endif
#if HAS_LDBL128 || HAS_FLOAT128
  testBesselJnX0<16>(RTNAME(BesselJnX0_16));
#endif
}

TEST(Transformational, BesselJn) { testBesselJn(); }

template <int KIND>
static void testBesselYn(BesselFuncType<KIND> rtFunc, int32_t n1, int32_t n2,
    CppTypeFor<TypeCategory::Real, KIND> x,
    const std::vector<CppTypeFor<TypeCategory::Real, KIND>> &expected) {
  StaticDescriptor desc;
  Descriptor &result{desc.descriptor()};
  unsigned len = expected.size();

  CppTypeFor<TypeCategory::Real, KIND> anc0 = len > 0 ? expected[0] : 0.0;
  CppTypeFor<TypeCategory::Real, KIND> anc1 = len > 1 ? expected[1] : 0.0;

  rtFunc(result, n1, n2, x, anc0, anc1, __FILE__, __LINE__);

  EXPECT_EQ(result.type(), (TypeCode{TypeCategory::Real, KIND}));
  EXPECT_EQ(result.rank(), 1);
  EXPECT_EQ(
      result.ElementBytes(), sizeof(CppTypeFor<TypeCategory::Real, KIND>));
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), len);

  for (size_t j{0}; j < len; ++j) {
    EXPECT_NEAR(
        (*result.ZeroBasedIndexedElement<CppTypeFor<TypeCategory::Real, KIND>>(
            j)),
        expected[j], besselEpsilon<KIND>);
  }
}

template <int KIND>
static void testBesselYnX0(
    BesselX0FuncType<KIND> rtFunc, int32_t n1, int32_t n2) {
  StaticDescriptor<2> desc;
  Descriptor &result{desc.descriptor()};

  rtFunc(result, n1, n2, __FILE__, __LINE__);

  EXPECT_EQ(result.type(), (TypeCode{TypeCategory::Real, KIND}));
  EXPECT_EQ(result.rank(), 1);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), n2 >= n1 ? n2 - n1 + 1 : 0);

  if (n2 < n1) {
    return;
  }

  for (int j{0}; j < (n2 - n1 + 1); ++j) {
    EXPECT_EQ(
        (*result.ZeroBasedIndexedElement<CppTypeFor<TypeCategory::Real, KIND>>(
            j)),
        (-std::numeric_limits<
            CppTypeFor<TypeCategory::Real, KIND>>::infinity()));
  }
}

template <int KIND> static void testBesselYn(BesselFuncType<KIND> rtFunc) {
  testBesselYn<KIND>(rtFunc, 1, 0, 1.0, {});
  testBesselYn<KIND>(rtFunc, 0, 0, 1.0, {0.08825695});
  testBesselYn<KIND>(rtFunc, 0, 1, 1.0, {0.08825695, -0.7812128});
  testBesselYn<KIND>(rtFunc, 0, 2, 1.0, {0.0882569555, -0.7812128, -1.6506826});
  testBesselYn<KIND>(rtFunc, 1, 5, 1.0,
      {-0.7812128, -1.6506826, -5.8215175, -33.278423, -260.40588});
}

template <int KIND> static void testBesselYnX0(BesselX0FuncType<KIND> rtFunc) {
  testBesselYnX0<KIND>(rtFunc, 1, 0);
  testBesselYnX0<KIND>(rtFunc, 0, 0);
  testBesselYnX0<KIND>(rtFunc, 1, 1);
  testBesselYnX0<KIND>(rtFunc, 0, 3);
  testBesselYnX0<KIND>(rtFunc, 1, 4);
}

static void testBesselYn() {
  testBesselYn<4>(RTNAME(BesselYn_4));
  testBesselYn<8>(RTNAME(BesselYn_8));
#if HAS_FLOAT80
  testBesselYn<10>(RTNAME(BesselYn_10));
#endif
#if HAS_LDBL128 || HAS_FLOAT128
  testBesselYn<16>(RTNAME(BesselYn_16));
#endif

  testBesselYnX0<4>(RTNAME(BesselYnX0_4));
  testBesselYnX0<8>(RTNAME(BesselYnX0_8));
#if HAS_FLOAT80
  testBesselYnX0<10>(RTNAME(BesselYnX0_10));
#endif
#if HAS_LDBL128 || HAS_FLOAT128
  testBesselYnX0<16>(RTNAME(BesselYnX0_16));
#endif
}

TEST(Transformational, BesselYn) { testBesselYn(); }

TEST(Transformational, Shifts) {
  // ARRAY  1 3 5
  //        2 4 6
  auto array{MakeArray<TypeCategory::Integer, 4>(
      std::vector<int>{2, 3}, std::vector<std::int32_t>{1, 2, 3, 4, 5, 6})};
  array->GetDimension(0).SetLowerBound(0); // shouldn't matter
  array->GetDimension(1).SetLowerBound(-1);
  StaticDescriptor<2, true> statDesc;
  Descriptor &result{statDesc.descriptor()};

  auto shift3{MakeArray<TypeCategory::Integer, 8>(
      std::vector<int>{3}, std::vector<std::int64_t>{1, -1, 2})};
  RTNAME(Cshift)(result, *array, *shift3, 1, __FILE__, __LINE__);
  EXPECT_EQ(result.type(), array->type());
  EXPECT_EQ(result.rank(), 2);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), 2);
  EXPECT_EQ(result.GetDimension(1).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(1).Extent(), 3);
  EXPECT_EQ(result.type(), (TypeCode{TypeCategory::Integer, 4}));
  static std::int32_t cshiftExpect1[6]{2, 1, 4, 3, 5, 6};
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(
        *result.ZeroBasedIndexedElement<std::int32_t>(j), cshiftExpect1[j]);
  }
  result.Destroy();

  auto shift2{MakeArray<TypeCategory::Integer, 1>(
      std::vector<int>{2}, std::vector<std::int8_t>{1, -1})};
  shift2->GetDimension(0).SetLowerBound(-1); // shouldn't matter
  RTNAME(Cshift)(result, *array, *shift2, 2, __FILE__, __LINE__);
  EXPECT_EQ(result.type(), array->type());
  EXPECT_EQ(result.rank(), 2);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), 2);
  EXPECT_EQ(result.GetDimension(1).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(1).Extent(), 3);
  EXPECT_EQ(result.type(), (TypeCode{TypeCategory::Integer, 4}));
  static std::int32_t cshiftExpect2[6]{3, 6, 5, 2, 1, 4};
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(
        *result.ZeroBasedIndexedElement<std::int32_t>(j), cshiftExpect2[j]);
  }
  result.Destroy();

  // VECTOR  1 3 5 2 4 6
  auto vector{MakeArray<TypeCategory::Integer, 4>(
      std::vector<int>{6}, std::vector<std::int32_t>{1, 2, 3, 4, 5, 6})};
  vector->GetDimension(0).SetLowerBound(0);
  StaticDescriptor<1, true> vectorDesc;
  Descriptor &vectorResult{vectorDesc.descriptor()};

  RTNAME(CshiftVector)(vectorResult, *vector, 2, __FILE__, __LINE__);
  EXPECT_EQ(vectorResult.type(), array->type());
  EXPECT_EQ(vectorResult.rank(), 1);
  EXPECT_EQ(vectorResult.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(vectorResult.GetDimension(0).Extent(), 6);
  EXPECT_EQ(vectorResult.type(), (TypeCode{TypeCategory::Integer, 4}));
  static std::int32_t cshiftExpect3[6]{3, 4, 5, 6, 1, 2};
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(*vectorResult.ZeroBasedIndexedElement<std::int32_t>(j),
        cshiftExpect3[j]);
  }
  vectorResult.Destroy();

  RTNAME(CshiftVector)(vectorResult, *vector, -2, __FILE__, __LINE__);
  EXPECT_EQ(vectorResult.type(), array->type());
  EXPECT_EQ(vectorResult.rank(), 1);
  EXPECT_EQ(vectorResult.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(vectorResult.GetDimension(0).Extent(), 6);
  EXPECT_EQ(vectorResult.type(), (TypeCode{TypeCategory::Integer, 4}));
  static std::int32_t cshiftExpect4[6]{5, 6, 1, 2, 3, 4};
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(*vectorResult.ZeroBasedIndexedElement<std::int32_t>(j),
        cshiftExpect4[j]);
  }
  vectorResult.Destroy();

  // VECTOR  1 3 5 2 4 6 WITH non zero lower bound in a negative cshift.
  auto vectorWithLowerBounds{MakeArray<TypeCategory::Integer, 4>(
      std::vector<int>{6}, std::vector<std::int32_t>{1, 2, 3, 4, 5, 6})};
  vectorWithLowerBounds->GetDimension(0).SetLowerBound(2);

  RTNAME(CshiftVector)
  (vectorResult, *vectorWithLowerBounds, -2, __FILE__, __LINE__);
  EXPECT_EQ(vectorResult.type(), array->type());
  EXPECT_EQ(vectorResult.rank(), 1);
  EXPECT_EQ(vectorResult.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(vectorResult.GetDimension(0).Extent(), 6);
  EXPECT_EQ(vectorResult.type(), (TypeCode{TypeCategory::Integer, 4}));
  static std::int32_t cshiftExpect5[6]{5, 6, 1, 2, 3, 4};
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(*vectorResult.ZeroBasedIndexedElement<std::int32_t>(j),
        cshiftExpect5[j]);
  }
  vectorResult.Destroy();

  auto boundary{MakeArray<TypeCategory::Integer, 4>(
      std::vector<int>{3}, std::vector<std::int32_t>{-1, -2, -3})};
  boundary->GetDimension(0).SetLowerBound(9); // shouldn't matter
  RTNAME(Eoshift)(result, *array, *shift3, &*boundary, 1, __FILE__, __LINE__);
  EXPECT_EQ(result.type(), array->type());
  EXPECT_EQ(result.rank(), 2);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), 2);
  EXPECT_EQ(result.GetDimension(1).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(1).Extent(), 3);
  EXPECT_EQ(result.type(), (TypeCode{TypeCategory::Integer, 4}));
  static std::int32_t eoshiftExpect1[6]{2, -1, -2, 3, -3, -3};
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(
        *result.ZeroBasedIndexedElement<std::int32_t>(j), eoshiftExpect1[j]);
  }
  result.Destroy();

  // VECTOR EOSHIFT
  StaticDescriptor<0> boundaryDescriptor;
  Descriptor vectorBoundary{boundaryDescriptor.descriptor()};
  std::int32_t boundaryValue{343};
  vectorBoundary.Establish(TypeCategory::Integer, 4,
      const_cast<void *>(reinterpret_cast<const void *>(&boundaryValue)), 0);
  RTNAME(EoshiftVector)
  (vectorResult, *vector, 2, &vectorBoundary, __FILE__, __LINE__);
  EXPECT_EQ(vectorResult.type(), array->type());
  EXPECT_EQ(vectorResult.rank(), 1);
  EXPECT_EQ(vectorResult.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(vectorResult.GetDimension(0).Extent(), 6);
  EXPECT_EQ(vectorResult.type(), (TypeCode{TypeCategory::Integer, 4}));
  static std::int32_t eoshiftVectorExpect[6]{3, 4, 5, 6, 343, 343};
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(*vectorResult.ZeroBasedIndexedElement<std::int32_t>(j),
        eoshiftVectorExpect[j]);
  }
  vectorResult.Destroy();

  // VECTOR EOSHIFT on input with non zero lower bounds
  RTNAME(EoshiftVector)
  (vectorResult, *vectorWithLowerBounds, -2, &vectorBoundary, __FILE__,
      __LINE__);
  EXPECT_EQ(vectorResult.type(), array->type());
  EXPECT_EQ(vectorResult.rank(), 1);
  EXPECT_EQ(vectorResult.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(vectorResult.GetDimension(0).Extent(), 6);
  EXPECT_EQ(vectorResult.type(), (TypeCode{TypeCategory::Integer, 4}));
  static std::int32_t eoshiftVectorExpect2[6]{343, 343, 1, 2, 3, 4};
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(*vectorResult.ZeroBasedIndexedElement<std::int32_t>(j),
        eoshiftVectorExpect2[j]);
  }
  vectorResult.Destroy();
}

TEST(Transformational, Pack) {
  // ARRAY  1 3 5
  //        2 4 6
  auto array{MakeArray<TypeCategory::Integer, 4>(
      std::vector<int>{2, 3}, std::vector<std::int32_t>{1, 2, 3, 4, 5, 6})};
  array->GetDimension(0).SetLowerBound(2); // shouldn't matter
  array->GetDimension(1).SetLowerBound(-1);
  auto mask{MakeArray<TypeCategory::Logical, 1>(std::vector<int>{2, 3},
      std::vector<std::uint8_t>{false, true, true, false, false, true})};
  mask->GetDimension(0).SetLowerBound(0); // shouldn't matter
  mask->GetDimension(1).SetLowerBound(2);
  StaticDescriptor<maxRank, true> statDesc;
  Descriptor &result{statDesc.descriptor()};

  RTNAME(Pack)(result, *array, *mask, nullptr, __FILE__, __LINE__);
  EXPECT_EQ(result.type(), array->type());
  EXPECT_EQ(result.rank(), 1);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), 3);
  static std::int32_t packExpect1[3]{2, 3, 6};
  for (int j{0}; j < 3; ++j) {
    EXPECT_EQ(*result.ZeroBasedIndexedElement<std::int32_t>(j), packExpect1[j])
        << " at " << j;
  }
  result.Destroy();

  auto vector{MakeArray<TypeCategory::Integer, 4>(
      std::vector<int>{5}, std::vector<std::int32_t>{-1, -2, -3, -4, -5})};
  RTNAME(Pack)(result, *array, *mask, &*vector, __FILE__, __LINE__);
  EXPECT_EQ(result.type(), array->type());
  EXPECT_EQ(result.rank(), 1);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), 5);
  static std::int32_t packExpect2[5]{2, 3, 6, -4, -5};
  for (int j{0}; j < 5; ++j) {
    EXPECT_EQ(*result.ZeroBasedIndexedElement<std::int32_t>(j), packExpect2[j])
        << " at " << j;
  }
  result.Destroy();
}

TEST(Transformational, Spread) {
  auto array{MakeArray<TypeCategory::Integer, 4>(
      std::vector<int>{3}, std::vector<std::int32_t>{1, 2, 3})};
  array->GetDimension(0).SetLowerBound(2); // shouldn't matter
  StaticDescriptor<2, true> statDesc;
  Descriptor &result{statDesc.descriptor()};

  RTNAME(Spread)(result, *array, 1, 2, __FILE__, __LINE__);
  EXPECT_EQ(result.type(), array->type());
  EXPECT_EQ(result.rank(), 2);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), 2);
  EXPECT_EQ(result.GetDimension(1).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(1).Extent(), 3);
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(*result.ZeroBasedIndexedElement<std::int32_t>(j), 1 + j / 2);
  }
  result.Destroy();

  RTNAME(Spread)(result, *array, 2, 2, __FILE__, __LINE__);
  EXPECT_EQ(result.type(), array->type());
  EXPECT_EQ(result.rank(), 2);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), 3);
  EXPECT_EQ(result.GetDimension(1).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(1).Extent(), 2);
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(*result.ZeroBasedIndexedElement<std::int32_t>(j), 1 + j % 3);
  }
  result.Destroy();

  auto scalar{MakeArray<TypeCategory::Integer, 4>(
      std::vector<int>{}, std::vector<std::int32_t>{1})};
  RTNAME(Spread)(result, *scalar, 1, 2, __FILE__, __LINE__);
  EXPECT_EQ(result.type(), array->type());
  EXPECT_EQ(result.rank(), 1);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), 2);
  for (int j{0}; j < 2; ++j) {
    EXPECT_EQ(*result.ZeroBasedIndexedElement<std::int32_t>(j), 1);
  }
  result.Destroy();
}

TEST(Transformational, Transpose) {
  // ARRAY  1 3 5
  //        2 4 6
  auto array{MakeArray<TypeCategory::Integer, 4>(
      std::vector<int>{2, 3}, std::vector<std::int32_t>{1, 2, 3, 4, 5, 6})};
  array->GetDimension(0).SetLowerBound(2); // shouldn't matter
  array->GetDimension(1).SetLowerBound(-6);
  StaticDescriptor<2, true> statDesc;
  Descriptor &result{statDesc.descriptor()};
  RTNAME(Transpose)(result, *array, __FILE__, __LINE__);
  EXPECT_EQ(result.type(), array->type());
  EXPECT_EQ(result.rank(), 2);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), 3);
  EXPECT_EQ(result.GetDimension(1).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(1).Extent(), 2);
  static std::int32_t expect[6]{1, 3, 5, 2, 4, 6};
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(*result.ZeroBasedIndexedElement<std::int32_t>(j), expect[j]);
  }
  result.Destroy();
}

TEST(Transformational, Unpack) {
  auto vector{MakeArray<TypeCategory::Integer, 4>(
      std::vector<int>{4}, std::vector<std::int32_t>{1, 2, 3, 4})};
  vector->GetDimension(0).SetLowerBound(2); // shouldn't matter
  auto mask{MakeArray<TypeCategory::Logical, 1>(std::vector<int>{2, 3},
      std::vector<std::uint8_t>{false, true, true, false, false, true})};
  mask->GetDimension(0).SetLowerBound(0); // shouldn't matter
  mask->GetDimension(1).SetLowerBound(2);
  auto field{MakeArray<TypeCategory::Integer, 4>(std::vector<int>{2, 3},
      std::vector<std::int32_t>{-1, -2, -3, -4, -5, -6})};
  field->GetDimension(0).SetLowerBound(-1); // shouldn't matter
  StaticDescriptor<2, true> statDesc;
  Descriptor &result{statDesc.descriptor()};
  RTNAME(Unpack)(result, *vector, *mask, *field, __FILE__, __LINE__);
  EXPECT_EQ(result.type(), vector->type());
  EXPECT_EQ(result.rank(), 2);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), 2);
  EXPECT_EQ(result.GetDimension(1).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(1).Extent(), 3);
  static std::int32_t expect[6]{-1, 1, 2, -4, -5, 3};
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(*result.ZeroBasedIndexedElement<std::int32_t>(j), expect[j]);
  }
  result.Destroy();

  // Test for scalar value of the "field" argument
  auto scalarField{MakeArray<TypeCategory::Integer, 4>(
      std::vector<int>{}, std::vector<std::int32_t>{343})};
  RTNAME(Unpack)(result, *vector, *mask, *scalarField, __FILE__, __LINE__);
  EXPECT_EQ(result.rank(), 2);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), 2);
  EXPECT_EQ(result.GetDimension(1).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(1).Extent(), 3);
  static std::int32_t scalarExpect[6]{343, 1, 2, 343, 343, 3};
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(
        *result.ZeroBasedIndexedElement<std::int32_t>(j), scalarExpect[j]);
  }
  result.Destroy();
}

#if HAS_FLOAT80
// Make sure the destination descriptor is created by the runtime
// with proper element size, when REAL*10 maps to 'long double'.
#define Real10CppType long double
TEST(Transformational, TransposeReal10) {
  // ARRAY  1 3 5
  //        2 4 6
  auto array{MakeArray<TypeCategory::Real, 10>(std::vector<int>{2, 3},
      std::vector<Real10CppType>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0},
      sizeof(Real10CppType))};
  StaticDescriptor<2, true> statDesc;
  Descriptor &result{statDesc.descriptor()};
  RTNAME(Transpose)(result, *array, __FILE__, __LINE__);
  EXPECT_EQ(result.ElementBytes(), sizeof(Real10CppType));
  EXPECT_EQ(result.type(), array->type());
  EXPECT_EQ(result.rank(), 2);
  EXPECT_EQ(result.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(0).Extent(), 3);
  EXPECT_EQ(result.GetDimension(1).LowerBound(), 1);
  EXPECT_EQ(result.GetDimension(1).Extent(), 2);
  static Real10CppType expect[6]{1.0, 3.0, 5.0, 2.0, 4.0, 6.0};
  for (int j{0}; j < 6; ++j) {
    EXPECT_EQ(*result.ZeroBasedIndexedElement<Real10CppType>(j), expect[j]);
  }
  result.Destroy();
}
#endif

TEST(Transformational, ShallowCopy) {
  auto charArray{MakeArray<TypeCategory::Character, 1>(std::vector<int>{2, 3},
      std::vector<std::string>{"ab", "cd", "ef", "gh", "ij", "kl"}, 2)};
  charArray->GetDimension(0).SetBounds(-1, 0);
  charArray->GetDimension(1).SetBounds(3, 5);
  StaticDescriptor<2> staticCharResult;
  Descriptor &charResult{staticCharResult.descriptor()};

  // Test allocating ShallowCopy.
  RTNAME(ShallowCopy)(charResult, *charArray);
  ASSERT_TRUE(charResult.IsAllocated());
  ASSERT_TRUE(charResult.IsContiguous());
  ASSERT_EQ(charResult.type(), charArray->type());
  ASSERT_EQ(charResult.ElementBytes(), 2u);
  EXPECT_EQ(charResult.GetDimension(0).LowerBound(), 1);
  EXPECT_EQ(charResult.GetDimension(0).Extent(), 2);
  EXPECT_EQ(charResult.GetDimension(1).LowerBound(), 1);
  EXPECT_EQ(charResult.GetDimension(1).Extent(), 3);
  std::string expectedCharResult{"abcdefghijkl"};
  EXPECT_EQ(std::memcmp(charResult.OffsetElement<char>(0),
                expectedCharResult.data(), expectedCharResult.length()),
      0);

  // Test ShallowCopyDirect with pre-allocated result.
  char *allocatedPtr = charResult.OffsetElement<char>(0);
  std::memset(
      charResult.OffsetElement<char>(0), 'x', expectedCharResult.length());
  // Set new lower bounds for charResult.
  charResult.GetDimension(0).SetBounds(-2, -1);
  charResult.GetDimension(1).SetBounds(2, 4);
  RTNAME(ShallowCopyDirect)(charResult, *charArray);
  ASSERT_TRUE(charResult.IsAllocated());
  ASSERT_TRUE(charResult.IsContiguous());
  ASSERT_EQ(charResult.type(), charArray->type());
  ASSERT_EQ(charResult.ElementBytes(), 2u);
  EXPECT_EQ(charResult.GetDimension(0).LowerBound(), -2);
  EXPECT_EQ(charResult.GetDimension(0).Extent(), 2);
  EXPECT_EQ(charResult.GetDimension(1).LowerBound(), 2);
  EXPECT_EQ(charResult.GetDimension(1).Extent(), 3);
  // Test that the result was not re-allocated.
  EXPECT_EQ(allocatedPtr, charResult.OffsetElement<char>(0));
  EXPECT_EQ(std::memcmp(charResult.OffsetElement<char>(0),
                expectedCharResult.data(), expectedCharResult.length()),
      0);
  charResult.Destroy();

  auto intScalar{MakeArray<TypeCategory::Integer, 4>(
      std::vector<int>{}, std::vector<std::int32_t>{-1})};
  StaticDescriptor<0> staticIntResult;
  Descriptor &intResult{staticIntResult.descriptor()};
  RTNAME(ShallowCopy)(intResult, *intScalar);
  ASSERT_TRUE(intResult.IsAllocated());
  ASSERT_EQ(intResult.rank(), 0);
  ASSERT_EQ(*intResult.ZeroBasedIndexedElement<std::int32_t>(0), -1);
  *intResult.ZeroBasedIndexedElement<std::int32_t>(0) = 0;
  allocatedPtr = intResult.OffsetElement<char>(0);
  RTNAME(ShallowCopyDirect)(intResult, *intScalar);
  ASSERT_TRUE(intResult.IsAllocated());
  ASSERT_EQ(intResult.rank(), 0);
  ASSERT_EQ(*intResult.ZeroBasedIndexedElement<std::int32_t>(0), -1);
  EXPECT_EQ(allocatedPtr, intResult.OffsetElement<char>(0));
  intResult.Destroy();
}
