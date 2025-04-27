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

#include <any>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace yth {   // yet another
namespace impl {

template <bool AllowUseCopy>
struct value_carrier_if;

template <>
struct value_carrier_if<true> {
	virtual const std::type_info&   get_type_info() const noexcept              = 0;
	virtual value_carrier_if<true>* mk_clone_by_copy_construction() const       = 0;
	virtual value_carrier_if<true>* mk_clone_by_move_construction()             = 0;
	virtual void                    copy_value( const value_carrier_if<true>& ) = 0;
	virtual void                    move_value( value_carrier_if<true>& )       = 0;
};

template <>
struct value_carrier_if<false> {
	virtual const std::type_info&    get_type_info() const noexcept         = 0;
	virtual value_carrier_if<false>* mk_clone_by_move_construction()        = 0;
	virtual void                     move_value( value_carrier_if<false>& ) = 0;
};

template <typename T, bool AllowCopyConstructAndAssign>
struct value_carrier;

template <typename T>
struct value_carrier<T, true> : public value_carrier_if<true> {
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

template <typename T>
struct value_carrier<T, false> : public value_carrier_if<false> {
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

template <bool AllowToUseCopy = true, template <class> class Constraint = no_constrained>
class constrained_any {
public:
	constrained_any()
	  : up_carrier_() {}

	constrained_any( const constrained_any& src )
		requires AllowToUseCopy
	  : up_carrier_( src.up_carrier_ == nullptr ? nullptr : src.up_carrier_->mk_clone_by_copy_construction() )
	{
	}

	constrained_any( constrained_any&& src )
	  : up_carrier_( src.up_carrier_ == nullptr ? nullptr : src.up_carrier_->mk_clone_by_move_construction() ) {}

	constrained_any& operator=( const constrained_any& rhs )
		requires AllowToUseCopy
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

	template <typename T, typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  ( AllowToUseCopy ? ( std::is_copy_constructible<VT>::value && std::is_copy_assignable<VT>::value ) : true ) &&
				  !( std::is_same<VT, constrained_any>::value ) &&
				  !( std::is_same<VT, std::any>::value ) &&
				  Constraint<VT>::value>::type* = nullptr>
	constrained_any( T&& v )
	  : up_carrier_( make_impl_value_carrier<VT>( std::forward<T>( v ) ) )
	{
	}

	template <class T, class... Args,
	          typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  ( AllowToUseCopy ? ( std::is_copy_constructible<VT>::value && std::is_copy_assignable<VT>::value ) : true ) &&
				  std::is_constructible<VT, Args...>::value &&
				  !( std::is_same<VT, constrained_any>::value ) &&
				  !( std::is_same<VT, std::any>::value ) &&
				  Constraint<VT>::value>::type* = nullptr>
	explicit constrained_any( std::in_place_type_t<T>, Args&&... args )
	  : up_carrier_( make_impl_value_carrier<VT>( std::forward<Args>( args )... ) )
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
				  ( AllowToUseCopy ? ( std::is_copy_constructible<VT>::value && std::is_copy_assignable<VT>::value ) : true ) &&
				  std::is_constructible<VT, Args...>::value &&
				  !( std::is_same<VT, constrained_any>::value ) &&
				  !( std::is_same<VT, std::any>::value ) &&
				  Constraint<VT>::value>::type* = nullptr>
	std::decay_t<T>& emplace( Args&&... args )
	{
		auto             up_vc = make_impl_value_carrier<VT>( std::forward<Args>( args )... );
		std::decay_t<T>* p_ans = &( up_vc->value_ );

		up_carrier_ = std::move( up_vc );

		return *p_ans;
	}

	template <class T, typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  ( AllowToUseCopy ? ( std::is_copy_constructible<VT>::value && std::is_copy_assignable<VT>::value ) : true ) &&
				  !( std::is_same<VT, constrained_any>::value ) &&
				  !( std::is_same<VT, std::any>::value ) &&
				  Constraint<VT>::value>::type* = nullptr>
	constrained_any& operator=( T&& rhs )
	{
		using carrier_t = impl::value_carrier<VT, AllowToUseCopy>;

		if ( up_carrier_ != nullptr ) {
			if ( up_carrier_->get_type_info() == typeid( VT ) ) {
				carrier_t& ref_src = dynamic_cast<carrier_t&>( *( up_carrier_.get() ) );
				ref_src.value_     = std::forward<T>( rhs );
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

private:
	template <typename T, class... Args>
	static auto make_impl_value_carrier( Args&&... args ) -> std::unique_ptr<impl::value_carrier<T, AllowToUseCopy>>
	{
		return std::make_unique<impl::value_carrier<T, AllowToUseCopy>>( std::in_place_type_t<T> {}, std::forward<Args>( args )... );
	}

	std::unique_ptr<impl::value_carrier_if<AllowToUseCopy>> up_carrier_;

	template <class T, bool UAllowToUseCopy, template <class> class UConstraint>
	friend T constrained_any_cast( const constrained_any<UAllowToUseCopy, UConstraint>& operand );

	template <class T, bool UAllowToUseCopy, template <class> class UConstraint>
	friend T constrained_any_cast( constrained_any<UAllowToUseCopy, UConstraint>& operand );

	template <class T, bool UAllowToUseCopy, template <class> class UConstraint>
	friend T constrained_any_cast( constrained_any<UAllowToUseCopy, UConstraint>&& operand );

	template <class T, bool UAllowToUseCopy, template <class> class UConstraint>
	friend const T* constrained_any_cast( const constrained_any<UAllowToUseCopy, UConstraint>* operand ) noexcept;

	template <class T, bool UAllowToUseCopy, template <class> class UConstraint>
	friend T* constrained_any_cast( constrained_any<UAllowToUseCopy, UConstraint>* operand ) noexcept;
};

template <class T, bool AllowToUseCopy, template <class> class Constraint = no_constrained, class... Args>
constrained_any<AllowToUseCopy, Constraint> make_constrained_any( Args&&... args )
{
	return constrained_any<AllowToUseCopy, Constraint>( std::in_place_type<T>, std::forward<Args>( args )... );
}

template <class T, bool AllowToUseCopy, template <class> class Constraint>
T constrained_any_cast( const constrained_any<AllowToUseCopy, Constraint>& operand )
{
	using U = typename std::remove_cv_t<std::remove_reference_t<T>>;
	static_assert( std::is_constructible<T, const U&>::value, "T must be constructible" );

	if ( operand.type() != typeid( U ) ) {
		throw std::bad_any_cast();
	}

	const impl::value_carrier<U, AllowToUseCopy>* p = dynamic_cast<const impl::value_carrier<U, AllowToUseCopy>*>( operand.up_carrier_.get() );
	if ( p == nullptr ) {
		throw std::bad_any_cast();
	}

	return p->ref();
}

template <class T, bool AllowToUseCopy, template <class> class Constraint>
T constrained_any_cast( constrained_any<AllowToUseCopy, Constraint>& operand )
{
	using U = typename std::remove_cv_t<std::remove_reference_t<T>>;
	static_assert( std::is_constructible<T, U&>::value, "T must be constructible" );

	if ( operand.type() != typeid( U ) ) {
		throw std::bad_any_cast();
	}

	impl::value_carrier<U, AllowToUseCopy>* p = dynamic_cast<impl::value_carrier<U, AllowToUseCopy>*>( operand.up_carrier_.get() );
	if ( p == nullptr ) {
		throw std::bad_any_cast();
	}

	return p->ref();
}

template <class T, bool AllowToUseCopy, template <class> class Constraint>
T constrained_any_cast( constrained_any<AllowToUseCopy, Constraint>&& operand )
{
	using U = typename std::remove_cv_t<std::remove_reference_t<T>>;
	static_assert( std::is_constructible<T, U>::value, "T must be constructible" );

	if ( operand.type() != typeid( U ) ) {
		throw std::bad_any_cast();
	}

	impl::value_carrier<U, AllowToUseCopy>* p = dynamic_cast<impl::value_carrier<U, AllowToUseCopy>*>( operand.up_carrier_.get() );
	if ( p == nullptr ) {
		throw std::bad_any_cast();
	}

	return p->ref();
}

template <class T, bool AllowToUseCopy, template <class> class Constraint>
const T* constrained_any_cast( const constrained_any<AllowToUseCopy, Constraint>* operand ) noexcept
{
	static_assert( !std::is_void_v<T>, "T should not be void" );

	if ( operand == nullptr ) return nullptr;

	if ( operand->type() != typeid( T ) ) {
		return nullptr;
	}

	const impl::value_carrier<T, AllowToUseCopy>* p = dynamic_cast<const impl::value_carrier<T, AllowToUseCopy>*>( operand->up_carrier_.get() );
	if ( p == nullptr ) {
		return nullptr;
	}

	return &( p->ref() );
}

template <class T, bool AllowToUseCopy, template <class> class Constraint>
T* constrained_any_cast( constrained_any<AllowToUseCopy, Constraint>* operand ) noexcept
{
	static_assert( !std::is_void_v<T>, "T should not be void" );

	if ( operand == nullptr ) return nullptr;

	if ( operand->type() != typeid( T ) ) {
		return nullptr;
	}

	impl::value_carrier<T, AllowToUseCopy>* p = dynamic_cast<impl::value_carrier<T, AllowToUseCopy>*>( operand->up_carrier_.get() );
	if ( p == nullptr ) {
		return nullptr;
	}

	return &( p->ref() );
}

}   // namespace yth

#endif
