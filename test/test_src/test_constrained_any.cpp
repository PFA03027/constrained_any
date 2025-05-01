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

static_assert( yan::impl::is_specialized_of_constrained_any<int>::value == false, "int is not a constrained_any" );
static_assert( yan::impl::is_specialized_of_constrained_any<yan::constrained_any<true, yan::no_constrained, yan::no_specialoperation, yan::no_specialoperation>>::value, "constrained_any is specialized type of constrained_any" );

// ================================================

TEST( TestConstrainedAny, CanDefaultConstruct )
{
	// Arrange

	// Act
	yan::no_constrained_any sut;

	// Assert
	EXPECT_FALSE( sut.has_value() );
}

TEST( TestConstrainedAny, NoValue_CanCopyConstruct )
{
	// Arrange
	yan::no_constrained_any src;

	// Act
	yan::no_constrained_any sut( src );

	// Assert
	EXPECT_FALSE( sut.has_value() );
	EXPECT_EQ( src.type(), sut.type() );
	EXPECT_FALSE( src.has_value() );
	EXPECT_FALSE( sut.has_value() );
}

TEST( TestConstrainedAny, NoValue_CanMoveConstruct )
{
	// Arrange
	yan::no_constrained_any src;

	// Act
	yan::no_constrained_any sut( std::move( src ) );

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
	yan::no_constrained_any sut( value );

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
	yan::no_constrained_any sut( std::move( value ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
}

TEST( TestConstrainedAny, NoValue_CanCopyAssign )
{
	// Arrange
	yan::no_constrained_any src;
	yan::no_constrained_any sut;

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
	yan::no_constrained_any src;
	yan::no_constrained_any sut;

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
	int                     value = 42;
	yan::no_constrained_any src( value );

	// Act
	yan::no_constrained_any sut( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
	EXPECT_EQ( yan::constrained_any_cast<int>( src ), value );
}

TEST( TestConstrainedAny, HasValue_CanMoveConstruct )
{
	// Arrange
	int                     value = 42;
	yan::no_constrained_any src( value );

	// Act
	yan::no_constrained_any sut( std::move( src ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
	EXPECT_TRUE( src.has_value() );
}

TEST( TestConstrainedAny, HasValue_CanCopyAssign )
{
	// Arrange
	int                     value = 42;
	yan::no_constrained_any src( value );
	yan::no_constrained_any sut;

	// Act
	sut = src;

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
	EXPECT_EQ( yan::constrained_any_cast<int>( src ), value );
}

TEST( TestConstrainedAny, HasValue_CanMoveAssign )
{
	// Arrange
	int                     value = 42;
	yan::no_constrained_any src( value );
	yan::no_constrained_any sut;

	// Act
	sut = std::move( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( int ) );
	EXPECT_EQ( yan::constrained_any_cast<int>( sut ), value );
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
	yan::constrained_any<true, is_callable_print, yan::no_specialoperation> sut( value );

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
	yan::constrained_any<true, is_callable_print, yan::no_specialoperation> sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( Foo_has_print ) );
	EXPECT_EQ( yan::constrained_any_cast<Foo_has_print&>( sut ).value_, 42 );
}

#if 1
struct special_operation_adapter_call_print_if {
	virtual ~special_operation_adapter_call_print_if() = default;

	virtual bool specialized_operation_print_proxy( void ) const = 0;
};

template <typename Carrier>
class special_operation_adapter_call_print : public special_operation_adapter_call_print_if {
public:
	template <typename U = Carrier, typename std::enable_if<!yan::is_callable_ref<U>::value>::type* = nullptr>
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

	template <typename U = Carrier, typename std::enable_if<yan::is_callable_ref<U>::value>::type* = nullptr>
	bool call_print( void ) const
	{
		const Carrier* p = static_cast<const Carrier*>( this );

		p->ref().print();
		return true;
	}
};

TEST( TestConstrainedAny, WithConstraintAndSpecialOperator_CanConstruct )
{
	// Arrange
	Foo_has_print value( 42 );

	// Act
	yan::constrained_any<true, is_callable_print, special_operation_adapter_call_print> sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( Foo_has_print ) );
	EXPECT_EQ( yan::constrained_any_cast<Foo_has_print&>( sut ).value_, 42 );
}

TEST( TestConstrainedAny, WithConstraintAndSpecialOperator_CanCallPrint )
{
	// Arrange
	Foo_has_print                                                                       value( 42 );
	yan::constrained_any<true, is_callable_print, special_operation_adapter_call_print> sut( value );

	// Act
	bool result = sut.call_print();

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( Foo_has_print ) );
	EXPECT_EQ( yan::constrained_any_cast<Foo_has_print&>( sut ).value_, 42 );
	EXPECT_TRUE( result );
}
#endif

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
	yan::no_constrained_any sut( value );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
}

TEST( TestConstrainedAny, OnlyCopyType_CanCopyConstruct )
{
	// Arrange
	TestCopyOnlyType        value( 42 );
	yan::no_constrained_any src( value );

	// Act
	yan::no_constrained_any sut( src );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
}

TEST( TestConstrainedAny, OnlyCopyType_CanMoveConstruct )
{
	// Arrange
	TestCopyOnlyType        value( 42 );
	yan::no_constrained_any src( value );

	// Act
	yan::no_constrained_any sut( std::move( src ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( TestCopyOnlyType ) );
	EXPECT_EQ( yan::constrained_any_cast<TestCopyOnlyType&>( sut ).v_, 42 );
}

TEST( TestConstrainedAny, OnlyCopyType_CanCopyAssign )
{
	// Arrange
	TestCopyOnlyType        value( 42 );
	yan::no_constrained_any src( value );
	yan::no_constrained_any sut;

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
	TestCopyOnlyType        value( 42 );
	yan::no_constrained_any src( value );
	yan::no_constrained_any sut;

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
	TestCopyAndMoveType     value( 42 );
	yan::no_constrained_any src( value );

	// Act
	yan::no_constrained_any sut( src );

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
	TestCopyAndMoveType     value( 42 );
	yan::no_constrained_any src( value );

	// Act
	yan::no_constrained_any sut( std::move( src ) );

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
	TestCopyAndMoveType     value( 42 );
	yan::no_constrained_any src( value );
	yan::no_constrained_any sut;

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
	TestCopyAndMoveType     value( 42 );
	yan::no_constrained_any src( value );
	yan::no_constrained_any sut;

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
	yan::constrained_any<false, yan::no_constrained, yan::no_specialoperation> sut( std::move( value ) );

	// Assert
	EXPECT_TRUE( sut.has_value() );
	EXPECT_EQ( sut.type(), typeid( std::unique_ptr<int> ) );
	EXPECT_EQ( *( yan::constrained_any_cast<std::unique_ptr<int>&>( sut ) ), 42 );
}

TEST( TestConstrainedAny_NowAllowCopy, CanMoveConstruct )
{
	// Arrange
	std::unique_ptr<int>                                                       value = std::make_unique<int>( 42 );
	yan::constrained_any<false, yan::no_constrained, yan::no_specialoperation> src( std::move( value ) );

	// Act
	yan::constrained_any<false, yan::no_constrained, yan::no_specialoperation> sut( std::move( src ) );

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
	std::unique_ptr<int>                                                       value = std::make_unique<int>( 42 );
	yan::constrained_any<false, yan::no_constrained, yan::no_specialoperation> src( std::move( value ) );
	yan::constrained_any<false, yan::no_constrained, yan::no_specialoperation> sut;

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
	std::unique_ptr<int>                                                       value = std::make_unique<int>( 42 );
	yan::constrained_any<false, yan::no_constrained, yan::no_specialoperation> sut( std::move( value ) );

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
	auto sut = yan::make_constrained_any<int, true, yan::no_constrained, yan::no_specialoperation>( 42 );

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
