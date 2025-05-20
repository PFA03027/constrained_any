/**
 * @file test_constrained_any.cpp
 * @author Teruaki Ata (PFA03027@nifty.com)
 * @brief
 * @version 0.1
 * @date 2025-04-26
 *
 * @copyright Copyright (c) 2025, Teruaki Ata (PFA03027@nifty.com)
 *
 */

#include <map>
#include <unordered_map>

#include "constrained_any.hpp"

#include <gtest/gtest.h>

// ================================================

struct TestCopyOnlyType {
	int v_;

	TestCopyOnlyType()
	  : v_( 0 ) {}
	TestCopyOnlyType( const TestCopyOnlyType& )            = default;
	TestCopyOnlyType( TestCopyOnlyType&& )                 = delete;
	TestCopyOnlyType& operator=( const TestCopyOnlyType& ) = default;
	TestCopyOnlyType& operator=( TestCopyOnlyType&& )      = delete;

	TestCopyOnlyType( int v )
	  : v_( v ) {}
};

struct TestMoveOnlyType {
	int v_;

	TestMoveOnlyType()
	  : v_( 0 ) {}
	TestMoveOnlyType( const TestMoveOnlyType& )            = delete;
	TestMoveOnlyType( TestMoveOnlyType&& )                 = default;
	TestMoveOnlyType& operator=( const TestMoveOnlyType& ) = delete;
	TestMoveOnlyType& operator=( TestMoveOnlyType&& )      = default;

	TestMoveOnlyType( int v )
	  : v_( v ) {}
};

struct TestCopyAndMoveType {
	int v_;

	TestCopyAndMoveType()
	  : v_( 0 ) {}
	TestCopyAndMoveType( const TestCopyAndMoveType& ) = default;
	TestCopyAndMoveType( TestCopyAndMoveType&& src )
	  : v_( src.v_ )
	{
		src.v_ = 0;
	}
	TestCopyAndMoveType& operator=( const TestCopyAndMoveType& ) = default;
	TestCopyAndMoveType& operator=( TestCopyAndMoveType&& src )
	{
		v_     = src.v_;
		src.v_ = 0;
		return *this;
	}

	TestCopyAndMoveType( int v )
	  : v_( v ) {}
};

struct TestCopyConstructOnly {
	int v_;

	TestCopyConstructOnly()
	  : v_( 0 ) {}
	TestCopyConstructOnly( const TestCopyConstructOnly& )            = default;
	TestCopyConstructOnly( TestCopyOnlyType&& )                      = delete;
	TestCopyConstructOnly& operator=( const TestCopyConstructOnly& ) = delete;
	TestCopyConstructOnly& operator=( TestCopyConstructOnly&& )      = delete;

	TestCopyConstructOnly( int v )
	  : v_( v ) {}
};

struct TestMoveConstructOnly {
	int v_;

	TestMoveConstructOnly()
	  : v_( 0 ) {}
	TestMoveConstructOnly( const TestMoveConstructOnly& )            = delete;
	TestMoveConstructOnly( TestMoveConstructOnly&& )                 = default;
	TestMoveConstructOnly& operator=( const TestMoveConstructOnly& ) = delete;
	TestMoveConstructOnly& operator=( TestMoveConstructOnly&& )      = delete;

	TestMoveConstructOnly( int v )
	  : v_( v ) {}
};

struct TestCopyMoveConstructOnly {
	int v_;

	TestCopyMoveConstructOnly()
	  : v_( 0 ) {}
	TestCopyMoveConstructOnly( const TestCopyMoveConstructOnly& )            = default;
	TestCopyMoveConstructOnly( TestCopyMoveConstructOnly&& )                 = default;
	TestCopyMoveConstructOnly& operator=( const TestCopyMoveConstructOnly& ) = delete;
	TestCopyMoveConstructOnly& operator=( TestCopyMoveConstructOnly&& )      = delete;

	TestCopyMoveConstructOnly( int v )
	  : v_( v ) {}
};

// ================================================

template <typename T>
struct no_specialoperation {
	static constexpr bool constraint_check_result = true;
};

template <typename T>
struct no_specialoperation2 {
	static constexpr bool constraint_check_result = true;
};

template <typename T>
struct constrained_alway_false {
	static constexpr bool constraint_check_result = false;
};

static_assert( yan::is_specialized_of_constrained_any<int>::value == false, "int is not a constrained_any" );
static_assert( yan::is_specialized_of_constrained_any<yan::constrained_any<>>::value, "constrained_any is specialized type of constrained_any" );
static_assert( yan::is_specialized_of_constrained_any<yan::constrained_any<no_specialoperation>>::value, "constrained_any is specialized type of constrained_any" );
static_assert( yan::is_specialized_of_constrained_any<yan::constrained_any<no_specialoperation, no_specialoperation2>>::value, "constrained_any is specialized type of constrained_any" );
static_assert( yan::is_specialized_of_constrained_any<yan::constrained_any<no_specialoperation, no_specialoperation2, constrained_alway_false>>::value, "constrained_any is specialized type of constrained_any" );

static_assert( yan::is_value_carrier_of_constrained_any<int>::value == false, "int is not a constrained_any" );
static_assert( yan::is_value_carrier_of_constrained_any<yan::impl::value_carrier<int, true, true>>::value == true, "yan::impl::value_carrier<int,true> is a value carrier type of constrained_any" );
static_assert( yan::is_value_carrier_of_constrained_any<yan::impl::value_carrier<int, true, true, no_specialoperation>>::value == true, "yan::impl::value_carrier<int,true,no_specialoperation> is a value carrier type of constrained_any" );

static_assert( yan::is_related_type_of_constrained_any<int>::value == false, "int should not be constrained_any related type" );
static_assert( yan::is_related_type_of_constrained_any<yan::impl::value_carrier<int, true, true>>::value == true, "yan::impl::value_carrier<int, true> should be constrained_any related type" );
static_assert( yan::is_related_type_of_constrained_any<yan::impl::value_carrier<int, true, true, no_specialoperation>>::value == true, "yan::impl::value_carrier<int, true, no_specialoperation> should be constrained_any related type" );
static_assert( yan::is_related_type_of_constrained_any<yan::constrained_any<>>::value == true, "constrained_any should be constrained_any related type" );
static_assert( yan::is_related_type_of_constrained_any<yan::constrained_any<no_specialoperation>>::value == true, "constrained_any should be constrained_any related type" );
static_assert( yan::is_related_type_of_constrained_any<yan::constrained_any<no_specialoperation, no_specialoperation2>>::value == true, "constrained_any should be constrained_any related type" );
static_assert( yan::is_related_type_of_constrained_any<yan::constrained_any<no_specialoperation, no_specialoperation2, constrained_alway_false>>::value == true, "constrained_any should be constrained_any related type" );

static_assert( yan::impl::is_acceptable_value_type<yan::constrained_any<>, yan::impl::special_operation_copyable>::value == false, "constrained_any should not be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<yan::constrained_any<no_specialoperation>, yan::impl::special_operation_copyable>::value == false, "constrained_any should not be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<yan::constrained_any<no_specialoperation, no_specialoperation2>, yan::impl::special_operation_copyable>::value == false, "constrained_any should not be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<yan::constrained_any<no_specialoperation, no_specialoperation2, constrained_alway_false>, yan::impl::special_operation_copyable>::value == false, "constrained_any should not be acceptable type" );
// static_assert( yan::impl::is_acceptable_value_type<yan::impl::value_carrier<int, true>, true>::value == false, "value carrier of constrained_any should not be acceptable type" );

static_assert( yan::impl::is_acceptable_value_type<int, yan::impl::special_operation_copyable>::value == true, "int should be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<int, yan::impl::special_operation_copyable, no_specialoperation>::value == true, "int should be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<int, yan::impl::special_operation_copyable, no_specialoperation, no_specialoperation2>::value == true, "int should be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<int, yan::impl::special_operation_copyable, no_specialoperation, no_specialoperation2, constrained_alway_false>::value == false, "int should be acceptable type" );

template <typename T>
struct copy_construction_required1 {
};

template <typename T>
struct copy_construction_required2 {
	static constexpr bool require_copy_constructible = false;
};

template <typename T>
struct copy_construction_required3 {
	static constexpr bool require_copy_constructible = true;
};

static_assert( yan::impl::are_any_constraints_required_copy_constructible<>::value == false, "copy constructible is not required" );
static_assert( yan::impl::are_any_constraints_required_copy_constructible<copy_construction_required1>::value == false, "copy constructible is not required" );
static_assert( yan::impl::are_any_constraints_required_copy_constructible<copy_construction_required1, copy_construction_required2>::value == false, "copy constructible is not required" );
static_assert( yan::impl::are_any_constraints_required_copy_constructible<copy_construction_required1, copy_construction_required2, copy_construction_required3>::value == true, "copy constructible is required" );

static_assert( yan::impl::is_satisfy_required_copy_constructible_constraint<int, copy_construction_required1>::value == true, "should not require require_copy_constructible" );
static_assert( yan::impl::is_satisfy_required_copy_constructible_constraint<int, copy_construction_required1, copy_construction_required2>::value == true, "should not require require_copy_constructible" );
static_assert( yan::impl::is_satisfy_required_copy_constructible_constraint<int, copy_construction_required1, copy_construction_required2, copy_construction_required3>::value == true, "should require require_copy_constructible" );

static_assert( yan::impl::is_satisfy_required_copy_constructible_constraint<TestCopyOnlyType, copy_construction_required1>::value == true, "T should satisfy require_copy_constructible" );
static_assert( yan::impl::is_satisfy_required_copy_constructible_constraint<TestCopyOnlyType, copy_construction_required1, copy_construction_required2>::value == true, "T should satisfy require require_copy_constructible" );
static_assert( yan::impl::is_satisfy_required_copy_constructible_constraint<TestCopyOnlyType, copy_construction_required1, copy_construction_required2, copy_construction_required3>::value == true, "T should satisfy require require_copy_constructible" );

static_assert( yan::impl::is_satisfy_required_copy_constructible_constraint<TestMoveOnlyType, copy_construction_required1>::value == true, "T should satisfy require_copy_constructible" );
static_assert( yan::impl::is_satisfy_required_copy_constructible_constraint<TestMoveOnlyType, copy_construction_required1, copy_construction_required2>::value == true, "T should satisfy require require_copy_constructible" );
static_assert( yan::impl::is_satisfy_required_copy_constructible_constraint<TestMoveOnlyType, copy_construction_required1, copy_construction_required2, copy_construction_required3>::value == false, "T should not satisfy require require_copy_constructible" );

template <typename T>
struct move_construction_required1 {
};

template <typename T>
struct move_construction_required2 {
	static constexpr bool require_move_constructible = false;
};

template <typename T>
struct move_construction_required3 {
	static constexpr bool require_move_constructible = true;
};

static_assert( yan::impl::are_any_constraints_required_move_constructible<>::value == false, "copy constructible is not required" );
static_assert( yan::impl::are_any_constraints_required_move_constructible<move_construction_required1>::value == false, "copy constructible is not required" );
static_assert( yan::impl::are_any_constraints_required_move_constructible<move_construction_required1, move_construction_required2>::value == false, "copy constructible is not required" );
static_assert( yan::impl::are_any_constraints_required_move_constructible<move_construction_required1, move_construction_required2, move_construction_required3>::value == true, "copy constructible is required" );

static_assert( yan::impl::is_satisfy_required_move_constructible_constraint<int, move_construction_required1>::value == true, "should not require require_move_constructible" );
static_assert( yan::impl::is_satisfy_required_move_constructible_constraint<int, move_construction_required1, move_construction_required2>::value == true, "should not require require_move_constructible" );
static_assert( yan::impl::is_satisfy_required_move_constructible_constraint<int, move_construction_required1, move_construction_required2, move_construction_required3>::value == true, "should require require_move_constructible" );

static_assert( yan::impl::is_satisfy_required_move_constructible_constraint<TestCopyOnlyType, move_construction_required1>::value == true, "should not require require_move_constructible" );
static_assert( yan::impl::is_satisfy_required_move_constructible_constraint<TestCopyOnlyType, move_construction_required1, move_construction_required2>::value == true, "should not require require_move_constructible" );
static_assert( yan::impl::is_satisfy_required_move_constructible_constraint<TestCopyOnlyType, move_construction_required1, move_construction_required2, move_construction_required3>::value == false, "should require require_move_constructible" );

static_assert( yan::impl::is_satisfy_required_move_constructible_constraint<TestMoveOnlyType, move_construction_required1>::value == true, "should not require require_move_constructible" );
static_assert( yan::impl::is_satisfy_required_move_constructible_constraint<TestMoveOnlyType, move_construction_required1, move_construction_required2>::value == true, "should not require require_move_constructible" );
static_assert( yan::impl::is_satisfy_required_move_constructible_constraint<TestMoveOnlyType, move_construction_required1, move_construction_required2, move_construction_required3>::value == true, "should require require_move_constructible" );

static_assert( yan::impl::is_acceptable_value_type<int, copy_construction_required3, move_construction_required3>::value == true, "int should be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<TestCopyOnlyType, copy_construction_required3, move_construction_required3>::value == true, "int should be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<TestMoveOnlyType, copy_construction_required3, move_construction_required3>::value == false, "int should be acceptable type" );

static_assert( yan::impl::is_acceptable_value_type<int, yan::impl::special_operation_less>::value == true, "int should be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<TestCopyOnlyType, yan::impl::special_operation_less>::value == false, "int should be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<TestMoveOnlyType, yan::impl::special_operation_less>::value == false, "int should be acceptable type" );

static_assert( yan::impl::are_any_constraints_required_copy_constructible<yan::impl::special_operation_copyable>::value == true, "yan::impl::special_operation_copyable should require copy constructible" );
static_assert( yan::impl::are_any_constraints_required_move_constructible<yan::impl::special_operation_movable>::value == true, "yan::impl::special_operation_movable should require move constructible" );

static_assert( yan::impl::is_acceptable_value_type<TestCopyConstructOnly, yan::impl::special_operation_copyable>::value == true, "TestCopyConstructOnly should be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<TestCopyConstructOnly, yan::impl::special_operation_movable>::value == true, "TestCopyConstructOnly should be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<TestMoveConstructOnly, yan::impl::special_operation_copyable>::value == false, "TestCopyConstructOnly should be acceptable type" );
static_assert( yan::impl::is_acceptable_value_type<TestMoveConstructOnly, yan::impl::special_operation_movable>::value == true, "TestCopyConstructOnly should be acceptable type" );

// ================================================

TEST( TestConstrainedAny, CanDefaultConstruct )
{
	// Arrange

	// Act
	yan::copyable_any sut;

	// Assert
	EXPECT_FALSE( sut.has_value() );
}

TEST( TestConstrainedAny, NoValue_CanCopyConstruct )
{
	// Arrange
	yan::copyable_any src;

	// Act
	yan::copyable_any sut( src );

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( src.type(), sut.type() );
	EXPECT_FALSE( src.has_value() );
	EXPECT_FALSE( sut.has_value() );
}

TEST( TestConstrainedAny, NoValue_CanMoveConstruct )
{
	// Arrange
	yan::copyable_any src;

	// Act
	yan::copyable_any sut( std::move( src ) );

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( src.type(), sut.type() );
	EXPECT_FALSE( src.has_value() );
	EXPECT_FALSE( sut.has_value() );
}

TEST( TestConstrainedAny, CanConstructWithLvalue )
{
	// Arrange
	int value = 42;

	// Act
	yan::copyable_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
}

TEST( TestConstrainedAny, CanConstructWithRvalue )
{
	// Arrange
	int value = 42;

	// Act
	yan::copyable_any sut( std::move( value ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
}

TEST( TestConstrainedAny, NoValue_CanCopyAssign )
{
	// Arrange
	yan::copyable_any src;
	yan::copyable_any sut;

	// Act
	sut = src;

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( src.type(), sut.type() );
	EXPECT_FALSE( src.has_value() );
	EXPECT_FALSE( sut.has_value() );
}

TEST( TestConstrainedAny, NoValue_CanMoveAssign )
{
	// Arrange
	yan::copyable_any src;
	yan::copyable_any sut;

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( src.type(), sut.type() );
	EXPECT_FALSE( src.has_value() );
	EXPECT_FALSE( sut.has_value() );
}

TEST( TestConstrainedAny, HasValue_CanCopyConstruct )
{
	// Arrange
	int               value = 42;
	yan::copyable_any src( value );

	// Act
	yan::copyable_any sut( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
	EXPECT_EQ( yan::constrained_any_cast<int>( src ), value );
}

TEST( TestConstrainedAny, HasValue_CanMoveConstruct )
{
	// Arrange
	int               value = 42;
	yan::copyable_any src( value );

	// Act
	yan::copyable_any sut( std::move( src ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
	EXPECT_TRUE( src.has_value() );
}

TEST( TestConstrainedAny, HasValue_CanCopyAssign )
{
	// Arrange
	int               value = 42;
	yan::copyable_any src( value );
	yan::copyable_any sut;

	// Act
	sut = src;

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
	EXPECT_EQ( yan::constrained_any_cast<int>( src ), value );
}

TEST( TestConstrainedAny, HasValue_CanCopyAssignBySameType )
{
	// Arrange
	TestCopyOnlyType  src_v( 42 );
	TestCopyOnlyType  sut_v( 41 );
	yan::copyable_any src( src_v );
	yan::copyable_any sut( sut_v );

	// Act
	sut = src;

	// Assert
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( src ).v_, 42 );
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
}

TEST( TestConstrainedAny, HasValue_CanMoveAssignBySameType )
{
	TestCopyOnlyType  src_v( 42 );
	TestCopyOnlyType  sut_v( 41 );
	yan::copyable_any src( src_v );
	yan::copyable_any sut( sut_v );

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( src ).v_, 42 );
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
}

TEST( TestConstrainedAny, CanTranslationConstructorFromLValue )
{
	// Arrange
	int value = 42;

	// Act
	yan::copyable_any sut = value;

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
}

TEST( TestConstrainedAny, CanTranslationConstructorFromRValue )
{
	// Arrange

	// Act
	yan::copyable_any sut = static_cast<int>( 42 );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), 42 );
}

TEST( TestConstrainedAny, CanSwap )
{
	// Arrange
	yan::copyable_any sut = static_cast<int>( 42 );
	yan::copyable_any src = static_cast<double>( 3.0 );

	// Act
	sut.swap( src );

	// Assert
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( src ), 42 );
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( double ) );
	EXPECT_EQ( yan::constrained_any_cast<double>( sut ), 3.0 );
}

TEST( TestConstrainedAny, StoreCopyConstructOnly_CanCopyAssignToDifferentTypeStoredAny )
{
	// Arrange
	TestCopyConstructOnly tv( 1 );
	yan::copyable_any     sut( static_cast<int>( 42 ) );
	yan::copyable_any     src( tv );

	// Act
	sut = src;

	// Assert
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyConstructOnly ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyConstructOnly&>( src ).v_, 1 );
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyConstructOnly ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyConstructOnly&>( sut ).v_, 1 );
}

TEST( TestConstrainedAny, StoreCopyConstructOnly_CanCopyAssignToSameTypeStoredAny )
{
	// Arrange
	TestCopyConstructOnly sut_v( 2 );
	TestCopyConstructOnly src_v( 1 );
	yan::copyable_any     sut( sut_v );
	yan::copyable_any     src( src_v );

	// Act
	sut = src;

	// Assert
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyConstructOnly ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyConstructOnly&>( src ).v_, 1 );
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyConstructOnly ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyConstructOnly&>( sut ).v_, 1 );
}

TEST( TestConstrainedAny, StoreCopyConstructOnly_CanMoveAssignToDifferentTypeStoredAny )
{
	// Arrange
	TestCopyConstructOnly tv( 1 );
	yan::copyable_any     sut( static_cast<int>( 42 ) );
	yan::copyable_any     src( tv );

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyConstructOnly ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyConstructOnly&>( sut ).v_, 1 );
	// source status is not specified by C++ standard spec of std::any
	// EXPECT_TRUE( src.has_value() );
	// EXPECT_EQ( src.type(), typeid( TestCopyConstructOnly ) );
	// EXPECT_EQ( yan::constrained_any_cast<TestCopyConstructOnly&>( src ).v_, 1 );
}

TEST( TestConstrainedAny, StoreCopyConstructOnly_CanMoveAssignToSameTypeStoredAny )
{
	// Arrange
	TestCopyConstructOnly sut_v( 2 );
	TestCopyConstructOnly src_v( 1 );
	yan::copyable_any     sut( sut_v );
	yan::copyable_any     src( src_v );

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyConstructOnly ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyConstructOnly&>( sut ).v_, 1 );
	// source status is not specified by C++ standard spec of std::any
	// EXPECT_TRUE( src.has_value() );
	// EXPECT_EQ( src.type(), typeid( TestCopyConstructOnly ) );
	// EXPECT_EQ( yan::constrained_any_cast<TestCopyConstructOnly&>( src ).v_, 1 );
}

TEST( TestConstrainedAny, StoreMoveConstructOnly_CanMoveAssignToDifferentTypeStoredAny )
{
	// Arrange
	TestMoveConstructOnly tv( 1 );
	yan::movable_any      sut( static_cast<int>( 42 ) );
	yan::movable_any      src( std::move( tv ) );

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestMoveConstructOnly ) );
	EXPECT_EQ( yan::constrained_any_cast<TestMoveConstructOnly&>( sut ).v_, 1 );
	// source status is not specified by C++ standard spec of std::any
	// EXPECT_TRUE( src.has_value() );
	// EXPECT_EQ( src.type(), typeid( TestMoveConstructOnly ) );
	// EXPECT_EQ( yan::constrained_any_cast<TestMoveConstructOnly&>( src ).v_, 1 );
}

TEST( TestConstrainedAny, StoreMoveConstructOnly_CanMoveAssignToSameTypeStoredAny )
{
	// Arrange
	TestMoveConstructOnly sut_v( 2 );
	TestMoveConstructOnly src_v( 1 );
	yan::movable_any      sut( std::move( sut_v ) );
	yan::movable_any      src( std::move( src_v ) );

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestMoveConstructOnly ) );
	EXPECT_EQ( yan::constrained_any_cast<TestMoveConstructOnly&>( sut ).v_, 1 );
	// source status is not specified by C++ standard spec of std::any
	// EXPECT_TRUE( src.has_value() );
	// EXPECT_EQ( src.type(), typeid( TestMoveConstructOnly ) );
	// EXPECT_EQ( yan::constrained_any_cast<TestMoveConstructOnly&>( src ).v_, 1 );
}

// ================================================================

TEST( TestConstrainedAnyCast, ConstAny_CanMatchType )
{
	// Arrange
	const yan::copyable_any sut( static_cast<int>( 42 ) );
	int                     ret = 0;

	// Act
	EXPECT_NO_THROW( ret = yan::constrained_any_cast<int>( sut ) );

	// Assert
	EXPECT_EQ( ret, 42 );
}

TEST( TestConstrainedAnyCast, ConstAny_CanUnMatchType )
{
	// Arrange
	const yan::copyable_any sut( static_cast<int>( 42 ) );
	double                  ret = 0.0;

	// Act
	EXPECT_THROW( ret = yan::constrained_any_cast<double>( sut ), std::bad_any_cast );

	// Assert
	EXPECT_EQ( ret, 0.0 );
}

TEST( TestConstrainedAnyCast, NonConstAny_CanMatchType )
{
	// Arrange
	yan::copyable_any sut( static_cast<int>( 42 ) );
	int               ret = 0;

	// Act
	EXPECT_NO_THROW( ret = yan::constrained_any_cast<int>( sut ) );

	// Assert
	EXPECT_EQ( ret, 42 );
}

TEST( TestConstrainedAnyCast, NonConstAny_CanUnMatchType )
{
	// Arrange
	yan::copyable_any sut( static_cast<int>( 42 ) );
	double            ret = 0.0;

	// Act
	EXPECT_THROW( ret = yan::constrained_any_cast<double>( sut ), std::bad_any_cast );

	// Assert
	EXPECT_EQ( ret, 0.0 );
}

TEST( TestConstrainedAnyCast, RvalueNonConstAny_CanMatchType )
{
	// Arrange
	yan::copyable_any sut( static_cast<int>( 42 ) );
	int               ret = 0;

	// Act
	EXPECT_NO_THROW( ret = yan::constrained_any_cast<int>( std::move( sut ) ) );

	// Assert
	EXPECT_EQ( ret, 42 );
}

TEST( TestConstrainedAnyCast, RvalueNonConstAny_CanUnMatchType )
{
	// Arrange
	yan::copyable_any sut( static_cast<int>( 42 ) );
	double            ret = 0.0;

	// Act
	EXPECT_THROW( ret = yan::constrained_any_cast<double>( std::move( sut ) ), std::bad_any_cast );

	// Assert
	EXPECT_EQ( ret, 0.0 );
}

// ================================================================

TEST( TestMovableAny, CanConstruct )
{
	// Arrange

	// Act
	yan::movable_any sut;

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( void ) );
}

TEST( TestMovableAny, Empty_CanMoveConstruct_ThenEmpty )
{
	// Arrange
	yan::movable_any src;

	// Act
	yan::movable_any sut( std::move( src ) );

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( void ) );
}

TEST( TestMovableAny, Empty_CanMoveAssignToEmpty_ThenEmpty )
{
	// Arrange
	yan::movable_any src;
	yan::movable_any sut;

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( void ) );
}

TEST( TestMovableAny, Empty_CanMoveAssignToValid_ThenEmpty )
{
	// Arrange
	yan::movable_any src;
	yan::movable_any sut( 1 );

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( void ) );
}

TEST( TestMovableAny, Valid_CanMoveConstruct_ThenValid )
{
	// Arrange
	yan::movable_any src( TestMoveOnlyType( 1 ) );

	// Act
	yan::movable_any sut( std::move( src ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestMoveOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestMoveOnlyType&>( sut ).v_, 1 );
}

TEST( TestMovableAny, Valid_CanMoveAssignBySameType_ThenValid )
{
	// Arrange
	yan::movable_any src( TestMoveOnlyType( 1 ) );
	yan::movable_any sut( TestMoveOnlyType( 2 ) );

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestMoveOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestMoveOnlyType&>( sut ).v_, 1 );
}

// ================================================================

struct is_callable_print_impl {
	template <typename T>
	static auto check( T* ) -> decltype( std::declval<T>().print(), std::true_type {} );
	template <typename T>
	static auto check( ... ) -> std::false_type;

	template <typename T>
	using type = decltype( check<T>( nullptr ) );
};

template <typename T>
struct is_callable_print : public is_callable_print_impl::type<T> {};

struct special_operation_adapter_call_print_if {
	virtual ~special_operation_adapter_call_print_if() = default;

	virtual bool specialized_operation_print_proxy( void ) const = 0;
};

template <typename Carrier>
class special_operation_adapter_call_print : public special_operation_adapter_call_print_if {
public:
	static constexpr bool require_copy_constructible = true;
	static constexpr bool constraint_check_result    = !yan::is_related_type_of_constrained_any<Carrier>::value &&
	                                                is_callable_print<Carrier>::value;

	template <typename U = Carrier, typename std::enable_if<yan::is_specialized_of_constrained_any<U>::value>::type* = nullptr>
	bool call_print( void ) const
	{
		const Carrier* p = static_cast<const Carrier*>( this );

		const special_operation_adapter_call_print_if* p_soi = p->template get_special_operation_if<special_operation_adapter_call_print_if>();
		if ( p_soi == nullptr ) {
			std::cout << "no_constrained_any has no value" << std::endl;
			return false;
		}

		return p_soi->specialized_operation_print_proxy();
	}

private:
	bool specialized_operation_print_proxy( void ) const override
	{
		return call_print();
	}

	template <typename U = Carrier, typename std::enable_if<yan::is_value_carrier_of_constrained_any<U>::value>::type* = nullptr>
	bool call_print( void ) const
	{
		const Carrier* p = static_cast<const Carrier*>( this );

		p->ref().print();
		return true;
	}
};

class Foo_has_print {
public:
	int value_;

	Foo_has_print()
	  : value_( 0 ) {}
	Foo_has_print( int value )
	  : value_( value ) {}
	Foo_has_print( const Foo_has_print& )            = default;
	Foo_has_print( Foo_has_print&& )                 = default;
	Foo_has_print& operator=( const Foo_has_print& ) = default;
	Foo_has_print& operator=( Foo_has_print&& )      = default;

	// This function is used to check if the class has a print
	void print() const
	{
		std::cout << "Success to call print() of Foo_has_print: " << value_ << std::endl;
	}
};

TEST( TestConstrainedAny, WithConstraint_CanCopyConstructFromLvalue )
{
	// Arrange
	Foo_has_print value( 42 );

	// Act
	yan::constrained_any<special_operation_adapter_call_print> sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( Foo_has_print ) );
	EXPECT_EQ( yan::constrained_any_cast<Foo_has_print&>( sut ).value_, 42 );
}

TEST( TestConstrainedAny, WithConstraint_CanCopyConstructFromLvalueInt )
{
	// Arrange
	Foo_has_print value( 42 );

	// Act
	yan::constrained_any<special_operation_adapter_call_print> sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( Foo_has_print ) );
	EXPECT_EQ( yan::constrained_any_cast<Foo_has_print&>( sut ).value_, 42 );
}

TEST( TestConstrainedAny, WithConstraintAndSpecialOperator_CanConstruct )
{
	// Arrange
	Foo_has_print value( 42 );

	// Act
	yan::constrained_any<special_operation_adapter_call_print> sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( Foo_has_print ) );
	EXPECT_EQ( yan::constrained_any_cast<Foo_has_print&>( sut ).value_, 42 );
}

TEST( TestConstrainedAny, WithConstraintAndSpecialOperator_CanCallPrint )
{
	// Arrange
	Foo_has_print                                              value( 42 );
	yan::constrained_any<special_operation_adapter_call_print> sut( value );

	// Act
	bool result = sut.call_print();

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( Foo_has_print ) );
	EXPECT_EQ( yan::constrained_any_cast<Foo_has_print&>( sut ).value_, 42 );
	EXPECT_TRUE( result );
}

// ================================================================

TEST( TestConstrainedAny, CanConstructWithCopyOnlyRvalue )
{
	// Arrange
	TestCopyOnlyType value( 42 );

	// Act
	yan::copyable_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
}

TEST( TestConstrainedAny, OnlyCopyType_CanCopyConstruct )
{
	// Arrange
	TestCopyOnlyType  value( 42 );
	yan::copyable_any src( value );

	// Act
	yan::copyable_any sut( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
}

TEST( TestConstrainedAny, OnlyCopyType_CanMoveConstruct )
{
	// Arrange
	TestCopyOnlyType  value( 42 );
	yan::copyable_any src( value );

	// Act
	yan::copyable_any sut( std::move( src ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
}

TEST( TestConstrainedAny, OnlyCopyType_CanCopyAssign )
{
	// Arrange
	TestCopyOnlyType  value( 42 );
	yan::copyable_any src( value );
	yan::copyable_any sut;

	// Act
	sut = src;

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( src ).v_, 42 );
}

TEST( TestConstrainedAny, OnlyCopyType_CanMoveAssign )
{
	// Arrange
	TestCopyOnlyType  value( 42 );
	yan::copyable_any src( value );
	yan::copyable_any sut;

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( src ).v_, 42 );
}

// ================================================================

TEST( TestConstrainedAny, CopyAndMoveType_CanCopyConstruct )
{
	// Arrange
	TestCopyAndMoveType value( 42 );
	yan::copyable_any   src( value );

	// Act
	yan::copyable_any sut( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyAndMoveType&>( sut ).v_, 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyAndMoveType&>( src ).v_, 42 );
}

TEST( TestConstrainedAny, CopyAndMoveType_CanMoveConstruct )
{
	// Arrange
	TestCopyAndMoveType value( 42 );
	yan::copyable_any   src( value );

	// Act
	yan::copyable_any sut( std::move( src ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyAndMoveType&>( sut ).v_, 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyAndMoveType&>( src ).v_, 0 );
}

TEST( TestConstrainedAny, CopyAndMoveType_CanCopyAssign )
{
	// Arrange
	TestCopyAndMoveType value( 42 );
	yan::copyable_any   src( value );
	yan::copyable_any   sut;

	// Act
	sut = src;

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyAndMoveType&>( sut ).v_, 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyAndMoveType&>( src ).v_, 42 );
}

TEST( TestConstrainedAny, CopyAndMoveType_CanMoveAssign )
{
	// Arrange
	TestCopyAndMoveType value( 42 );
	yan::copyable_any   src( value );
	yan::copyable_any   sut;

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyAndMoveType&>( sut ).v_, 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyAndMoveType&>( src ).v_, 0 );
}

// ================================================================

TEST( TestConstrainedAny_NowAllowCopy, CanConstruct )
{
	// Arrange
	std::unique_ptr<int> value = std::make_unique<int>( 42 );

	// Act
	yan::constrained_any<yan::impl::special_operation_movable> sut( std::move( value ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( *( yan::constrained_any_cast<std::unique_ptr<int>&>( sut ) ), 42 );
}

TEST( TestConstrainedAny_NowAllowCopy, CanMoveConstruct )
{
	// Arrange
	std::unique_ptr<int>                                       value = std::make_unique<int>( 42 );
	yan::constrained_any<yan::impl::special_operation_movable> src( std::move( value ) );

	// Act
	yan::constrained_any<yan::impl::special_operation_movable> sut( std::move( src ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( *( yan::constrained_any_cast<std::unique_ptr<int>&>( sut ) ), 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( yan::constrained_any_cast<std::unique_ptr<int>&>( src ), nullptr );
}

TEST( TestConstrainedAny_NowAllowCopy, CanMoveAssign )
{
	// Arrange
	std::unique_ptr<int>                                       value = std::make_unique<int>( 42 );
	yan::constrained_any<yan::impl::special_operation_movable> src( std::move( value ) );
	yan::constrained_any<yan::impl::special_operation_movable> sut;

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( *( yan::constrained_any_cast<std::unique_ptr<int>&>( sut ) ), 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( yan::constrained_any_cast<std::unique_ptr<int>&>( src ), nullptr );
}

TEST( TestConstrainedAny_NowAllowCopy, CanGetValueByMoveCast )
{
	// Arrange
	std::unique_ptr<int>                                       value = std::make_unique<int>( 42 );
	yan::constrained_any<yan::impl::special_operation_movable> sut( std::move( value ) );

	// Act
	auto up_ret = yan::constrained_any_cast<std::unique_ptr<int>&&>( std::move( sut ) );

	// Assert
	ASSERT_NE( up_ret, nullptr );
	EXPECT_EQ( *up_ret, 42 );
	EXPECT_EQ( sut.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( yan::constrained_any_cast<std::unique_ptr<int>&>( sut ), nullptr );
}

TEST( TestConstrainedAny_NonMemberFunction, CanMakeConstrainedAny )
{
	// Arrange
	int value = 42;

	// Act
	auto sut = yan::make_constrained_any<int, yan::impl::special_operation_copyable>( 42 );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
}

TEST( TestConstrainedAny_NonMemberFunction, CanMakeConstrainedAnyWithKeyableAny )
{
	// Arrange
	int value = 42;

	// Act
	auto sut = yan::make_constrained_any<int, yan::keyable_any>( 42 );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
}

#if 0
TEST( TestConstrainedAny_NonMemberFunction, CanNotMakeConstrainedAnyWithKeyableAny )
{
	// Arrange
	Foo_has_print value = 42;

	// Act
	auto sut = yan::make_constrained_any<int, yan::keyable_any>( value );	// compile error is expected

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
}
#endif

// ================================================

TEST( TestWeakOrderingAny, CanConstruct )
{
	// Arrange

	// Act
	yan::weak_ordering_any sut;

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( void ) );
}

TEST( TestWeakOrderingAny, CanConstructWithInt )
{
	// Arrange
	int value = 42;

	// Act
	yan::weak_ordering_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
}

TEST( TestWeakOrderingAny, CanConstructWithString )
{
	// Arrange
	std::string value = "Hello";

	// Act
	yan::weak_ordering_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( std::string ) );
	EXPECT_EQ( yan::constrained_any_cast<std::string>( sut ), value );
}
TEST( TestWeakOrderingAny, CanConstructWithDouble )
{
	// Arrange
	double value = 3.14;

	// Act
	yan::weak_ordering_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( double ) );
	EXPECT_EQ( yan::constrained_any_cast<double>( sut ), value );
}

TEST( TestWeakOrderingAny, CanLessWithSameType )
{
	// Arrange
	yan::weak_ordering_any a( 42 );
	yan::weak_ordering_any b( 43 );

	// Act
	bool result = ( a < b );

	// Assert
	EXPECT_TRUE( result );
}

TEST( TestWeakOrderingAny, CanLessWithDifferentType )
{
	// Arrange
	yan::weak_ordering_any a( 42 );
	yan::weak_ordering_any b( std::string( "Hello" ) );

	// Act
	ASSERT_NO_THROW( a < b );

	// Assert
}

TEST( TestWeakOrderingAny, CanUseMapWithWeakOrderingAny )
{
	// Arrange
	std::map<yan::weak_ordering_any, int> map;
	yan::weak_ordering_any                key1( 42 );
	yan::weak_ordering_any                key2( 43 );

	// Act
	map[key1] = 1;
	map[key2] = 2;

	// Assert
	EXPECT_EQ( map.size(), 2 );
	EXPECT_EQ( map[key1], 1 );
	EXPECT_EQ( map[key2], 2 );
}

// ================================================

TEST( TestUnorderedKeyAny, CanConstruct )
{
	// Arrange

	// Act
	yan::unordered_key_any sut;

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( void ) );
}

TEST( TestUnorderedKeyAny, CanConstructWithInt )
{
	// Arrange
	int value = 42;

	// Act
	yan::unordered_key_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
}

TEST( TestUnorderedKeyAny, CanConstructWithString )
{
	// Arrange
	std::string value = "Hello";

	// Act
	yan::unordered_key_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( std::string ) );
	EXPECT_EQ( yan::constrained_any_cast<std::string>( sut ), value );
}
TEST( TestUnorderedKeyAny, CanConstructWithDouble )
{
	// Arrange
	double value = 3.14;

	// Act
	yan::unordered_key_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( double ) );
	EXPECT_EQ( yan::constrained_any_cast<double>( sut ), value );
}

TEST( TestUnorderedKeyAny, CanEqualToWithSameTypeSameValue )
{
	// Arrange
	yan::unordered_key_any a( 42 );
	yan::unordered_key_any b( 42 );

	// Act
	bool result = ( a == b );

	// Assert
	EXPECT_TRUE( result );
}

TEST( TestUnorderedKeyAny, CanEqualToWithSameTypeDifferentValue )
{
	// Arrange
	yan::unordered_key_any a( 42 );
	yan::unordered_key_any b( 43 );

	// Act
	bool result = ( a == b );

	// Assert
	EXPECT_FALSE( result );
}

TEST( TestUnorderedKeyAny, CanEqualToWithDifferentType )
{
	// Arrange
	yan::unordered_key_any a( 42 );
	yan::unordered_key_any b( std::string( "Hello" ) );
	bool                   result = true;

	// Act
	ASSERT_NO_THROW( result = ( a == b ) );

	// Assert
	EXPECT_FALSE( result );
}

TEST( TestUnorderedKeyAny, CanUseUnorderedMapWithUnorderedKeyAny )
{
	// Arrange
	std::unordered_map<yan::unordered_key_any, int> map;
	yan::unordered_key_any                          key1( 42 );
	yan::unordered_key_any                          key2( 43 );
	yan::unordered_key_any                          key3( std::string( "Hello" ) );

	// Act
	map[key1] = 1;
	map[key2] = 2;
	map[key3] = 3;

	// Assert
	EXPECT_EQ( map.size(), 3 );
	EXPECT_EQ( map[key1], 1 );
	EXPECT_EQ( map[key2], 2 );
	EXPECT_EQ( map[key3], 3 );
}

// ================================================

TEST( TestKeyableAny, CanConstruct )
{
	// Arrange

	// Act
	yan::keyable_any sut;

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( void ) );
}

TEST( TestKeyableAny, CanConstructWithInt )
{
	// Arrange
	int value = 42;

	// Act
	yan::keyable_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
}

TEST( TestKeyableAny, CanConstructWithString )
{
	// Arrange
	std::string value = "Hello";

	// Act
	yan::keyable_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( std::string ) );
	EXPECT_EQ( yan::constrained_any_cast<std::string>( sut ), value );
}

TEST( TestKeyableAny, CanConstructWithDouble )
{
	// Arrange
	double value = 3.14;

	// Act
	yan::keyable_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( double ) );
	EXPECT_EQ( yan::constrained_any_cast<double>( sut ), value );
}

TEST( TestKeyableAny, CanEqualToWithSameTypeSameValue )
{
	// Arrange
	yan::keyable_any a( 42 );
	yan::keyable_any b( 42 );

	// Act
	bool result = ( a == b );

	// Assert
	EXPECT_TRUE( result );
}

TEST( TestKeyableAny, CanEqualToWithSameTypeDifferentValue )
{
	// Arrange
	yan::keyable_any a( 42 );
	yan::keyable_any b( 43 );

	// Act
	bool result = ( a == b );

	// Assert
	EXPECT_FALSE( result );
}

TEST( TestKeyableAny, CanEqualToWithDifferentType )
{
	// Arrange
	yan::keyable_any a( 42 );
	yan::keyable_any b( std::string( "Hello" ) );
	bool             result = true;

	// Act
	ASSERT_NO_THROW( result = ( a == b ) );

	// Assert
	EXPECT_FALSE( result );
}

TEST( TestKeyableAny, CanLessWithSameType )
{
	// Arrange
	yan::keyable_any a( 42 );
	yan::keyable_any b( 43 );

	// Act
	bool result = ( a < b );

	// Assert
	EXPECT_TRUE( result );
}

TEST( TestKeyableAny, CanLessWithDifferentType )
{
	// Arrange
	yan::keyable_any a( 42 );
	yan::keyable_any b( std::string( "Hello" ) );
	bool             result = true;

	// Act
	ASSERT_NO_THROW( result = ( a < b ) );

	// Assert
	EXPECT_FALSE( result );
}

TEST( TestKeyableAny, CanHash )
{
	// Arrange
	yan::keyable_any a( 42 );
	yan::keyable_any b( 42 );

	// Act
	size_t hash_a = std::hash<yan::keyable_any>()( a );
	size_t hash_b = std::hash<yan::keyable_any>()( b );

	// Assert
	EXPECT_EQ( hash_a, hash_b );
}

TEST( TestKeyableAny, CanUseUnorderedMapWithKeyableAny )
{
	// Arrange
	std::unordered_map<yan::keyable_any, int> map;
	yan::keyable_any                          key1( 42 );
	yan::keyable_any                          key2( 43 );
	yan::keyable_any                          key3( std::string( "Hello" ) );

	// Act
	map[key1] = 1;
	map[key2] = 2;
	map[key3] = 3;

	// Assert
	EXPECT_EQ( map.size(), 3 );
	EXPECT_EQ( map[key1], 1 );
	EXPECT_EQ( map[key2], 2 );
	EXPECT_EQ( map[key3], 3 );
}

TEST( TestKeyableAny, CanUseMapWithKeyableAny )
{
	// Arrange
	std::map<yan::keyable_any, int> map;
	yan::keyable_any                key1( 42 );
	yan::keyable_any                key2( 43 );
	yan::keyable_any                key3( std::string( "Hello" ) );

	// Act
	map[key1] = 1;
	map[key2] = 2;
	map[key3] = 3;

	// Assert
	EXPECT_EQ( map.size(), 3 );
	EXPECT_EQ( map[key1], 1 );
	EXPECT_EQ( map[key2], 2 );
	EXPECT_EQ( map[key3], 3 );
}
