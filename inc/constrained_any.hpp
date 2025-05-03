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

template <bool RequiresCopy, template <class> class... ConstrainAndOperationArgs>
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
template <bool RequiresCopy, template <class> class... ConstrainAndOperationArgs>
struct is_specialized_of_constrained_any<constrained_any<RequiresCopy, ConstrainAndOperationArgs...>> : public std::true_type {};

namespace impl {

// helper metafunction like std::remove cvref for C++17
#if __cpp_lib_remove_cvref >= 201711L
template <typename T>
using remove_cvref = std::remove_cvref<T>;
#else
template <typename T>
using remove_cvref = std::remove_reference<typename std::remove_cv<T>::type>;
#endif

// helper metafunction to check T is acceptable value type or not
template <typename T, bool AllowUseCopy, template <class> class... ConstrainAndOperationArgs>
struct is_acceptable_value_type {
	static constexpr bool value = ( AllowUseCopy ? ( std::is_copy_constructible<T>::value && std::is_copy_assignable<T>::value ) : true ) &&
	                              !( is_specialized_of_constrained_any<T>::value ) &&
	                              !( std::is_same<T, std::any>::value ) &&
	                              ( ... && ConstrainAndOperationArgs<T>::constraint_check_result );
};

struct value_carrier_if_common {
	virtual ~value_carrier_if_common() = default;

	virtual const std::type_info& get_type_info() const noexcept = 0;
};

template <bool AllowUseCopy>
struct value_carrier_if;

template <>
struct value_carrier_if<true> : public value_carrier_if_common {
	virtual value_carrier_if<true>* mk_clone_by_copy_construction() const       = 0;
	virtual value_carrier_if<true>* mk_clone_by_move_construction()             = 0;
	virtual void                    copy_value( const value_carrier_if<true>& ) = 0;
	virtual void                    move_value( value_carrier_if<true>& )       = 0;
};

template <>
struct value_carrier_if<false> : public value_carrier_if_common {
	virtual value_carrier_if<false>* mk_clone_by_move_construction()        = 0;
	virtual void                     move_value( value_carrier_if<false>& ) = 0;
};

template <typename T, bool AllowCopyConstructAndAssign, template <class> class... ConstrainAndOperationArgs>
struct value_carrier;

template <typename T, template <class> class... ConstrainAndOperationArgs>
struct value_carrier<T, true, ConstrainAndOperationArgs...> : public value_carrier_if<true>, public ConstrainAndOperationArgs<value_carrier<T, true, ConstrainAndOperationArgs...>>... {
	using abst_if_t  = value_carrier_if<true>;
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
		return typeid( T );
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

	void copy_value( const abst_if_t& src ) override
	{
		const value_carrier& ref_src = dynamic_cast<const value_carrier&>( src );
		*this                        = ref_src;
	}
	void move_value( abst_if_t& src ) override
	{
		value_carrier& ref_src = dynamic_cast<value_carrier&>( src );
		*this                  = std::move( ref_src );
	}

private:
	value_type value_;
};

template <typename T, template <class> class... ConstrainAndOperationArgs>
struct value_carrier<T, false, ConstrainAndOperationArgs...> : public value_carrier_if<false>, public ConstrainAndOperationArgs<value_carrier<T, false, ConstrainAndOperationArgs...>>... {
	using abst_if_t  = value_carrier_if<false>;
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
		return typeid( T );
	}

	abst_if_t* mk_clone_by_move_construction() override
	{
		value_carrier* p = new value_carrier( std::move( *this ) );
		return p;
	}

	void move_value( abst_if_t& src ) override
	{
		value_carrier& ref_src = dynamic_cast<value_carrier&>( src );
		*this                  = std::move( ref_src );
	}

private:
	value_type value_;
};

struct is_callable_ref_impl {
	template <typename T>
	static auto check( T* ) -> decltype( std::declval<T>().ref(), std::true_type() );
	template <typename T>
	static auto check( ... ) -> std::false_type;
};

template <typename T>
struct is_callable_ref : public decltype( impl::is_callable_ref_impl::check<T>( nullptr ) ) {};

struct is_defined_value_type_impl {
	template <typename T, typename VT = typename impl::remove_cvref<T>::type, typename VT::value_type* = nullptr>
	static auto check( T* ) -> std::true_type;
	template <typename T>
	static auto check( ... ) -> std::false_type;
};

template <typename T>
struct is_defined_value_type : public decltype( is_defined_value_type_impl::check<T>( nullptr ) ) {};

}   // namespace impl

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
	                              std::is_base_of<impl::value_carrier_if_common, T>::value &&
	                              impl::is_defined_value_type<T>::value &&
	                              impl::is_callable_ref<T>::value;
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

/**
 * @brief Constrained any type
 *
 * @tparam RequiresCopy if true, copy constructible and copy assignable are requires for input type, and constrained_any itself supports copy constructor and copy assignment operator.
 * @tparam ConstrainAndOperationArgs template parameter packs for multiple specialized operator classes.
 */
template <bool RequiresCopy, template <class> class... ConstrainAndOperationArgs>
class constrained_any : public ConstrainAndOperationArgs<constrained_any<RequiresCopy, ConstrainAndOperationArgs...>>... {
public:
	constrained_any()
	  : up_carrier_() {}

	constrained_any( const constrained_any& src )
#if __cpp_concepts >= 201907L
		requires RequiresCopy
#endif
	  : up_carrier_( src.up_carrier_ == nullptr ? nullptr : src.up_carrier_->mk_clone_by_copy_construction() )
	{
	}

	constrained_any( constrained_any&& src )
	  : up_carrier_( src.up_carrier_ == nullptr ? nullptr : src.up_carrier_->mk_clone_by_move_construction() ) {}

	constrained_any& operator=( const constrained_any& rhs )
#if __cpp_concepts >= 201907L
		requires RequiresCopy
#endif
	{
		if ( this == &rhs ) return *this;

		if ( ( up_carrier_ != nullptr ) && ( rhs.up_carrier_ != nullptr ) ) {
			if ( up_carrier_->get_type_info() == rhs.up_carrier_->get_type_info() ) {
				up_carrier_->copy_value( *rhs.up_carrier_ );
				return *this;
			}
		}

		constrained_any( rhs ).swap( *this );

		return *this;
	}

	constrained_any& operator=( constrained_any&& rhs ) noexcept
	{
		if ( this == &rhs ) return *this;

		if ( ( up_carrier_ != nullptr ) && ( rhs.up_carrier_ != nullptr ) ) {
			if ( up_carrier_->get_type_info() == rhs.up_carrier_->get_type_info() ) {
				up_carrier_->move_value( *rhs.up_carrier_ );
				return *this;
			}
		}

		constrained_any( std::move( rhs ) ).swap( *this );

		return *this;
	}

	template <class T, class... Args, typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  impl::is_acceptable_value_type<VT, RequiresCopy, ConstrainAndOperationArgs...>::value &&
				  std::is_constructible<VT, Args...>::value>::type* = nullptr>
	explicit constrained_any( std::in_place_type_t<T>, Args&&... args )
	  : up_carrier_( make_impl_value_carrier<VT>( std::forward<Args>( args )... ) )
	{
	}

	template <typename T,
	          typename std::enable_if<!is_specialized_of_constrained_any<typename impl::remove_cvref<T>::type>::value>::type* = nullptr>
	constrained_any( T&& v )
	  : constrained_any( std::in_place_type<std::decay_t<T>>, std::forward<T>( v ) )
	{
	}

	void swap( constrained_any& src )
	{
		std::swap( up_carrier_, src.up_carrier_ );
	}

	void reset() noexcept
	{
		up_carrier_.reset();
	}

	template <class T, class... Args,
	          typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  impl::is_acceptable_value_type<VT, RequiresCopy, ConstrainAndOperationArgs...>::value &&
				  std::is_constructible<VT, Args...>::value>::type* = nullptr>
	std::decay_t<T>& emplace( Args&&... args )
	{
		auto             up_vc = make_impl_value_carrier<VT>( std::forward<Args>( args )... );
		std::decay_t<T>* p_ans = &( up_vc->value_ );

		up_carrier_ = std::move( up_vc );

		return *p_ans;
	}

	template <class T, typename VT = std::decay_t<T>,
	          typename std::enable_if<impl::is_acceptable_value_type<VT, RequiresCopy, ConstrainAndOperationArgs...>::value>::type* = nullptr>
	constrained_any& operator=( T&& rhs )
	{
		using carrier_t = impl::value_carrier<VT, RequiresCopy, ConstrainAndOperationArgs...>;

		if ( up_carrier_ != nullptr ) {
			if ( up_carrier_->get_type_info() == typeid( VT ) ) {
				carrier_t& ref_src = dynamic_cast<carrier_t&>( *( up_carrier_.get() ) );   // TODO: should be static_cast
				ref_src.ref()      = std::forward<T>( rhs );
				return *this;
			}
		}

		up_carrier_ = make_impl_value_carrier<VT>( std::forward<T>( rhs ) );

		return *this;
	}

	bool has_value() const noexcept
	{
		return up_carrier_ != nullptr;
	}

	const std::type_info& type() const noexcept
	{
		if ( up_carrier_ == nullptr ) return typeid( void );

		return up_carrier_->get_type_info();
	}

	template <typename SpecializedOperatorIF>
	SpecializedOperatorIF* get_special_operation_if() noexcept
	{
		if ( up_carrier_ == nullptr ) return nullptr;

		return dynamic_cast<SpecializedOperatorIF*>( up_carrier_.get() );
	}
	template <typename SpecializedOperatorIF>
	const SpecializedOperatorIF* get_special_operation_if() const noexcept
	{
		if ( up_carrier_ == nullptr ) return nullptr;

		return dynamic_cast<const SpecializedOperatorIF*>( up_carrier_.get() );
	}

private:
	template <typename T, class... Args>
	static auto make_impl_value_carrier( Args&&... args ) -> std::unique_ptr<impl::value_carrier<T, RequiresCopy, ConstrainAndOperationArgs...>>
	{
		return std::make_unique<impl::value_carrier<T, RequiresCopy, ConstrainAndOperationArgs...>>( std::in_place_type_t<T> {}, std::forward<Args>( args )... );
	}

	template <typename T>
	auto static_cast_T_carrier() const -> const impl::value_carrier<T, RequiresCopy, ConstrainAndOperationArgs...>*
	{
		if ( this->type() != typeid( T ) ) {
			return nullptr;
		}

		return static_cast<const impl::value_carrier<T, RequiresCopy, ConstrainAndOperationArgs...>*>( up_carrier_.get() );
	}

	template <typename T>
	auto static_cast_T_carrier() -> impl::value_carrier<T, RequiresCopy, ConstrainAndOperationArgs...>*
	{
		if ( this->type() != typeid( T ) ) {
			return nullptr;
		}

		return static_cast<impl::value_carrier<T, RequiresCopy, ConstrainAndOperationArgs...>*>( up_carrier_.get() );
	}

	std::unique_ptr<impl::value_carrier_if<RequiresCopy>> up_carrier_;

	template <class T, bool URequiresCopy, template <class> class... USpecializedOperator>
	friend T constrained_any_cast( const constrained_any<URequiresCopy, USpecializedOperator...>& operand );

	template <class T, bool URequiresCopy, template <class> class... USpecializedOperator>
	friend T constrained_any_cast( constrained_any<URequiresCopy, USpecializedOperator...>& operand );

	template <class T, bool URequiresCopy, template <class> class... USpecializedOperator>
	friend T constrained_any_cast( constrained_any<URequiresCopy, USpecializedOperator...>&& operand );

	template <class T, bool URequiresCopy, template <class> class... USpecializedOperator>
	friend const T* constrained_any_cast( const constrained_any<URequiresCopy, USpecializedOperator...>* operand ) noexcept;

	template <class T, bool URequiresCopy, template <class> class... USpecializedOperator>
	friend T* constrained_any_cast( constrained_any<URequiresCopy, USpecializedOperator...>* operand ) noexcept;
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
template <class T, bool RequiresCopy, template <class> class... ConstrainAndOperationArgs, class... Args>
constrained_any<RequiresCopy, ConstrainAndOperationArgs...> make_constrained_any( Args&&... args )
{
	return constrained_any<RequiresCopy, ConstrainAndOperationArgs...>( std::in_place_type<T>, std::forward<Args>( args )... );
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

template <class T, bool RequiresCopy, template <class> class... ConstrainAndOperationArgs>
T constrained_any_cast( const constrained_any<RequiresCopy, ConstrainAndOperationArgs...>& operand )
{
	using U = typename std::remove_cv_t<std::remove_reference_t<T>>;
	static_assert( std::is_constructible<T, const U&>::value, "T must be constructible from const U&(=std::remove_cvref<T>::type&)" );

	auto p = operand.template static_cast_T_carrier<U>();
	if ( p == nullptr ) {
		throw std::bad_any_cast();
	}

	return p->ref();
}

template <class T, bool RequiresCopy, template <class> class... ConstrainAndOperationArgs>
T constrained_any_cast( constrained_any<RequiresCopy, ConstrainAndOperationArgs...>& operand )
{
	using U = typename std::remove_cv_t<std::remove_reference_t<T>>;
	static_assert( std::is_constructible<T, U&>::value, "T must be constructible from U&(=std::remove_cvref<T>::type&)" );

	auto p = operand.template static_cast_T_carrier<U>();
	if ( p == nullptr ) {
		throw std::bad_any_cast();
	}

	return p->ref();
}

template <class T, bool RequiresCopy, template <class> class... ConstrainAndOperationArgs>
T constrained_any_cast( constrained_any<RequiresCopy, ConstrainAndOperationArgs...>&& operand )
{
	using U = typename std::remove_cv_t<std::remove_reference_t<T>>;
	static_assert( std::is_constructible<T, U>::value, "T must be constructible from U(=std::remove_cvref<T>::type)" );

	auto p = operand.template static_cast_T_carrier<U>();
	if ( p == nullptr ) {
		throw std::bad_any_cast();
	}

	return std::move( p->ref() );
}

template <class T, bool RequiresCopy, template <class> class... ConstrainAndOperationArgs>
const T* constrained_any_cast( const constrained_any<RequiresCopy, ConstrainAndOperationArgs...>* operand ) noexcept
{
	static_assert( !std::is_void_v<T>, "T should not be void" );

	if ( operand == nullptr ) return nullptr;

	auto p = operand.template static_cast_T_carrier<T>();
	if ( p == nullptr ) {
		return nullptr;
	}

	return &( p->ref() );
}

template <class T, bool RequiresCopy, template <class> class... ConstrainAndOperationArgs>
T* constrained_any_cast( constrained_any<RequiresCopy, ConstrainAndOperationArgs...>* operand ) noexcept
{
	static_assert( !std::is_void_v<T>, "T should not be void" );

	if ( operand == nullptr ) return nullptr;

	auto p = operand.template static_cast_T_carrier<T>();
	if ( p == nullptr ) {
		return nullptr;
	}

	return &( p->ref() );
}

using no_constrained_any = constrained_any<true>;   //!< @brief no special operation. this is same to std::any.

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
			// this means both this and b have no value. Therefore *this == b.
			// So, *this < b is false.
			return false;
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
				throw std::bad_any_cast();
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
			return false;
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
				throw std::bad_any_cast();
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
			return 0;
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
 * @brief constrained_any with weak ordering
 *
 * @details
 * This class is used to create a constrained_any type that supports weak ordering.
 * Therefore, this class can be used as a key type in std::map or std::set.
 */
using weak_ordering_any = constrained_any<true, impl::special_operation_less>;
inline bool operator<( const weak_ordering_any& lhs, const weak_ordering_any& rhs )
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
using unordered_key_any = constrained_any<true, impl::special_operation_hash_value, impl::special_operation_equal_to>;
inline bool operator==( const unordered_key_any& lhs, const unordered_key_any& rhs )
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
using keyable_any = constrained_any<true, impl::special_operation_less, impl::special_operation_hash_value, impl::special_operation_equal_to>;
inline bool operator<( const keyable_any& lhs, const keyable_any& rhs )
{
	return lhs.less( rhs );
}

inline bool operator==( const keyable_any& lhs, const keyable_any& rhs )
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
