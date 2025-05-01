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

#if __cpp_lib_remove_cvref >= 201711L
template <typename T>
using remove_cvref = std::remove_cvref<T>;
#else
template <typename T>
using remove_cvref = std::remove_reference<typename std::remove_cv<T>::type>;
#endif

struct is_callable_ref_impl {
	template <typename T>
	static auto check( T* ) -> decltype( std::declval<T>().ref(), std::true_type() );
	template <typename T>
	static auto check( ... ) -> std::false_type;
};

}   // namespace impl

template <typename T>
struct is_callable_ref : public decltype( impl::is_callable_ref_impl::check<T>( nullptr ) ) {};

struct special_operation_if {
	virtual ~special_operation_if() = default;

	virtual void specialized_operation_callback( void* )       = 0;
	virtual void specialized_operation_callback( void* ) const = 0;
};

template <bool RequiresCopy, template <class> class Constraint, template <class> class... SpecializedOperatorArgs>
class constrained_any_base;

namespace impl {

template <typename T>
struct is_specialized_of_constrained_any : public std::false_type {};

template <bool RequiresCopy, template <class> class Constraint, template <class> class SpecializedOperatorArgs>
struct is_specialized_of_constrained_any<constrained_any_base<RequiresCopy, Constraint, SpecializedOperatorArgs>> : public std::true_type {};

template <typename T, bool AllowUseCopy, template <class> class Constraint>
struct is_acceptable_value_type {
	static constexpr bool value = ( AllowUseCopy ? ( std::is_copy_constructible<T>::value && std::is_copy_assignable<T>::value ) : true ) &&
	                              !( is_specialized_of_constrained_any<T>::value ) &&
	                              !( std::is_same<T, std::any>::value ) &&
	                              Constraint<T>::value;
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

template <typename T, bool AllowCopyConstructAndAssign, template <class> class... SpecializedOperatorArgs>
struct value_carrier;

template <typename T, template <class> class... SpecializedOperatorArgs>
struct value_carrier<T, true, SpecializedOperatorArgs...> : public value_carrier_if<true>, public SpecializedOperatorArgs<value_carrier<T, true, SpecializedOperatorArgs>>... {
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

template <typename T, template <class> class... SpecializedOperatorArgs>
struct value_carrier<T, false, SpecializedOperatorArgs...> : public value_carrier_if<false>, public SpecializedOperatorArgs<value_carrier<T, false, SpecializedOperatorArgs>>... {
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

}   // namespace impl

template <typename T>
struct no_constrained : std::true_type {};
template <typename T>
struct no_specialoperation : special_operation_if {
	void specialized_operation_callback( void* ) override {}
	void specialized_operation_callback( void* ) const override {}
};

template <bool RequiresCopy, template <class> class Constraint, template <class> class... SpecializedOperatorArgs>
class constrained_any_base : public SpecializedOperatorArgs<constrained_any_base<RequiresCopy, Constraint, SpecializedOperatorArgs>>... {
	// static_assert( std::is_base_of<special_operation_if, SpecializedOperatorArgs<constrained_any_base>>::value, "SpecializedOperatorArgs must be derived from special_operation_if" );
	// static_assert( std::is_convertible<SpecializedOperatorArgs<constrained_any_base>*, special_operation_if*>::value, "SpecializedOperatorArgs must be convertible to special_operation_if. Therefore SpecializedOperatorArgs should be PUBLIC derived type from special_operation_if." );

public:
	constrained_any_base()
	  : up_carrier_() {}

	constrained_any_base( const constrained_any_base& src )
#if __cpp_concepts >= 201907L
		requires RequiresCopy
#endif
	  : up_carrier_( src.up_carrier_ == nullptr ? nullptr : src.up_carrier_->mk_clone_by_copy_construction() )
	{
	}

	constrained_any_base( constrained_any_base&& src )
	  : up_carrier_( src.up_carrier_ == nullptr ? nullptr : src.up_carrier_->mk_clone_by_move_construction() ) {}

	constrained_any_base& operator=( const constrained_any_base& rhs )
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

		constrained_any_base( rhs ).swap( *this );

		return *this;
	}

	constrained_any_base& operator=( constrained_any_base&& rhs ) noexcept
	{
		if ( this == &rhs ) return *this;

		if ( ( up_carrier_ != nullptr ) && ( rhs.up_carrier_ != nullptr ) ) {
			if ( up_carrier_->get_type_info() == rhs.up_carrier_->get_type_info() ) {
				up_carrier_->move_value( *rhs.up_carrier_ );
				return *this;
			}
		}

		constrained_any_base( std::move( rhs ) ).swap( *this );

		return *this;
	}

	template <class T, class... Args, typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  impl::is_acceptable_value_type<VT, RequiresCopy, Constraint>::value &&
				  std::is_constructible<VT, Args...>::value>::type* = nullptr>
	explicit constrained_any_base( std::in_place_type_t<T>, Args&&... args )
	  : up_carrier_( make_impl_value_carrier<VT>( std::forward<Args>( args )... ) )
	{
	}

	template <typename T,
	          typename std::enable_if<!impl::is_specialized_of_constrained_any<typename impl::remove_cvref<T>::type>::value>::type* = nullptr>
	constrained_any_base( T&& v )
	  : constrained_any_base( std::in_place_type<std::decay_t<T>>, std::forward<T>( v ) )
	{
	}

	void swap( constrained_any_base& src )
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
				  impl::is_acceptable_value_type<VT, RequiresCopy, Constraint>::value &&
				  std::is_constructible<VT, Args...>::value>::type* = nullptr>
	std::decay_t<T>& emplace( Args&&... args )
	{
		auto             up_vc = make_impl_value_carrier<VT>( std::forward<Args>( args )... );
		std::decay_t<T>* p_ans = &( up_vc->value_ );

		up_carrier_ = std::move( up_vc );

		return *p_ans;
	}

	template <class T, typename VT = std::decay_t<T>,
	          typename std::enable_if<impl::is_acceptable_value_type<VT, RequiresCopy, Constraint>::value>::type* = nullptr>
	constrained_any_base& operator=( T&& rhs )
	{
		using carrier_t = impl::value_carrier<VT, RequiresCopy, SpecializedOperatorArgs...>;

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

	special_operation_if* get_special_operation_if() noexcept
	{
		if ( up_carrier_ == nullptr ) return nullptr;

		return dynamic_cast<special_operation_if*>( up_carrier_.get() );
	}
	const special_operation_if* get_special_operation_if() const noexcept
	{
		if ( up_carrier_ == nullptr ) return nullptr;

		return dynamic_cast<const special_operation_if*>( up_carrier_.get() );
	}

private:
	template <typename T, class... Args>
	static auto make_impl_value_carrier( Args&&... args ) -> std::unique_ptr<impl::value_carrier<T, RequiresCopy, SpecializedOperatorArgs...>>
	{
		return std::make_unique<impl::value_carrier<T, RequiresCopy, SpecializedOperatorArgs...>>( std::in_place_type_t<T> {}, std::forward<Args>( args )... );
	}

	template <typename T>
	auto static_cast_T_carrier() const -> const impl::value_carrier<T, RequiresCopy, SpecializedOperatorArgs...>*
	{
		if ( this->type() != typeid( T ) ) {
			return nullptr;
		}

		return static_cast<const impl::value_carrier<T, RequiresCopy, SpecializedOperatorArgs...>*>( up_carrier_.get() );
	}

	template <typename T>
	auto static_cast_T_carrier() -> impl::value_carrier<T, RequiresCopy, SpecializedOperatorArgs...>*
	{
		if ( this->type() != typeid( T ) ) {
			return nullptr;
		}

		return static_cast<impl::value_carrier<T, RequiresCopy, SpecializedOperatorArgs...>*>( up_carrier_.get() );
	}

	std::unique_ptr<impl::value_carrier_if<RequiresCopy>> up_carrier_;

	template <class T, bool URequiresCopy, template <class> class UConstraint, template <class> class... USpecializedOperator>
	friend T constrained_any_cast( const constrained_any_base<URequiresCopy, UConstraint, USpecializedOperator...>& operand );

	template <class T, bool URequiresCopy, template <class> class UConstraint, template <class> class... USpecializedOperator>
	friend T constrained_any_cast( constrained_any_base<URequiresCopy, UConstraint, USpecializedOperator...>& operand );

	template <class T, bool URequiresCopy, template <class> class UConstraint, template <class> class... USpecializedOperator>
	friend T constrained_any_cast( constrained_any_base<URequiresCopy, UConstraint, USpecializedOperator...>&& operand );

	template <class T, bool URequiresCopy, template <class> class UConstraint, template <class> class... USpecializedOperator>
	friend const T* constrained_any_cast( const constrained_any_base<URequiresCopy, UConstraint, USpecializedOperator...>* operand ) noexcept;

	template <class T, bool URequiresCopy, template <class> class UConstraint, template <class> class... USpecializedOperator>
	friend T* constrained_any_cast( constrained_any_base<URequiresCopy, UConstraint, USpecializedOperator...>* operand ) noexcept;
};

template <class T, bool RequiresCopy, template <class> class Constraint, template <class> class... SpecializedOperatorArgs, class... Args>
constrained_any_base<RequiresCopy, Constraint, SpecializedOperatorArgs...> make_constrained_any( Args&&... args )
{
	return constrained_any_base<RequiresCopy, Constraint, SpecializedOperatorArgs...>( std::in_place_type<T>, std::forward<Args>( args )... );
}

template <class T, bool RequiresCopy, template <class> class Constraint, template <class> class... SpecializedOperatorArgs>
T constrained_any_cast( const constrained_any_base<RequiresCopy, Constraint, SpecializedOperatorArgs...>& operand )
{
	using U = typename std::remove_cv_t<std::remove_reference_t<T>>;
	static_assert( std::is_constructible<T, const U&>::value, "T must be constructible" );

	auto p = operand.template static_cast_T_carrier<U>();
	if ( p == nullptr ) {
		throw std::bad_any_cast();
	}

	return p->ref();
}

template <class T, bool RequiresCopy, template <class> class Constraint, template <class> class... SpecializedOperatorArgs>
T constrained_any_cast( constrained_any_base<RequiresCopy, Constraint, SpecializedOperatorArgs...>& operand )
{
	using U = typename std::remove_cv_t<std::remove_reference_t<T>>;
	static_assert( std::is_constructible<T, U&>::value, "T must be constructible" );

	auto p = operand.template static_cast_T_carrier<U>();
	if ( p == nullptr ) {
		throw std::bad_any_cast();
	}

	return p->ref();
}

template <class T, bool RequiresCopy, template <class> class Constraint, template <class> class... SpecializedOperatorArgs>
T constrained_any_cast( constrained_any_base<RequiresCopy, Constraint, SpecializedOperatorArgs...>&& operand )
{
	using U = typename std::remove_cv_t<std::remove_reference_t<T>>;
	static_assert( std::is_constructible<T, U>::value, "T must be constructible" );

	auto p = operand.template static_cast_T_carrier<U>();
	if ( p == nullptr ) {
		throw std::bad_any_cast();
	}

	return std::move( p->ref() );
}

template <class T, bool RequiresCopy, template <class> class Constraint, template <class> class... SpecializedOperatorArgs>
const T* constrained_any_cast( const constrained_any_base<RequiresCopy, Constraint, SpecializedOperatorArgs...>* operand ) noexcept
{
	static_assert( !std::is_void_v<T>, "T should not be void" );

	if ( operand == nullptr ) return nullptr;

	auto p = operand.template static_cast_T_carrier<T>();
	if ( p == nullptr ) {
		return nullptr;
	}

	return &( p->ref() );
}

template <class T, bool RequiresCopy, template <class> class Constraint, template <class> class... SpecializedOperatorArgs>
T* constrained_any_cast( constrained_any_base<RequiresCopy, Constraint, SpecializedOperatorArgs...>* operand ) noexcept
{
	static_assert( !std::is_void_v<T>, "T should not be void" );

	if ( operand == nullptr ) return nullptr;

	auto p = operand.template static_cast_T_carrier<T>();
	if ( p == nullptr ) {
		return nullptr;
	}

	return &( p->ref() );
}

using constrained_any = constrained_any_base<true, no_constrained, no_specialoperation>;

namespace impl {

struct is_weak_orderable_impl {
	template <typename T>
	static auto check( T* ) -> decltype( std::declval<T>() < std::declval<T>(), std::true_type() );
	template <typename T>
	static auto check( ... ) -> std::false_type;
};

template <typename T>
struct is_weak_orderable : public decltype( is_weak_orderable_impl::check<T>( nullptr ) ) {};

template <typename Carrier>
class special_operation_less : public special_operation_if {
public:
	template <typename U = Carrier, typename std::enable_if<!yan::is_callable_ref<U>::value>::type* = nullptr>
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

		const special_operation_if* p_a_soi = p_a->get_special_operation_if();
		const special_operation_if* p_b_soi = p_b->get_special_operation_if();
		if ( p_a_soi == nullptr ) {
			// this means both this and b have no value. Therefore *this == b.
			// So, *this < b is false.
			return false;
		}

		argument_and_return_data data { p_b_soi, false };
		p_a_soi->specialized_operation_callback( &data );
		return data.result_;
	}

private:
	struct argument_and_return_data {
		const special_operation_if* p_b_;
		bool                        result_;
	};

	void specialized_operation_callback( void* p_b ) override
	{
		argument_and_return_data* p_args_of_b = reinterpret_cast<argument_and_return_data*>( p_b );
		p_args_of_b->result_                  = less_by_value( p_args_of_b->p_b_ );
	}
	void specialized_operation_callback( void* p_b ) const override
	{
		argument_and_return_data* p_args_of_b = reinterpret_cast<argument_and_return_data*>( p_b );
		p_args_of_b->result_                  = less_by_value( p_args_of_b->p_b_ );
	}

	bool less_by_value( const special_operation_if* p_b_if ) const
	{
		if constexpr ( yan::is_callable_ref<Carrier>::value ) {
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

}   // namespace impl

using weak_ordering_any = constrained_any_base<true, impl::is_weak_orderable, impl::special_operation_less>;
inline bool operator<( const weak_ordering_any& lhs, const weak_ordering_any& rhs )
{
	return lhs.less( rhs );
}

namespace impl {

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

template <typename T>
struct is_acceptable_as_unordered_key {
	static constexpr bool value = is_hashable<T>::value && is_callable_equal_to<T>::value;
};

template <typename Carrier>
class special_operation_unordered_key : public special_operation_if {
public:
	template <typename U = Carrier, typename std::enable_if<!yan::is_callable_ref<U>::value>::type* = nullptr>
	size_t hash_value( void ) const
	{
		const Carrier* p_a = static_cast<const Carrier*>( this );

		const special_operation_if* p_a_soi = p_a->get_special_operation_if();
		if ( p_a_soi == nullptr ) {
			return 0;
		}

		argument_and_return_data data { nullptr, false, 0 };
		p_a_soi->specialized_operation_callback( &data );
		return data.hash_value_;
	}

	template <typename U = Carrier, typename std::enable_if<!yan::is_callable_ref<U>::value>::type* = nullptr>
	bool equal_to( const Carrier& b ) const
	{
		const Carrier* p_a = static_cast<const Carrier*>( this );
		const Carrier* p_b = static_cast<const Carrier*>( &b );

		std::type_index ti_a = std::type_index( p_a->type() );
		std::type_index ti_b = std::type_index( p_b->type() );
		if ( ti_a != ti_b ) {
			return false;
		}

		const special_operation_if* p_a_soi = p_a->get_special_operation_if();
		const special_operation_if* p_b_soi = p_b->get_special_operation_if();
		if ( p_a_soi == nullptr ) {
			return false;
		}

		argument_and_return_data data { p_b_soi, false, 0 };
		p_a_soi->specialized_operation_callback( &data );
		return data.equal_to_result_;
	}

private:
	struct argument_and_return_data {
		const special_operation_if* p_b_;
		bool                        equal_to_result_;
		size_t                      hash_value_;
	};

	void specialized_operation_callback( void* p_b ) override
	{
		argument_and_return_data* p_args_of_b = reinterpret_cast<argument_and_return_data*>( p_b );
		if ( p_args_of_b->p_b_ != nullptr ) {
			p_args_of_b->equal_to_result_ = unordered_key_equal_to( p_args_of_b->p_b_ );
		} else {
			p_args_of_b->hash_value_ = hash_value();
		}
	}
	void specialized_operation_callback( void* p_b ) const override
	{
		argument_and_return_data* p_args_of_b = reinterpret_cast<argument_and_return_data*>( p_b );
		if ( p_args_of_b->p_b_ != nullptr ) {
			p_args_of_b->equal_to_result_ = unordered_key_equal_to( p_args_of_b->p_b_ );
		} else {
			p_args_of_b->hash_value_ = hash_value();
		}
	}

	template <typename U = Carrier, typename std::enable_if<yan::is_callable_ref<U>::value>::type* = nullptr>
	size_t hash_value( void ) const
	{
		const Carrier* p_a_carrier = static_cast<const Carrier*>( this );

		return std::hash<typename impl::remove_cvref<typename Carrier::value_type>::type>()( p_a_carrier->ref() );
	}

	bool unordered_key_equal_to( const special_operation_if* p_b_if ) const
	{
		if constexpr ( yan::is_callable_ref<Carrier>::value ) {
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

}   // namespace impl

using unordered_key_any = constrained_any_base<true, impl::is_acceptable_as_unordered_key, impl::special_operation_unordered_key>;
inline bool operator==( const unordered_key_any& lhs, const unordered_key_any& rhs )
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
}   // namespace std

namespace yan {

namespace impl {
template <typename T>
struct is_keyable {
	static constexpr bool value = is_weak_orderable<T>::value && is_callable_equal_to<T>::value && is_hashable<T>::value;
};

template <typename Carrier>
class special_operation_keyable : public special_operation_if {
public:
	template <typename U = Carrier, typename std::enable_if<!yan::is_callable_ref<U>::value>::type* = nullptr>
	size_t hash_value( void ) const
	{
		// this I/F expects to be called as the member function of constranted_any.

		const Carrier* p_a = static_cast<const Carrier*>( this );

		const special_operation_if* p_a_soi = p_a->get_special_operation_if();
		if ( p_a_soi == nullptr ) {
			return 0;
		}

		argument_and_return_data data { operation_type::e_hash_value, nullptr, false, 0 };
		p_a_soi->specialized_operation_callback( &data );
		return data.hash_value_;
	}

	template <typename U = Carrier, typename std::enable_if<!yan::is_callable_ref<U>::value>::type* = nullptr>
	bool equal_to( const Carrier& b ) const
	{
		// this I/F expects to be called as the member function of constranted_any.

		const Carrier* p_a = static_cast<const Carrier*>( this );
		const Carrier* p_b = static_cast<const Carrier*>( &b );

		std::type_index ti_a = std::type_index( p_a->type() );
		std::type_index ti_b = std::type_index( p_b->type() );
		if ( ti_a != ti_b ) {
			return false;
		}

		const special_operation_if* p_a_soi = p_a->get_special_operation_if();
		const special_operation_if* p_b_soi = p_b->get_special_operation_if();
		if ( ( p_a_soi == nullptr ) && ( p_b_soi == nullptr ) ) {
			// in this case, this has no value and b has no value. therefore this and b is same.
			return true;
		} else if ( p_a_soi == nullptr ) {
			// in this case, this has no value and b had void. therefore this and b is NOT equal.
			return false;
		} else if ( p_b_soi == nullptr ) {
			// in this case, this has void and b has no value. therefore this and b is NOT equal.
			return false;
		}

		argument_and_return_data data { operation_type::e_equal_to, p_b_soi, false, 0 };
		p_a_soi->specialized_operation_callback( &data );
		return data.bool_to_result_;
	}

	template <typename U = Carrier, typename std::enable_if<!yan::is_callable_ref<U>::value>::type* = nullptr>
	bool less( const Carrier& b ) const
	{
		// this I/F expects to be called as the member function of constranted_any.

		const Carrier* p_a = static_cast<const Carrier*>( this );
		const Carrier* p_b = static_cast<const Carrier*>( &b );

		std::type_index ti_a = std::type_index( p_a->type() );
		std::type_index ti_b = std::type_index( p_b->type() );
		if ( ti_a != ti_b ) {
			return ti_a < ti_b;
		}

		const special_operation_if* p_a_soi = p_a->get_special_operation_if();
		const special_operation_if* p_b_soi = p_b->get_special_operation_if();
		if ( ( p_a_soi == nullptr ) && ( p_b_soi == nullptr ) ) {
			// in this case, this has no value and b has void. therefore this and b is NOT equal.
			return false;
		} else if ( p_a_soi == nullptr ) {
			// in this case, this has no value and b had void. therefore this and b is NOT equal.
			return true;
		} else if ( p_b_soi == nullptr ) {
			// in this case, this has void and b has no value. therefore this and b is NOT equal.
			return false;
		}

		argument_and_return_data data { operation_type::e_less, p_b_soi, false, 0 };
		p_a_soi->specialized_operation_callback( &data );
		return data.bool_to_result_;
	}

private:
	enum class operation_type {
		e_hash_value,
		e_equal_to,
		e_less
	};
	struct argument_and_return_data {

		operation_type              op_type_;
		const special_operation_if* p_b_;
		bool                        bool_to_result_;
		size_t                      hash_value_;
	};

	void specialized_operation_callback( void* p_b ) override
	{
		argument_and_return_data* p_args_of_b = reinterpret_cast<argument_and_return_data*>( p_b );
		call_operation( p_args_of_b );
	}
	void specialized_operation_callback( void* p_b ) const override
	{
		argument_and_return_data* p_args_of_b = reinterpret_cast<argument_and_return_data*>( p_b );
		call_operation( p_args_of_b );
	}

	void call_operation( argument_and_return_data* p_args_of_b ) const
	{
		switch ( p_args_of_b->op_type_ ) {
			case operation_type::e_hash_value:
				p_args_of_b->hash_value_ = hash_value();
				break;
			case operation_type::e_equal_to:
				if ( p_args_of_b->p_b_ != nullptr ) {
					p_args_of_b->bool_to_result_ = equal_to_by_value( p_args_of_b->p_b_ );
				} else {
					throw std::logic_error( "argument is nullptr for equal_to" );
				}
				break;
			case operation_type::e_less:
				if ( p_args_of_b->p_b_ != nullptr ) {
					p_args_of_b->bool_to_result_ = less_by_value( p_args_of_b->p_b_ );
				} else {
					throw std::logic_error( "argument is nullptr for less" );
				}
				break;
			default:
				throw std::logic_error( "unknown operation type" );
				break;
		}
	}

	template <typename U = Carrier, typename std::enable_if<yan::is_callable_ref<U>::value>::type* = nullptr>
	size_t hash_value( void ) const
	{
		const Carrier* p_a_carrier = static_cast<const Carrier*>( this );

		return std::hash<typename impl::remove_cvref<typename Carrier::value_type>::type>()( p_a_carrier->ref() );
	}

	bool equal_to_by_value( const special_operation_if* p_b_if ) const
	{
		if constexpr ( yan::is_callable_ref<Carrier>::value ) {
			const Carrier* p_a_carrier = static_cast<const Carrier*>( this );
			const Carrier* p_b_carrier = dynamic_cast<const Carrier*>( p_b_if );
			if ( p_b_carrier == nullptr ) {
				throw std::bad_any_cast();
			}

			return p_a_carrier->ref() == p_b_carrier->ref();
		} else {
			throw std::logic_error( "equal_to_by_value() is not implemented for constrained_any itself" );
		}
	}

	bool less_by_value( const special_operation_if* p_b_if ) const
	{
		if constexpr ( yan::is_callable_ref<Carrier>::value ) {
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

}   // namespace impl

using keyable_any = constrained_any_base<true, impl::is_keyable, impl::special_operation_keyable>;
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
struct hash<yan::keyable_any> {
	size_t operator()( const yan::keyable_any& key ) const
	{
		return key.hash_value();
	}
};
}   // namespace std

#endif
