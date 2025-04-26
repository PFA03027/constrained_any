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

#include "constrained_any.hpp"

#include <gtest/gtest.h>

TEST( TestConstrainedAny, CanDefaultConstruct )
{
	// Arrange

	// Act
	yth::constrained_any sut;

	// Assert
	EXPECT_FALSE( sut.has_value() );
}

TEST( TestConstrainedAny, NoValue_CanCopyConstruct )
{
	// Arrange
	yth::constrained_any src;

	// Act
	yth::constrained_any sut( src );

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( src.type(), sut.type() );
	EXPECT_FALSE( src.has_value() );
	EXPECT_FALSE( sut.has_value() );
}

TEST( TestConstrainedAny, NoValue_CanMoveConstruct )
{
	// Arrange
	yth::constrained_any src;

	// Act
	yth::constrained_any sut( std::move( src ) );

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
	yth::constrained_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yth::constrained_any_cast<int>( sut ), value );
}

TEST( TestConstrainedAny, CanConstructWithRvalue )
{
	// Arrange
	int value = 42;

	// Act
	yth::constrained_any sut( std::move( value ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yth::constrained_any_cast<int>( sut ), value );
}

TEST( TestConstrainedAny, NoValue_CanCopyAssign )
{
	// Arrange
	yth::constrained_any src;
	yth::constrained_any sut;

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
	yth::constrained_any src;
	yth::constrained_any sut;

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
	int                  value = 42;
	yth::constrained_any src( value );

	// Act
	yth::constrained_any sut( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yth::constrained_any_cast<int>( sut ), value );
	EXPECT_EQ( yth::constrained_any_cast<int>( src ), value );
}

TEST( TestConstrainedAny, HasValue_CanMoveConstruct )
{
	// Arrange
	int                  value = 42;
	yth::constrained_any src( value );

	// Act
	yth::constrained_any sut( std::move( src ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yth::constrained_any_cast<int>( sut ), value );
	EXPECT_TRUE( src.has_value() );
}

TEST( TestConstrainedAny, HasValue_CanCopyAssign )
{
	// Arrange
	int                  value = 42;
	yth::constrained_any src( value );
	yth::constrained_any sut;

	// Act
	sut = src;

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yth::constrained_any_cast<int>( sut ), value );
	EXPECT_EQ( yth::constrained_any_cast<int>( src ), value );
}

TEST( TestConstrainedAny, HasValue_CanMoveAssign )
{
	// Arrange
	int                  value = 42;
	yth::constrained_any src( value );
	yth::constrained_any sut;

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yth::constrained_any_cast<int>( sut ), value );
	EXPECT_TRUE( src.has_value() );
}
