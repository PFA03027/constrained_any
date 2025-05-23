/**
 * @file constrained_any.hpp
 * @author Teruaki Ata (PFA03027@nifty.com)
 * @brief constrained any type
 * @version 0.1
 * @date 2025-04-26
 *
 * @copyright Copyright (c) 2025, Teruaki Ata (PFA03027@nifty.com)
 *
 */

#ifndef INC_CONSTRAINED_ANY_HPP_
#define INC_CONSTRAINED_ANY_HPP_

#if __has_include( <version> )
#include <version>
#endif

#include <any>
#include <functional>   // for std::hash
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>

namespace yan {   // yet another

namespace impl {
class constrained_any_tag {};

struct value_carrier_if_common {
	virtual ~value_carrier_if_common() = default;

	virtual const std::type_info& get_type_info() const noexcept = 0;
};

}   // namespace impl

template <template <class> class... ConstrainAndOperationArgs>
class constrained_any;

/**
 * @brief primary template to check if T is specialized type of constrained_any
 *
 * @tparam T
 */
template <typename T>
struct is_specialized_of_constrained_any : public std::false_type {};

/**
 * @brief specialization of is_specialized_of_constrained_any for constrained_any
 *
 * In case that T is specialized constrained_any like below:
 * @code {.cpp}
 * bool ret = yan::is_specialized_of_constrained_any<constrained_any<true, special_operation_adapter_call_print>>::value;
 * @endcode
 * this class is derived from std::true_type. and then, class static member variable "value" is true.
 *
 * @tparam RequiresCopy compiler deduces this parameter.
 * @tparam ConstrainAndOperationArgs compiler deduces this parameter.
 */
template <template <class> class... ConstrainAndOperationArgs>
struct is_specialized_of_constrained_any<constrained_any<ConstrainAndOperationArgs...>> : public std::true_type {};

template <>
struct is_specialized_of_constrained_any<impl::constrained_any_tag> : public std::true_type {};

/**
 * @brief meta function to check if T is specialized type of value_carrier class in constrained_any
 *
 * this class is the helper class to implement constrained and operation class.
 *
 * @tparam T type to check
 */
template <typename T>
struct is_value_carrier_of_constrained_any {
	static constexpr bool value = !is_specialized_of_constrained_any<T>::value &&
	                              std::is_base_of<impl::value_carrier_if_common, T>::value;
};

/**
 * @brief meta function to check if T is not specialized type of constrained_any and not value_carrier class in constrained_any
 *
 * this class is the helper class to implement constrained and operation class.
 *
 * @tparam T type to check
 */
template <typename T>
struct is_related_type_of_constrained_any {
	static constexpr bool value = is_specialized_of_constrained_any<T>::value || is_value_carrier_of_constrained_any<T>::value;
};

namespace impl {

// helper metafunction like std::remove cvref for C++17
#if __cpp_lib_remove_cvref >= 201711L
template <typename T>
using remove_cvref = std::remove_cvref<T>;
#else
template <typename T>
using remove_cvref = std::remove_reference<typename std::remove_cv<T>::type>;
#endif

// =====================

struct is_required_copy_constructible_impl {
	template <typename T, typename VT = typename impl::remove_cvref<T>::type>
	static auto check( T* ) -> decltype( VT::require_copy_constructible == true, std::integral_constant<bool, VT::require_copy_constructible> {} );
	template <typename T>
	static auto check( ... ) -> std::false_type;
};

template <typename T>
struct is_required_copy_constructible : public decltype( is_required_copy_constructible_impl::check<T>( nullptr ) ) {};

template <template <class> class... ConstrainAndOperationArgs>
struct are_any_constraints_required_copy_constructible {
	static constexpr bool value = ( ... || is_required_copy_constructible<ConstrainAndOperationArgs<impl::constrained_any_tag>>::value );
};

template <typename T, template <class> class... ConstrainAndOperationArgs>
struct is_satisfy_required_copy_constructible_constraint {
	static constexpr bool are_constrains_required_copy_constructible = are_any_constraints_required_copy_constructible<ConstrainAndOperationArgs...>::value;

	static constexpr bool value = ( are_constrains_required_copy_constructible ? std::is_copy_constructible<T>::value : true );
};

// =====================

struct is_required_move_constructible_impl {
	template <typename T, typename VT = typename impl::remove_cvref<T>::type>
	static auto check( T* ) -> decltype( VT::require_move_constructible == true, std::integral_constant<bool, VT::require_move_constructible> {} );
	template <typename T>
	static auto check( ... ) -> std::false_type;
};

template <typename T>
struct is_required_move_constructible : public decltype( is_required_move_constructible_impl::check<T>( nullptr ) ) {};

template <template <class> class... ConstrainAndOperationArgs>
struct are_any_constraints_required_move_constructible {
private:
	static constexpr bool are_constrains_required_copy_constructible = are_any_constraints_required_copy_constructible<ConstrainAndOperationArgs...>::value;

public:
	// コピー構築を要求されている場合、ムーブ構築をサポートしていない型であってもムーブ構築をコピー構築で代用できるため、ムーブ構築の要求を無効化する。
	static constexpr bool value = are_constrains_required_copy_constructible ? false : ( ... || is_required_move_constructible<ConstrainAndOperationArgs<impl::constrained_any_tag>>::value );
};

template <typename T, template <class> class... ConstrainAndOperationArgs>
struct is_satisfy_required_move_constructible_constraint {
	static constexpr bool are_constrains_required_move_constructible = are_any_constraints_required_move_constructible<ConstrainAndOperationArgs...>::value;

	static constexpr bool value = ( are_constrains_required_move_constructible ? std::is_move_constructible<T>::value : true );
};

// =====================

struct is_constraint_check_result_impl {
	template <typename T, typename VT = typename impl::remove_cvref<T>::type>
	static auto check( T* ) -> decltype( VT::constraint_check_result == true, std::integral_constant<bool, VT::constraint_check_result> {} );
	template <typename T>
	static auto check( ... ) -> std::true_type;
};

template <typename T>
struct is_constraint_check_result : public decltype( is_constraint_check_result_impl::check<T>( nullptr ) ) {};

template <template <class> class Constraint, template <class> class... ConstrainAndOperationArgs>
struct is_constraint_check_result<Constraint<constrained_any<ConstrainAndOperationArgs...>>> : public std::false_type {};

template <template <class> class Constraint>
struct is_constraint_check_result<Constraint<impl::constrained_any_tag>> : public std::false_type {};

template <typename T, template <class> class... ConstrainAndOperationArgs>
struct is_satisfy_constraints {
	static constexpr bool value = ( ... && is_constraint_check_result<ConstrainAndOperationArgs<T>>::value );
};

// =====================

// helper metafunction to check T is acceptable value type or not
template <typename T, template <class> class... ConstrainAndOperationArgs>
struct is_acceptable_value_type {
	static constexpr bool value = is_satisfy_required_copy_constructible_constraint<T, ConstrainAndOperationArgs...>::value &&
	                              is_satisfy_required_move_constructible_constraint<T, ConstrainAndOperationArgs...>::value &&
	                              is_satisfy_constraints<T, ConstrainAndOperationArgs...>::value;
};

template <template <class> class... ConstrainAndOperationArgs, template <class> class... UConstrainAndOperationArgs>
struct is_acceptable_value_type<constrained_any<ConstrainAndOperationArgs...>, UConstrainAndOperationArgs...> {
	static constexpr bool value = false;
};
template <template <class> class... UConstrainAndOperationArgs>
struct is_acceptable_value_type<std::any, UConstrainAndOperationArgs...> {
	static constexpr bool value = false;
};

// =====================

template <bool SupportUseCopy, bool SupportUseMove>
struct value_carrier_if;

template <bool SupportUseMove>
struct value_carrier_if<true, SupportUseMove> : public value_carrier_if_common {
	virtual value_carrier_if<true, SupportUseMove>* mk_clone_by_copy_construction() const                       = 0;
	virtual value_carrier_if<true, SupportUseMove>* mk_clone_by_move_construction()                             = 0;
	virtual value_carrier_if<true, SupportUseMove>* copy_value( const value_carrier_if<true, SupportUseMove>& ) = 0;
	virtual value_carrier_if<true, SupportUseMove>* move_value( value_carrier_if<true, SupportUseMove>& )       = 0;
};

template <>
struct value_carrier_if<false, true> : public value_carrier_if_common {
	virtual value_carrier_if<false, true>* mk_clone_by_move_construction()              = 0;
	virtual value_carrier_if<false, true>* move_value( value_carrier_if<false, true>& ) = 0;
};

template <>
struct value_carrier_if<false, false> : public value_carrier_if_common {
};

// primary template for value_carrier
template <typename T, bool SupportUseCopy, bool SupportUseMove, template <class> class... ConstrainAndOperationArgs>
struct value_carrier;

// specialization for void
template <bool SupportUseMove, template <class> class... ConstrainAndOperationArgs>
struct value_carrier<void, true, SupportUseMove, ConstrainAndOperationArgs...> : public value_carrier_if<true, SupportUseMove> {
	using abst_if_t  = value_carrier_if<true, SupportUseMove>;
	using value_type = void;

	~value_carrier()                                 = default;
	value_carrier()                                  = default;
	value_carrier( const value_carrier& )            = default;
	value_carrier( value_carrier&& )                 = default;
	value_carrier& operator=( const value_carrier& ) = default;
	value_carrier& operator=( value_carrier&& )      = default;

	template <typename... Args>
	value_carrier( std::in_place_type_t<void>, Args&&... args )
	{
	}

	const std::type_info& get_type_info() const noexcept override
	{
		return typeid( void );
	}

	abst_if_t* mk_clone_by_copy_construction() const override
	{
		value_carrier* p = new value_carrier( *this );
		return p;
	}

	abst_if_t* mk_clone_by_move_construction() override
	{
		value_carrier* p = new value_carrier( std::move( *this ) );
		return p;
	}

	abst_if_t* copy_value( const abst_if_t& src ) override
	{
		return nullptr;
	}
	abst_if_t* move_value( abst_if_t& src ) override
	{
		return nullptr;
	}
};

// specialization for void
template <template <class> class... ConstrainAndOperationArgs>
struct value_carrier<void, false, true, ConstrainAndOperationArgs...> : public value_carrier_if<false, true> {
	using abst_if_t  = value_carrier_if<false, true>;
	using value_type = void;

	~value_carrier()                                 = default;
	value_carrier()                                  = default;
	value_carrier( const value_carrier& )            = default;
	value_carrier( value_carrier&& )                 = default;
	value_carrier& operator=( const value_carrier& ) = default;
	value_carrier& operator=( value_carrier&& )      = default;

	template <typename... Args>
	value_carrier( std::in_place_type_t<void>, Args&&... args )
	{
	}

	const std::type_info& get_type_info() const noexcept override
	{
		return typeid( void );
	}

	abst_if_t* mk_clone_by_move_construction() override
	{
		value_carrier* p = new value_carrier( std::move( *this ) );
		return p;
	}

	abst_if_t* move_value( abst_if_t& src ) override
	{
		return nullptr;
	}
};

// specialization for void
template <template <class> class... ConstrainAndOperationArgs>
struct value_carrier<void, false, false, ConstrainAndOperationArgs...> : public value_carrier_if<false, false> {
	using abst_if_t  = value_carrier_if<false, true>;
	using value_type = void;

	~value_carrier()                                 = default;
	value_carrier()                                  = default;
	value_carrier( const value_carrier& )            = default;
	value_carrier( value_carrier&& )                 = default;
	value_carrier& operator=( const value_carrier& ) = default;
	value_carrier& operator=( value_carrier&& )      = default;

	template <typename... Args>
	value_carrier( std::in_place_type_t<void>, Args&&... args )
	{
	}

	const std::type_info& get_type_info() const noexcept override
	{
		return typeid( void );
	}
};

template <typename T, bool SupportUseMove, template <class> class... ConstrainAndOperationArgs>
struct value_carrier<T, true, SupportUseMove, ConstrainAndOperationArgs...> : public value_carrier_if<true, SupportUseMove>, public ConstrainAndOperationArgs<value_carrier<T, true, SupportUseMove, ConstrainAndOperationArgs...>>... {
	using abst_if_t  = value_carrier_if<true, SupportUseMove>;
	using value_type = T;

	~value_carrier()                                 = default;
	value_carrier()                                  = default;
	value_carrier( const value_carrier& )            = default;
	value_carrier( value_carrier&& )                 = default;
	value_carrier& operator=( const value_carrier& ) = default;
	value_carrier& operator=( value_carrier&& )      = default;

	template <typename... Args>
	value_carrier( std::in_place_type_t<T>, Args&&... args )
	  : value_( std::forward<Args>( args )... )
	{
	}

	T& ref( void )
	{
		return value_;
	}

	const T& ref( void ) const
	{
		return value_;
	}

	const std::type_info& get_type_info() const noexcept override
	{
		return typeid( value_type );
	}

	abst_if_t* mk_clone_by_copy_construction() const override
	{
		value_carrier* p = new value_carrier( *this );
		return p;
	}

	abst_if_t* mk_clone_by_move_construction() override
	{
		value_carrier* p = new value_carrier( std::move( *this ) );
		return p;
	}

	abst_if_t* copy_value( const abst_if_t& src ) override
	{
		const value_carrier& ref_src = dynamic_cast<const value_carrier&>( src );
		if constexpr ( std::is_copy_assignable<value_carrier>::value ) {
			*this = ref_src;

			return nullptr;
		} else {
			value_carrier* p = new value_carrier( ref_src );
			return p;
		}
	}
	abst_if_t* move_value( abst_if_t& src ) override
	{
		value_carrier& ref_src = dynamic_cast<value_carrier&>( src );
		if constexpr ( std::is_move_assignable<value_carrier>::value ) {
			*this = std::move( ref_src );

			return nullptr;
		} else {
			value_carrier* p = new value_carrier( std::move( ref_src ) );
			return p;
		}
	}

private:
	value_type value_;
};

template <typename T, template <class> class... ConstrainAndOperationArgs>
struct value_carrier<T, false, true, ConstrainAndOperationArgs...> : public value_carrier_if<false, true>, public ConstrainAndOperationArgs<value_carrier<T, false, true, ConstrainAndOperationArgs...>>... {
	using abst_if_t  = value_carrier_if<false, true>;
	using value_type = T;

	~value_carrier()                                 = default;
	value_carrier()                                  = default;
	value_carrier( const value_carrier& )            = default;
	value_carrier( value_carrier&& )                 = default;
	value_carrier& operator=( const value_carrier& ) = default;
	value_carrier& operator=( value_carrier&& )      = default;

	template <typename... Args>
	value_carrier( std::in_place_type_t<T>, Args&&... args )
	  : value_( std::forward<Args>( args )... )
	{
	}

	T& ref( void )
	{
		return value_;
	}

	const T& ref( void ) const
	{
		return value_;
	}

	const std::type_info& get_type_info() const noexcept override
	{
		return typeid( value_type );
	}

	abst_if_t* mk_clone_by_move_construction() override
	{
		value_carrier* p = new value_carrier( std::move( *this ) );
		return p;
	}

	abst_if_t* move_value( abst_if_t& src ) override
	{
		value_carrier& ref_src = dynamic_cast<value_carrier&>( src );
		if constexpr ( std::is_move_assignable<value_carrier>::value ) {
			*this = std::move( ref_src );

			return nullptr;
		} else {
			value_carrier* p = new value_carrier( std::move( ref_src ) );
			return p;
		}
	}

private:
	value_type value_;
};

template <typename T, template <class> class... ConstrainAndOperationArgs>
struct value_carrier<T, false, false, ConstrainAndOperationArgs...> : public value_carrier_if<false, false>, public ConstrainAndOperationArgs<value_carrier<T, false, false, ConstrainAndOperationArgs...>>... {
	using abst_if_t  = value_carrier_if<false, false>;
	using value_type = T;

	~value_carrier()                                 = default;
	value_carrier()                                  = default;
	value_carrier( const value_carrier& )            = default;
	value_carrier( value_carrier&& )                 = default;
	value_carrier& operator=( const value_carrier& ) = default;
	value_carrier& operator=( value_carrier&& )      = default;

	template <typename... Args>
	value_carrier( std::in_place_type_t<T>, Args&&... args )
	  : value_( std::forward<Args>( args )... )
	{
	}

	T& ref( void )
	{
		return value_;
	}

	const T& ref( void ) const
	{
		return value_;
	}

	const std::type_info& get_type_info() const noexcept override
	{
		return typeid( value_type );
	}

private:
	value_type value_;
};

}   // namespace impl

/**
 * @brief Constrained any type
 *
 * @tparam ConstrainAndOperationArgs template parameter packs for multiple specialized operator classes.
 *
 * @note
 * It is implemented based on the following concepts:
 * member variable up_carrier_ is always valid (= non nullptr).
 */
template <template <class> class... ConstrainAndOperationArgs>
class constrained_any : public ConstrainAndOperationArgs<constrained_any<ConstrainAndOperationArgs...>>... {
	static constexpr bool RequiresCopy = impl::are_any_constraints_required_copy_constructible<ConstrainAndOperationArgs...>::value;
	static constexpr bool RequiresMove = impl::are_any_constraints_required_move_constructible<ConstrainAndOperationArgs...>::value;

public:
	constrained_any()
	  : up_carrier_( make_impl_value_carrier<void>() ) {}

	constrained_any( const constrained_any& src )
#if __cpp_concepts >= 201907L
		requires RequiresCopy
#endif
	  : up_carrier_( src.up_carrier_->mk_clone_by_copy_construction() )
	{
	}

	constrained_any( constrained_any&& src )
#if __cpp_concepts >= 201907L
		requires RequiresCopy || RequiresMove
#endif
	  : up_carrier_( src.up_carrier_->mk_clone_by_move_construction() )
	{
	}

	constrained_any& operator=( const constrained_any& rhs )
#if __cpp_concepts >= 201907L
		requires RequiresCopy
#endif
	{
		if ( this == &rhs ) return *this;

		if ( this->type() == rhs.type() ) {
			auto p = up_carrier_->copy_value( *rhs.up_carrier_ );
			if ( p != nullptr ) {
				up_carrier_.reset( p );
			}
			return *this;
		}

		constrained_any( rhs ).swap( *this );

		return *this;
	}

	constrained_any& operator=( constrained_any&& rhs ) noexcept
#if __cpp_concepts >= 201907L
		requires RequiresCopy || RequiresMove
#endif
	{
		if ( this == &rhs ) return *this;

		if ( this->type() == rhs.type() ) {
			auto p = up_carrier_->move_value( *rhs.up_carrier_ );
			if ( p != nullptr ) {
				up_carrier_.reset( p );
			}
			return *this;
		}

		constrained_any( std::move( rhs ) ).swap( *this );

		return *this;
	}

	template <class T, class... Args, typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  impl::is_acceptable_value_type<VT, ConstrainAndOperationArgs...>::value &&
				  std::is_constructible<VT, Args...>::value>::type* = nullptr>
	explicit constrained_any( std::in_place_type_t<T>, Args&&... args )
	  : up_carrier_( make_impl_value_carrier<VT>( std::forward<Args>( args )... ) )
	{
	}

	template <class T, typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  ( RequiresCopy || RequiresMove ) &&
				  !is_related_type_of_constrained_any<typename impl::remove_cvref<T>::type>::value &&
				  impl::is_acceptable_value_type<VT, ConstrainAndOperationArgs...>::value>::type* = nullptr>
	constrained_any( T&& v )
	  : constrained_any( std::in_place_type<std::decay_t<T>>, std::forward<T>( v ) )
	{
	}

	void swap( constrained_any& src )
	{
		up_carrier_.swap( src.up_carrier_ );
	}

	void reset() noexcept
	{
		constrained_any().swap( *this );
	}

	template <class T, class... Args,
	          typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  !is_related_type_of_constrained_any<typename impl::remove_cvref<T>::type>::value &&
				  impl::is_acceptable_value_type<VT, ConstrainAndOperationArgs...>::value &&
				  std::is_constructible<VT, Args...>::value>::type* = nullptr>
	std::decay_t<T>& emplace( Args&&... args )
	{
		auto             up_vc = make_impl_value_carrier<VT>( std::forward<Args>( args )... );
		std::decay_t<T>* p_ans = &( up_vc->value_ );

		up_carrier_ = std::move( up_vc );

		return *p_ans;
	}

	template <class T, typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  ( RequiresCopy || RequiresMove ) &&
				  !is_related_type_of_constrained_any<typename impl::remove_cvref<T>::type>::value &&
				  impl::is_acceptable_value_type<VT, ConstrainAndOperationArgs...>::value>::type* = nullptr>
	constrained_any& operator=( T&& rhs )
	{
		if ( this->type() == typeid( VT ) ) {
			using carrier_t    = impl::value_carrier<VT, RequiresCopy, RequiresMove, ConstrainAndOperationArgs...>;
			carrier_t& ref_src = dynamic_cast<carrier_t&>( *( up_carrier_.get() ) );   // TODO: should be static_cast
			ref_src.ref()      = std::forward<T>( rhs );
			return *this;
		}

		up_carrier_ = make_impl_value_carrier<VT>( std::forward<T>( rhs ) );

		return *this;
	}

	bool has_value() const noexcept
	{
		return ( this->type() != typeid( void ) );
	}

	const std::type_info& type() const noexcept
	{
		return up_carrier_->get_type_info();
	}

	template <typename SpecializedOperatorIF>
	SpecializedOperatorIF* get_special_operation_if() noexcept
	{
		return dynamic_cast<SpecializedOperatorIF*>( up_carrier_.get() );
	}
	template <typename SpecializedOperatorIF>
	const SpecializedOperatorIF* get_special_operation_if() const noexcept
	{
		return dynamic_cast<const SpecializedOperatorIF*>( up_carrier_.get() );
	}

private:
	template <typename T, class... Args>
	static auto make_impl_value_carrier( Args&&... args ) -> std::unique_ptr<impl::value_carrier<T, RequiresCopy, RequiresMove, ConstrainAndOperationArgs...>>
	{
		return std::make_unique<impl::value_carrier<T, RequiresCopy, RequiresMove, ConstrainAndOperationArgs...>>( std::in_place_type_t<T> {}, std::forward<Args>( args )... );
	}

	template <typename T>
	auto static_cast_T_carrier() const -> const impl::value_carrier<T, RequiresCopy, RequiresMove, ConstrainAndOperationArgs...>*
	{
		if ( this->type() != typeid( T ) ) {
			return nullptr;
		}

		return static_cast<const impl::value_carrier<T, RequiresCopy, RequiresMove, ConstrainAndOperationArgs...>*>( up_carrier_.get() );
	}

	template <typename T>
	auto static_cast_T_carrier() -> impl::value_carrier<T, RequiresCopy, RequiresMove, ConstrainAndOperationArgs...>*
	{
		if ( this->type() != typeid( T ) ) {
			return nullptr;
		}

		return static_cast<impl::value_carrier<T, RequiresCopy, RequiresMove, ConstrainAndOperationArgs...>*>( up_carrier_.get() );
	}

	std::unique_ptr<impl::value_carrier_if<RequiresCopy, RequiresMove>> up_carrier_;

	template <class T, template <class> class... USpecializedOperator>
	friend T constrained_any_cast( const constrained_any<USpecializedOperator...>& operand );

	template <class T, template <class> class... USpecializedOperator>
	friend T constrained_any_cast( constrained_any<USpecializedOperator...>& operand );

	template <class T, template <class> class... USpecializedOperator>
	friend T constrained_any_cast( constrained_any<USpecializedOperator...>&& operand );

	template <class T, template <class> class... USpecializedOperator>
	friend const T* constrained_any_cast( const constrained_any<USpecializedOperator...>* operand ) noexcept;

	template <class T, template <class> class... USpecializedOperator>
	friend T* constrained_any_cast( constrained_any<USpecializedOperator...>* operand ) noexcept;
};

/**
 * @brief helper function to create constrained_any object
 *
 * @tparam T type of the value which you want to store in constrained_any
 * @tparam RequiresCopy if true, created constrained_any object supports copy constructor and copy assignment operator.
 * @tparam ConstrainAndOperationArgs template parameter packs for multiple constrained and specialized operator classes.
 * @tparam Args constructor argument types for the value type
 * @param args constructor arguments for the value type
 * @return constrained_any<RequiresCopy, ConstrainAndOperationArgs...> created constrained_any object
 */
template <class T, template <class> class... ConstrainAndOperationArgs, class... Args>
constrained_any<ConstrainAndOperationArgs...> make_constrained_any( Args&&... args )
{
	return constrained_any<ConstrainAndOperationArgs...>( std::in_place_type<T>, std::forward<Args>( args )... );
}

/**
 * @brief helper function to create specialized constrained_any object
 *
 * @tparam T type of the value which you want to store in constrained_any
 * @tparam SpecializedConstraintAny specialized constrained_any class what you want to create.
 * @tparam Args constructor argument types for the value type
 * @param args constructor arguments for the value type
 * @return SpecializedConstraintAny created specialized constrained_any object
 */
template <class T, typename SpecializedConstraintAny, class... Args, typename std::enable_if<is_specialized_of_constrained_any<SpecializedConstraintAny>::value>::type* = nullptr>
SpecializedConstraintAny make_constrained_any( Args&&... args )
{
	return SpecializedConstraintAny( std::in_place_type<T>, std::forward<Args>( args )... );
}

template <class T, template <class> class... ConstrainAndOperationArgs>
T constrained_any_cast( const constrained_any<ConstrainAndOperationArgs...>& operand )
{
	using U = typename std::remove_cv_t<std::remove_reference_t<T>>;
	static_assert( std::is_constructible<T, const U&>::value, "T must be constructible from const U&(=std::remove_cvref<T>::type&)" );

	auto p = operand.template static_cast_T_carrier<U>();
	if ( p == nullptr ) {
		throw std::bad_any_cast();
	}

	return p->ref();
}

template <class T, template <class> class... ConstrainAndOperationArgs>
T constrained_any_cast( constrained_any<ConstrainAndOperationArgs...>& operand )
{
	using U = typename std::remove_cv_t<std::remove_reference_t<T>>;
	static_assert( std::is_constructible<T, U&>::value, "T must be constructible from U&(=std::remove_cvref<T>::type&)" );

	auto p = operand.template static_cast_T_carrier<U>();
	if ( p == nullptr ) {
		throw std::bad_any_cast();
	}

	return p->ref();
}

template <class T, template <class> class... ConstrainAndOperationArgs>
T constrained_any_cast( constrained_any<ConstrainAndOperationArgs...>&& operand )
{
	using U = typename std::remove_cv_t<std::remove_reference_t<T>>;
	static_assert( std::is_constructible<T, U>::value, "T must be constructible from U(=std::remove_cvref<T>::type)" );

	auto p = operand.template static_cast_T_carrier<U>();
	if ( p == nullptr ) {
		throw std::bad_any_cast();
	}

	return std::move( p->ref() );
}

template <class T, template <class> class... ConstrainAndOperationArgs>
const T* constrained_any_cast( const constrained_any<ConstrainAndOperationArgs...>* operand ) noexcept
{
	static_assert( !std::is_void_v<T>, "T should not be void" );

	if ( operand == nullptr ) return nullptr;

	auto p = operand.template static_cast_T_carrier<T>();
	if ( p == nullptr ) {
		return nullptr;
	}

	return &( p->ref() );
}

template <class T, template <class> class... ConstrainAndOperationArgs>
T* constrained_any_cast( constrained_any<ConstrainAndOperationArgs...>* operand ) noexcept
{
	static_assert( !std::is_void_v<T>, "T should not be void" );

	if ( operand == nullptr ) return nullptr;

	auto p = operand.template static_cast_T_carrier<T>();
	if ( p == nullptr ) {
		return nullptr;
	}

	return &( p->ref() );
}

namespace impl {

// -----------------------------------------
// Constraint implementation section

struct is_weak_orderable_impl {
	template <typename T>
	static auto check( T* ) -> decltype( std::declval<T>() < std::declval<T>(), std::true_type() );
	template <typename T>
	static auto check( ... ) -> std::false_type;
};

template <typename T>
struct is_weak_orderable : public decltype( is_weak_orderable_impl::check<T>( nullptr ) ) {};

struct is_hashable_impl {
	template <typename T, typename RVCVR_T = typename impl::remove_cvref<T>::type>
	static auto check( T* ) -> decltype( std::hash<RVCVR_T>()( std::declval<RVCVR_T>() ), std::true_type() );
	template <typename T, typename RVCVR_T = typename impl::remove_cvref<T>::type>
	static auto check( ... ) -> std::false_type;
};
template <typename T>
struct is_hashable : public decltype( is_hashable_impl::check<T>( nullptr ) ) {};

struct is_callable_equal_to_impl {
	template <typename T>
	static auto check( T* ) -> decltype( std::declval<T>() == std::declval<T>(), std::true_type() );
	template <typename T>
	static auto check( ... ) -> std::false_type;
};
template <typename T>
struct is_callable_equal_to : public decltype( is_callable_equal_to_impl::check<T>( nullptr ) ) {};

// -----------------------------------------
// Special Operation implementation section

template <typename Carrier>
class special_operation_copyable {
public:
	static constexpr bool require_copy_constructible = true;
};

template <typename Carrier>
class special_operation_movable {
public:
	static constexpr bool require_move_constructible = true;
};

class special_operation_less_if {
public:
	virtual ~special_operation_less_if() = default;

	virtual bool specialized_operation_less_proxy( const special_operation_less_if* ) const = 0;
};

template <typename Carrier>
class special_operation_less : public special_operation_less_if {
public:
	static constexpr bool constraint_check_result = !is_related_type_of_constrained_any<Carrier>::value &&
	                                                is_weak_orderable<Carrier>::value;

	template <typename U = Carrier, typename std::enable_if<is_specialized_of_constrained_any<U>::value>::type* = nullptr>
	bool less( const Carrier& b ) const
	{
		// this I/F is expected that constranted_any calls this function.

		const Carrier* p_a = static_cast<const Carrier*>( this );
		const Carrier* p_b = static_cast<const Carrier*>( &b );

		std::type_index ti_a = std::type_index( p_a->type() );
		std::type_index ti_b = std::type_index( p_b->type() );
		if ( ti_a != ti_b ) {
			return ti_a < ti_b;
		}

		const special_operation_less_if* p_a_soi = p_a->template get_special_operation_if<special_operation_less_if>();
		const special_operation_less_if* p_b_soi = p_b->template get_special_operation_if<special_operation_less_if>();
		if ( p_a_soi == nullptr ) {
			throw std::logic_error( "constrained_any should be get valid pointer of special_operation_less_if. But it failed." );
		}

		return p_a_soi->specialized_operation_less_proxy( p_b_soi );
	}

private:
	bool specialized_operation_less_proxy( const special_operation_less_if* p_b_if ) const override
	{
		return less_by_value( p_b_if );
	}

	bool less_by_value( const special_operation_less_if* p_b_if ) const
	{
		if constexpr ( is_value_carrier_of_constrained_any<Carrier>::value ) {
			const Carrier* p_a_carrier = static_cast<const Carrier*>( this );
			const Carrier* p_b_carrier = dynamic_cast<const Carrier*>( p_b_if );
			if ( p_b_carrier == nullptr ) {
				throw std::logic_error( "p_b_if of less_by_value() should be success to cast to same type to this. But it failed." );
			}

			return p_a_carrier->ref() < p_b_carrier->ref();
		} else {
			throw std::logic_error( "less_by_value() is not implemented for constrained_any itself" );
		}
	}
};

class special_operation_equal_to_if {
public:
	virtual ~special_operation_equal_to_if() = default;

	virtual bool specialized_operation_equal_to_proxy( const special_operation_equal_to_if* ) const = 0;
};

template <typename Carrier>
class special_operation_equal_to : public special_operation_equal_to_if {
public:
	static constexpr bool constraint_check_result = !is_related_type_of_constrained_any<Carrier>::value &&
	                                                is_callable_equal_to<Carrier>::value;

	template <typename U = Carrier, typename std::enable_if<is_specialized_of_constrained_any<U>::value>::type* = nullptr>
	bool equal_to( const Carrier& b ) const
	{
		const Carrier* p_a = static_cast<const Carrier*>( this );
		const Carrier* p_b = static_cast<const Carrier*>( &b );

		std::type_index ti_a = std::type_index( p_a->type() );
		std::type_index ti_b = std::type_index( p_b->type() );
		if ( ti_a != ti_b ) {
			return false;
		}

		const special_operation_equal_to_if* p_a_soi = p_a->template get_special_operation_if<special_operation_equal_to_if>();
		const special_operation_equal_to_if* p_b_soi = p_b->template get_special_operation_if<special_operation_equal_to_if>();
		if ( p_a_soi == nullptr ) {
			throw std::logic_error( "constrained_any should be get valid pointer of special_operation_equal_to_if. But it failed." );
		}

		return p_a_soi->specialized_operation_equal_to_proxy( p_b_soi );
	}

private:
	bool specialized_operation_equal_to_proxy( const special_operation_equal_to_if* p_b_if ) const override
	{
		return unordered_key_equal_to( p_b_if );
	}

	bool unordered_key_equal_to( const special_operation_equal_to_if* p_b_if ) const
	{
		if constexpr ( is_value_carrier_of_constrained_any<Carrier>::value ) {
			const Carrier* p_a_carrier = static_cast<const Carrier*>( this );
			const Carrier* p_b_carrier = dynamic_cast<const Carrier*>( p_b_if );
			if ( p_b_carrier == nullptr ) {
				throw std::logic_error( "p_b_if of less_by_value() should be success to cast to same type to this. But it failed." );
			}

			return p_a_carrier->ref() == p_b_carrier->ref();
		} else {
			throw std::logic_error( "unordered_key_equal_to() is not implemented for constrained_any itself" );
		}
	}
};

class special_operation_hash_value_if {
public:
	virtual ~special_operation_hash_value_if() = default;

	virtual size_t specialized_operation_hash_value_proxy( void ) const = 0;
};

template <typename Carrier>
class special_operation_hash_value : public special_operation_hash_value_if {
public:
	static constexpr bool constraint_check_result = !is_related_type_of_constrained_any<Carrier>::value &&
	                                                is_hashable<Carrier>::value;

	template <typename U = Carrier, typename std::enable_if<is_specialized_of_constrained_any<U>::value>::type* = nullptr>
	size_t hash_value( void ) const
	{
		const Carrier* p_a = static_cast<const Carrier*>( this );

		const special_operation_hash_value_if* p_a_soi = p_a->template get_special_operation_if<special_operation_hash_value_if>();
		if ( p_a_soi == nullptr ) {
			throw std::logic_error( "constrained_any should be get valid pointer of special_operation_hash_value_if. But it failed." );
		}

		return p_a_soi->specialized_operation_hash_value_proxy();
	}

private:
	size_t specialized_operation_hash_value_proxy( void ) const override
	{
		return hash_value();
	}

	template <typename U = Carrier, typename std::enable_if<is_value_carrier_of_constrained_any<U>::value>::type* = nullptr>
	size_t hash_value( void ) const
	{
		const Carrier* p_a_carrier = static_cast<const Carrier*>( this );

		return std::hash<typename impl::remove_cvref<typename Carrier::value_type>::type>()( p_a_carrier->ref() );
	}
};

}   // namespace impl

/**
 * @brief any type that could stores the copy constructible type
 *
 * This constrained any has copy/move constructor and copy/move assigner.
 *
 * @note
 * This is same to std::any.
 */
using copyable_any = constrained_any<impl::special_operation_copyable>;

/**
 * @brief any type that could stores the move constructible type
 *
 * This constrained any has move constructor and move assigner.
 */
using movable_any = constrained_any<impl::special_operation_movable>;

/**
 * @brief constrained_any with weak ordering
 *
 * @details
 * This class is used to create a constrained_any type that supports weak ordering.
 * Therefore, this class can be used as a key type in std::map or std::set.
 */
using weak_ordering_any = constrained_any<impl::special_operation_copyable, impl::special_operation_less>;

/**
 * @brief less operator(operator <) of weak_ordering_any
 *
 * @tparam T weak_ordering_any is only acceptable
 * @param lhs left side variable of operator <
 * @param rhs right side variable of operator <
 * @return expression result of lhs < rhs
 *
 * @note
 * any type absoves all type.@n
 * Therefore, because un-explicit translation constructor leads implicit conversion for all type, it leads unexpected overload resolution of general name function.
 * To avoid this implicit conversion, general name function is defined by template parameter T with SFINE to apply only target constraind_any<...> type.
 *
 */
template <typename T, typename std::enable_if<!std::is_same<T, weak_ordering_any>::value>::type* = nullptr>
inline bool operator<( const T& lhs, const T& rhs )
{
	return lhs.less( rhs );
}

/**
 * @brief constrained_any satisfied unordered_set/unordered_map key requirements
 *
 * @details
 * This class is used to create a constrained_any type that supports operator== and std::hash<unordered_key_any>.
 * Therefore, this class can be used as a key type in std::unordered_set or std::unordered_map.
 */
using unordered_key_any = constrained_any<impl::special_operation_copyable, impl::special_operation_hash_value, impl::special_operation_equal_to>;

/**
 * @brief less operator(operator ==) of unordered_key_any
 *
 * @tparam T unordered_key_any is only acceptable
 * @param lhs left side variable of operator ==
 * @param rhs right side variable of operator ==
 * @return expression result of lhs < rhs
 *
 * @note
 * any type absoves all type.@n
 * Therefore, because un-explicit translation constructor leads implicit conversion for all type, it leads unexpected overload resolution of general name function.
 * To avoid this implicit conversion, general name function is defined by template parameter T with SFINE to apply only target constraind_any<...> type.
 *
 */
template <typename T, typename std::enable_if<!std::is_same<T, unordered_key_any>::value>::type* = nullptr>
inline bool operator==( const T& lhs, const T& rhs )
{
	return lhs.equal_to( rhs );
}

/**
 * @brief constrained_any satisfied std::unordered_set/std::unordered_map/std::set/std::map key requirements
 *
 * @details
 * This class is used to create a constrained_any type that supports operator<, operator== and std::hash<keyable_any>.
 * Therefore, this class can be used as a key type in std::unordered_set or std::unordered_map.
 * This class can also be used as a key type in std::set or std::map.
 */
using keyable_any = constrained_any<impl::special_operation_copyable, impl::special_operation_less, impl::special_operation_hash_value, impl::special_operation_equal_to>;

/**
 * @brief less operator(operator <) of keyable_any
 *
 * @tparam T keyable_any is only acceptable
 * @param lhs left side variable of operator <
 * @param rhs right side variable of operator <
 * @return expression result of lhs < rhs
 *
 * @note
 * any type absoves all type.@n
 * Therefore, because un-explicit translation constructor leads implicit conversion for all type, it leads unexpected overload resolution of general name function.
 * To avoid this implicit conversion, general name function is defined by template parameter T with SFINE to apply only target constraind_any<...> type.
 *
 */
template <typename T, typename std::enable_if<!std::is_same<T, keyable_any>::value>::type* = nullptr>
inline bool operator<( const T& lhs, const T& rhs )
{
	return lhs.less( rhs );
}

/**
 * @brief less operator(operator ==) of keyable_any
 *
 * @tparam T keyable_any is only acceptable
 * @param lhs left side variable of operator ==
 * @param rhs right side variable of operator ==
 * @return expression result of lhs < rhs
 *
 * @note
 * any type absoves all type.@n
 * Therefore, because un-explicit translation constructor leads implicit conversion for all type, it leads unexpected overload resolution of general name function.
 * To avoid this implicit conversion, general name function is defined by template parameter T with SFINE to apply only target constraind_any<...> type.
 *
 */
template <typename T, typename std::enable_if<!std::is_same<T, keyable_any>::value>::type* = nullptr>
inline bool operator==( const T& lhs, const T& rhs )
{
	return lhs.equal_to( rhs );
}

}   // namespace yan

namespace std {
template <>
struct hash<yan::unordered_key_any> {
	size_t operator()( const yan::unordered_key_any& key ) const
	{
		return key.hash_value();
	}
};
template <>
struct hash<yan::keyable_any> {
	size_t operator()( const yan::keyable_any& key ) const
	{
		return key.hash_value();
	}
};

}   // namespace std

#endif
