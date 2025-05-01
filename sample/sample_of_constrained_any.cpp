/**
 * @file sample_of_constrained_any.cpp
 * @author Teruaki Ata (PFA03027@nifty.com)
 * @brief Sample code for constrained_any
 * @version 0.1
 * @date 2025-04-30
 *
 * @copyright Copyright (c) 2025, Teruaki Ata (PFA03027@nifty.com)
 *
 * @details
 * This is a sample code for constrained_any to prepare specialized constrained_any that has your own constraints.
 */

#include <iostream>
#include <string>
#include <vector>

#include "constrained_any.hpp"

// ---------------------------------------------------
// Step 1: Define your own constraints

/**
 * @brief Constraint meta class to check if the type is convertible to std::string
 *
 * @tparam T Type to check
 *
 * @details
 * This constraint is true if one of below conditions is satisfied:
 * @li T is std::string
 * @li T is std::string_view
 * @li T is char*
 * @li T is convertible to std::string implicitly
 * @li std::to_string(T) is valid
 */
template <typename T>
struct is_convertible_to_string {
	template <typename U>
	static auto check_if_callable_to_string( U* ) -> decltype( std::to_string( std::declval<U>() ), std::string() );
	template <typename U>
	static auto check_if_callable_to_string( ... ) -> std::false_type;

	static constexpr bool value = std::is_same_v<T, std::string> ||
	                              std::is_same_v<T, std::string_view> ||
	                              std::is_same_v<T, char*> ||
	                              std::is_convertible_v<T, std::string> ||
	                              std::is_same<std::string, decltype( check_if_callable_to_string<T>( nullptr ) )>::value;
};

// ---------------------------------------------------
// Step 2: Define your own specialized operation

/**
 * @brief Specialized operation interface for converting to std::string
 */
class special_operation_convert_to_string_if {
public:
	virtual ~special_operation_convert_to_string_if() = default;

	/**
	 * @brief Specialized operation callback function
	 *
	 * @note
	 * This function is called from constrained_any class.
	 */
	virtual std::string specialized_operation_convert_to_string_proxy( void ) const = 0;
};

/**
 * @brief Specialized operation to_string member function that converts the value to std::string
 *
 * @tparam Carrier Type of the carrier
 */
template <typename Carrier>
class special_operation_convert_to_string : public special_operation_convert_to_string_if {
public:
	/**
	 * @brief Constraint check result
	 *
	 * @details
	 * In case of this class of this sample code, input value type should support convertibility to std::string as the constraint.
	 * So, this boolean value indicates the constraint check result.
	 * @li true, if the Carrier type is convertible to std::string.
	 * @li otherwise false.
	 */
	static constexpr bool constraint_check_result = is_convertible_to_string<Carrier>::value;

	/**
	 * @brief convert the value to std::string
	 *
	 * @tparam U Concrete constrained_any class. to make easy to control instantiation, default type of U is Carrier.
	 * @return std::string converted string
	 *
	 * @details
	 * This function is visible from outside of Concrete constrained_any class.
	 *
	 * @note
	 * Although this I/F definition use SFINAE to define this function in case of only Concrete constrained_any class, it is not mandatory to use SFINAE.
	 */
	template <typename U = Carrier, typename std::enable_if<!yan::is_callable_ref<U>::value>::type* = nullptr>
	std::string to_string( void ) const
	{
		// this I/F expects to be called as the member function of constranted_any.

		const Carrier* p_a = static_cast<const Carrier*>( this );

		const special_operation_convert_to_string_if* p_a_soi = p_a->template get_special_operation_if<special_operation_convert_to_string_if>();
		if ( p_a_soi == nullptr ) {
			return 0;
		}

		return p_a_soi->specialized_operation_convert_to_string_proxy();
	}

private:
	/**
	 * @brief proxy I/F to communicate b/w constrained_any and actual value.
	 *
	 * @note
	 * This function is called from const constrained_any class.
	 */
	std::string specialized_operation_convert_to_string_proxy( void ) const override
	{
		return call_to_string();
	}

	/**
	 * @brief call_to_string is the actual implementation of conversion to std::string
	 *
	 * @tparam U Concrete constrained_any class. to make easy to control instantiation, default type of U is Carrier.
	 * @return std::string converted string
	 *
	 * @details
	 * This function is called from specialized_operation_callback.
	 */
	std::string call_to_string( void ) const
	{
		if constexpr ( yan::is_callable_ref<Carrier>::value ) {
			using value_type   = typename std::remove_reference<typename std::remove_cv<typename Carrier::value_type>::type>::type;
			const Carrier* p_a = static_cast<const Carrier*>( this );

			if constexpr ( std::is_same_v<value_type, std::string> ) {
				return p_a->ref();
			} else if constexpr ( std::is_same_v<value_type, std::string_view> ) {
				return std::string( p_a->ref() );
			} else if constexpr ( std::is_same_v<value_type, char*> ) {
				return std::string( p_a->ref() );
			} else if constexpr ( std::is_convertible_v<value_type, std::string> ) {
				return std::string( p_a->ref() );
			} else if constexpr ( std::is_same<std::string, decltype( std::to_string( std::declval<value_type>() ) )>::value ) {
				return std::to_string( p_a->ref() );
			} else {
				static_assert( false, "to_string() is not implemented for this type" );
			}
		} else {
			throw std::logic_error( "call_to_string() is not implemented for constrained_any itself" );
		}
	}
};

// ---------------------------------------------------
// Step 3: Define your own constrained_any class

/**
 * @brief Constrained any class that has the constraint of convertible to std::string
 */
using string_convertible_any = yan::constrained_any<true, special_operation_convert_to_string>;

// ---------------------------------------------------
// Step 4: Use your own constrained_any class

struct Foo_convertible_to_string {
	int value_;

	Foo_convertible_to_string( int value )
	  : value_( value )
	{
	}

	operator std::string( void ) const
	{
		return std::to_string( value_ );
	}
};

int main( void )
{
	// Create a constrained_any object with an int value
	string_convertible_any int_value( 42 );
	std::cout << "int_value: " << int_value.to_string() << std::endl;

	// Create a constrained_any object with a string value
	string_convertible_any str_value( "Hello, World!" );
	std::cout << "str_value: " << str_value.to_string() << std::endl;

	// Create a constrained_any object with a double value
	string_convertible_any double_value( 3.14 );
	std::cout << "double_value: " << double_value.to_string() << std::endl;

	// Create a constrained_any object with a Foo_convertible_to_string value
	Foo_convertible_to_string foo_value( 100 );
	string_convertible_any    foo_any( foo_value );
	std::cout << "foo_any: " << foo_any.to_string() << std::endl;

	// Create a list containter of constrained_any objects
	std::vector<string_convertible_any> vec;
	vec.push_back( int_value );
	vec.push_back( str_value );
	vec.push_back( double_value );
	vec.push_back( foo_any );
	for ( const auto& item : vec ) {
		std::cout << "vector item: " << item.to_string() << std::endl;
	}

	return 0;
}
