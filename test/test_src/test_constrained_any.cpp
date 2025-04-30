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
	yth::constrained_any<true, is_callable_print> sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( Foo_has_print ) );
	EXPECT_EQ( yth::constrained_any_cast<Foo_has_print&>( sut ).value_, 42 );
}

TEST( TestConstrainedAny, WithConstraint_CanCopyConstructFromLvalueInt )
{
	// Arrange
	Foo_has_print value( 42 );

	// Act
	yth::constrained_any<true, is_callable_print> sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( Foo_has_print ) );
	EXPECT_EQ( yth::constrained_any_cast<Foo_has_print&>( sut ).value_, 42 );
}

template <typename Carrier>
class special_operation_adapter_call_print : public yth::special_operation_if {
public:
	void specialized_operation_callback( void* p_com_data ) override
	{
		*reinterpret_cast<bool*>( p_com_data ) = call_print();
	}
	void specialized_operation_callback( void* p_com_data ) const override
	{
		*reinterpret_cast<bool*>( p_com_data ) = call_print();
	}

	template <typename U = Carrier, typename std::enable_if<yth::is_callable_ref<U>::value>::type* = nullptr>
	bool call_print( void ) const
	{
		const Carrier* p = static_cast<const Carrier*>( this );

		p->ref().print();
		return true;
	}

	template <typename U = Carrier, typename std::enable_if<!yth::is_callable_ref<U>::value>::type* = nullptr>
	bool call_print( void ) const
	{
		const Carrier* p = static_cast<const Carrier*>( this );

		const yth::special_operation_if* p_soi = p->get_special_operation_if();
		if ( p_soi == nullptr ) {
			std::cout << "constrained_any has no value" << std::endl;
			return false;
		}

		bool result = false;
		p_soi->specialized_operation_callback( &result );
		return result;
	}
};

TEST( TestConstrainedAny, WithConstraintAndSpecialOperator_CanConstruct )
{
	// Arrange
	Foo_has_print value( 42 );

	// Act
	yth::constrained_any<true, is_callable_print, special_operation_adapter_call_print> sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( Foo_has_print ) );
	EXPECT_EQ( yth::constrained_any_cast<Foo_has_print&>( sut ).value_, 42 );
}

TEST( TestConstrainedAny, WithConstraintAndSpecialOperator_CanCallPrint )
{
	// Arrange
	Foo_has_print                                                                       value( 42 );
	yth::constrained_any<true, is_callable_print, special_operation_adapter_call_print> sut( value );

	// Act
	bool result = sut.call_print();

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( Foo_has_print ) );
	EXPECT_EQ( yth::constrained_any_cast<Foo_has_print&>( sut ).value_, 42 );
	EXPECT_TRUE( result );
}

// ================================================================

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

TEST( TestConstrainedAny, CanConstructWithCopyOnlyRvalue )
{
	// Arrange
	TestCopyOnlyType value( 42 );

	// Act
	yth::constrained_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
}

TEST( TestConstrainedAny, OnlyCopyType_CanCopyConstruct )
{
	// Arrange
	TestCopyOnlyType     value( 42 );
	yth::constrained_any src( value );

	// Act
	yth::constrained_any sut( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
}

TEST( TestConstrainedAny, OnlyCopyType_CanMoveConstruct )
{
	// Arrange
	TestCopyOnlyType     value( 42 );
	yth::constrained_any src( value );

	// Act
	yth::constrained_any sut( std::move( src ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
}

TEST( TestConstrainedAny, OnlyCopyType_CanCopyAssign )
{
	// Arrange
	TestCopyOnlyType     value( 42 );
	yth::constrained_any src( value );
	yth::constrained_any sut;

	// Act
	sut = src;

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyOnlyType&>( src ).v_, 42 );
}

TEST( TestConstrainedAny, OnlyCopyType_CanMoveAssign )
{
	// Arrange
	TestCopyOnlyType     value( 42 );
	yth::constrained_any src( value );
	yth::constrained_any sut;

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyOnlyType&>( src ).v_, 42 );
}

// ================================================================

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

TEST( TestConstrainedAny, CopyAndMoveType_CanCopyConstruct )
{
	// Arrange
	TestCopyAndMoveType  value( 42 );
	yth::constrained_any src( value );

	// Act
	yth::constrained_any sut( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyAndMoveType&>( sut ).v_, 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyAndMoveType&>( src ).v_, 42 );
}

TEST( TestConstrainedAny, CopyAndMoveType_CanMoveConstruct )
{
	// Arrange
	TestCopyAndMoveType  value( 42 );
	yth::constrained_any src( value );

	// Act
	yth::constrained_any sut( std::move( src ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyAndMoveType&>( sut ).v_, 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyAndMoveType&>( src ).v_, 0 );
}

TEST( TestConstrainedAny, CopyAndMoveType_CanCopyAssign )
{
	// Arrange
	TestCopyAndMoveType  value( 42 );
	yth::constrained_any src( value );
	yth::constrained_any sut;

	// Act
	sut = src;

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyAndMoveType&>( sut ).v_, 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyAndMoveType&>( src ).v_, 42 );
}

TEST( TestConstrainedAny, CopyAndMoveType_CanMoveAssign )
{
	// Arrange
	TestCopyAndMoveType  value( 42 );
	yth::constrained_any src( value );
	yth::constrained_any sut;

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyAndMoveType&>( sut ).v_, 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( TestCopyAndMoveType ) );
	EXPECT_EQ( yth::constrained_any_cast<TestCopyAndMoveType&>( src ).v_, 0 );
}

// ================================================================

TEST( TestConstrainedAny_NowAllowCopy, CanConstruct )
{
	// Arrange
	std::unique_ptr<int> value = std::make_unique<int>( 42 );

	// Act
	yth::constrained_any<false> sut( std::move( value ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( *( yth::constrained_any_cast<std::unique_ptr<int>&>( sut ) ), 42 );
}

TEST( TestConstrainedAny_NowAllowCopy, CanMoveConstruct )
{
	// Arrange
	std::unique_ptr<int>        value = std::make_unique<int>( 42 );
	yth::constrained_any<false> src( std::move( value ) );

	// Act
	yth::constrained_any<false> sut( std::move( src ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( *( yth::constrained_any_cast<std::unique_ptr<int>&>( sut ) ), 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( yth::constrained_any_cast<std::unique_ptr<int>&>( src ), nullptr );
}

TEST( TestConstrainedAny_NowAllowCopy, CanMoveAssign )
{
	// Arrange
	std::unique_ptr<int>        value = std::make_unique<int>( 42 );
	yth::constrained_any<false> src( std::move( value ) );
	yth::constrained_any<false> sut;

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( *( yth::constrained_any_cast<std::unique_ptr<int>&>( sut ) ), 42 );
	EXPECT_TRUE( src.has_value() );
	EXPECT_EQ( src.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( yth::constrained_any_cast<std::unique_ptr<int>&>( src ), nullptr );
}

TEST( TestConstrainedAny_NowAllowCopy, CanGetValueByMoveCast )
{
	// Arrange
	std::unique_ptr<int>        value = std::make_unique<int>( 42 );
	yth::constrained_any<false> sut( std::move( value ) );

	// Act
	auto up_ret = yth::constrained_any_cast<std::unique_ptr<int>&&>( std::move( sut ) );

	// Assert
	ASSERT_NE( up_ret, nullptr );
	EXPECT_EQ( *up_ret, 42 );
	EXPECT_EQ( sut.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( yth::constrained_any_cast<std::unique_ptr<int>&>( sut ), nullptr );
}
