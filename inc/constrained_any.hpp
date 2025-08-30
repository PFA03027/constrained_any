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

static constexpr size_t sso_buff_size = 128;

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
struct do_any_constraints_requir_copy_constructible {
	static constexpr bool value = ( ... || is_required_copy_constructible<ConstrainAndOperationArgs<impl::constrained_any_tag>>::value );
};

template <typename T, template <class> class... ConstrainAndOperationArgs>
struct is_satisfy_required_copy_constructible_constraint {
	static constexpr bool are_constrains_required_copy_constructible = do_any_constraints_requir_copy_constructible<ConstrainAndOperationArgs...>::value;

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
struct do_any_constraints_requir_move_constructible {
private:
	static constexpr bool are_constrains_required_copy_constructible = do_any_constraints_requir_copy_constructible<ConstrainAndOperationArgs...>::value;

public:
	// コピー構築を要求されている場合、ムーブ構築をサポートしていない型であってもムーブ構築をコピー構築で代用できるため、ムーブ構築の要求を無効化する。
	static constexpr bool value = are_constrains_required_copy_constructible ? false : ( ... || is_required_move_constructible<ConstrainAndOperationArgs<impl::constrained_any_tag>>::value );
};

template <typename T, template <class> class... ConstrainAndOperationArgs>
struct is_satisfy_required_move_constructible_constraint {
	static constexpr bool are_constrains_required_move_constructible = do_any_constraints_requir_move_constructible<ConstrainAndOperationArgs...>::value;

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
	using abst_if_t = value_carrier_if<true, SupportUseMove>;

	virtual std::unique_ptr<abst_if_t> mk_clone_by_copy_construction( abst_if_t**, unsigned char* ) const      = 0;
	virtual std::unique_ptr<abst_if_t> mk_clone_by_move_construction( abst_if_t**, unsigned char* )            = 0;
	virtual std::unique_ptr<abst_if_t> copy_my_value_to_other( abst_if_t&, abst_if_t**, unsigned char* ) const = 0;
	virtual std::unique_ptr<abst_if_t> move_my_value_to_other( abst_if_t&, abst_if_t**, unsigned char* )       = 0;
};

template <>
struct value_carrier_if<false, true> : public value_carrier_if_common {
	using abst_if_t = value_carrier_if<false, true>;

	virtual std::unique_ptr<abst_if_t> mk_clone_by_move_construction( abst_if_t**, unsigned char* )      = 0;
	virtual std::unique_ptr<abst_if_t> move_my_value_to_other( abst_if_t&, abst_if_t**, unsigned char* ) = 0;
};

template <>
struct value_carrier_if<false, false> : public value_carrier_if_common {
	using abst_if_t = value_carrier_if<false, false>;
};

// primary template for value_carrier
template <typename T, bool SupportUseCopy, bool SupportUseMove, template <class> class... ConstrainAndOperationArgs>
struct value_carrier;

// specialization for void
template <bool SupportUseMove, template <class> class... ConstrainAndOperationArgs>
struct value_carrier<void, true, SupportUseMove, ConstrainAndOperationArgs...> : public value_carrier_if<true, SupportUseMove> {
	using abst_if_t  = typename value_carrier_if<true, SupportUseMove>::abst_if_t;
	using value_type = void;

	static constexpr bool is_possible_sso = ( sizeof( value_carrier ) < yan::impl::sso_buff_size ) &&
	                                        std::is_nothrow_move_constructible<value_carrier>::value;

	~value_carrier()                                 = default;
	value_carrier()                                  = default;
	value_carrier( const value_carrier& )            = default;
	value_carrier( value_carrier&& )                 = default;
	value_carrier& operator=( const value_carrier& ) = default;
	value_carrier& operator=( value_carrier&& )      = default;

	template <typename... Args>
	value_carrier( std::in_place_type_t<void>, Args&&... args ) noexcept
	{
	}

	const std::type_info& get_type_info() const noexcept override
	{
		return typeid( void );
	}

	std::unique_ptr<abst_if_t> mk_clone_by_copy_construction( abst_if_t** pp_k, unsigned char* p_buff ) const override
	{
		std::unique_ptr<abst_if_t> up_ans;
		if constexpr ( is_possible_sso ) {
			*pp_k = new ( p_buff ) value_carrier( *this );
		} else {
			up_ans = std::make_unique<value_carrier>( *this );
			*pp_k  = up_ans.get();
		}
		return up_ans;
	}

	std::unique_ptr<abst_if_t> mk_clone_by_move_construction( abst_if_t** pp_k, unsigned char* p_buff ) override
	{
		std::unique_ptr<abst_if_t> up_ans;
		if constexpr ( is_possible_sso ) {
			*pp_k = new ( p_buff ) value_carrier( std::move( *this ) );
		} else {
			up_ans = std::make_unique<value_carrier>( std::move( *this ) );
			*pp_k  = up_ans.get();
		}
		return up_ans;
	}

	std::unique_ptr<abst_if_t> copy_my_value_to_other( abst_if_t& other, abst_if_t** pp_k, unsigned char* p_buff ) const override
	{
		return nullptr;
	}
	std::unique_ptr<abst_if_t> move_my_value_to_other( abst_if_t& other, abst_if_t** pp_k, unsigned char* p_buff ) override
	{
		return nullptr;
	}
};

// specialization for void
template <template <class> class... ConstrainAndOperationArgs>
struct value_carrier<void, false, true, ConstrainAndOperationArgs...> : public value_carrier_if<false, true> {
	using abst_if_t  = typename value_carrier_if<false, true>::abst_if_t;
	using value_type = void;

	static constexpr bool is_possible_sso = ( sizeof( value_carrier ) < yan::impl::sso_buff_size ) &&
	                                        std::is_nothrow_move_constructible<value_carrier>::value;

	~value_carrier()                                 = default;
	value_carrier()                                  = default;
	value_carrier( const value_carrier& )            = default;
	value_carrier( value_carrier&& )                 = default;
	value_carrier& operator=( const value_carrier& ) = default;
	value_carrier& operator=( value_carrier&& )      = default;

	template <typename... Args>
	value_carrier( std::in_place_type_t<void>, Args&&... args ) noexcept
	{
	}

	const std::type_info& get_type_info() const noexcept override
	{
		return typeid( void );
	}

	std::unique_ptr<abst_if_t> mk_clone_by_move_construction( abst_if_t** pp_k, unsigned char* p_buff ) override
	{
		std::unique_ptr<abst_if_t> up_ans;
		if constexpr ( is_possible_sso ) {
			*pp_k = new ( p_buff ) value_carrier( std::move( *this ) );
		} else {
			up_ans = std::make_unique<value_carrier>( std::move( *this ) );
			*pp_k  = up_ans.get();
		}
		return up_ans;
	}

	std::unique_ptr<abst_if_t> move_my_value_to_other( abst_if_t& other, abst_if_t** pp_k, unsigned char* p_buff ) override
	{
		return nullptr;
	}
};

// specialization for void
template <template <class> class... ConstrainAndOperationArgs>
struct value_carrier<void, false, false, ConstrainAndOperationArgs...> : public value_carrier_if<false, false> {
	using abst_if_t  = typename value_carrier_if<false, false>::abst_if_t;
	using value_type = void;

	static constexpr bool is_possible_sso = ( sizeof( value_carrier ) < yan::impl::sso_buff_size ) &&
	                                        std::is_nothrow_move_constructible<value_carrier>::value;

	~value_carrier()                                 = default;
	value_carrier()                                  = default;
	value_carrier( const value_carrier& )            = default;
	value_carrier( value_carrier&& )                 = default;
	value_carrier& operator=( const value_carrier& ) = default;
	value_carrier& operator=( value_carrier&& )      = default;

	template <typename... Args>
	value_carrier( std::in_place_type_t<void>, Args&&... args ) noexcept
	{
	}

	const std::type_info& get_type_info() const noexcept override
	{
		return typeid( void );
	}
};

template <typename T, bool SupportUseMove, template <class> class... ConstrainAndOperationArgs>
struct value_carrier<T, true, SupportUseMove, ConstrainAndOperationArgs...> : public value_carrier_if<true, SupportUseMove>, public ConstrainAndOperationArgs<value_carrier<T, true, SupportUseMove, ConstrainAndOperationArgs...>>... {
	using abst_if_t  = typename value_carrier_if<true, SupportUseMove>::abst_if_t;
	using value_type = T;

	static constexpr bool is_possible_sso = ( sizeof( value_carrier ) < yan::impl::sso_buff_size ) &&
	                                        std::is_nothrow_move_constructible<value_carrier>::value;

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

	T& ref( void ) noexcept
	{
		return value_;
	}

	const T& ref( void ) const noexcept
	{
		return value_;
	}

	const std::type_info& get_type_info() const noexcept override
	{
		return typeid( value_type );
	}

	std::unique_ptr<abst_if_t> mk_clone_by_copy_construction( abst_if_t** pp_k, unsigned char* p_buff ) const override
	{
		std::unique_ptr<abst_if_t> up_ans;
		if constexpr ( is_possible_sso ) {
			*pp_k = new ( p_buff ) value_carrier( *this );
		} else {
			up_ans = std::make_unique<value_carrier>( *this );
			*pp_k  = up_ans.get();
		}
		return up_ans;
	}

	std::unique_ptr<abst_if_t> mk_clone_by_move_construction( abst_if_t** pp_k, unsigned char* p_buff ) override
	{
		std::unique_ptr<abst_if_t> up_ans;
		if constexpr ( is_possible_sso ) {
			*pp_k = new ( p_buff ) value_carrier( std::move( *this ) );
		} else {
			up_ans = std::make_unique<value_carrier>( std::move( *this ) );
			*pp_k  = up_ans.get();
		}
		return up_ans;
	}

	std::unique_ptr<abst_if_t> copy_my_value_to_other( abst_if_t& other, abst_if_t** pp_k, unsigned char* p_buff ) const override
	{
		value_carrier& ref_other = dynamic_cast<value_carrier&>( other );
		if constexpr ( std::is_copy_assignable<value_carrier>::value ) {
			ref_other = *this;

			return nullptr;
		} else {
			( *pp_k )->~abst_if_t();   // TODO: ソース変更に対して不安に艇になりやすい、危険なコード
			return mk_clone_by_copy_construction( pp_k, p_buff );
		}
	}
	std::unique_ptr<abst_if_t> move_my_value_to_other( abst_if_t& other, abst_if_t** pp_k, unsigned char* p_buff ) override
	{
		value_carrier& ref_other = dynamic_cast<value_carrier&>( other );
		if constexpr ( std::is_copy_assignable<value_carrier>::value ) {
			ref_other = std::move( *this );

			return nullptr;
		} else {
			( *pp_k )->~abst_if_t();   // TODO: ソース変更に対して不安に艇になりやすい、危険なコード
			return mk_clone_by_move_construction( pp_k, p_buff );
		}
	}

private:
	value_type value_;
};

template <typename T, template <class> class... ConstrainAndOperationArgs>
struct value_carrier<T, false, true, ConstrainAndOperationArgs...> : public value_carrier_if<false, true>, public ConstrainAndOperationArgs<value_carrier<T, false, true, ConstrainAndOperationArgs...>>... {
	using abst_if_t  = typename value_carrier_if<false, true>::abst_if_t;
	using value_type = T;

	static constexpr bool is_possible_sso = ( sizeof( value_carrier ) < yan::impl::sso_buff_size ) &&
	                                        std::is_nothrow_move_constructible<value_carrier>::value;

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

	T& ref( void ) noexcept
	{
		return value_;
	}

	const T& ref( void ) const noexcept
	{
		return value_;
	}

	const std::type_info& get_type_info() const noexcept override
	{
		return typeid( value_type );
	}

	std::unique_ptr<abst_if_t> mk_clone_by_move_construction( abst_if_t** pp_k, unsigned char* p_buff ) override
	{
		std::unique_ptr<abst_if_t> up_ans;
		if constexpr ( is_possible_sso ) {
			*pp_k = new ( p_buff ) value_carrier( std::move( *this ) );
		} else {
			up_ans = std::make_unique<value_carrier>( std::move( *this ) );
			*pp_k  = up_ans.get();
		}
		return up_ans;
	}

	std::unique_ptr<abst_if_t> move_my_value_to_other( abst_if_t& other, abst_if_t** pp_k, unsigned char* p_buff ) override
	{
		value_carrier& ref_other = dynamic_cast<value_carrier&>( other );
		if constexpr ( std::is_copy_assignable<value_carrier>::value ) {
			ref_other = std::move( *this );

			return nullptr;
		} else {
			( *pp_k )->~abst_if_t();   // TODO: ソース変更に対して不安に艇になりやすい、危険なコード
			return mk_clone_by_move_construction( pp_k, p_buff );
		}
	}

private:
	value_type value_;
};

template <typename T, template <class> class... ConstrainAndOperationArgs>
struct value_carrier<T, false, false, ConstrainAndOperationArgs...> : public value_carrier_if<false, false>, public ConstrainAndOperationArgs<value_carrier<T, false, false, ConstrainAndOperationArgs...>>... {
	using abst_if_t  = typename value_carrier_if<false, false>::abst_if_t;
	using value_type = T;

	static constexpr bool is_possible_sso = ( sizeof( value_carrier ) < yan::impl::sso_buff_size ) &&
	                                        std::is_nothrow_move_constructible<value_carrier>::value;

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

	T& ref( void ) noexcept
	{
		return value_;
	}

	const T& ref( void ) const noexcept
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
 * member variable p_cur_carrier_ is always valid (= non nullptr).
 */
template <template <class> class... ConstrainAndOperationArgs>
class constrained_any : public ConstrainAndOperationArgs<constrained_any<ConstrainAndOperationArgs...>>... {
	static constexpr bool RequiresCopy = impl::do_any_constraints_requir_copy_constructible<ConstrainAndOperationArgs...>::value;
	static constexpr bool RequiresMove = impl::do_any_constraints_requir_move_constructible<ConstrainAndOperationArgs...>::value;

public:
	template <typename T>
	using value_carrier_t = impl::value_carrier<std::decay_t<T>, RequiresCopy, RequiresMove, ConstrainAndOperationArgs...>;

	~constrained_any()
	{
		destruct_value_carrier();
	}

	constrained_any()
	  : p_cur_carrier_( nullptr )
	  , up_carrier_( construct_value_carrier_info<void>( &p_cur_carrier_, buff_ ) )
	{
	}

	constrained_any( const constrained_any& src )
#if __cpp_concepts >= 201907L
		requires RequiresCopy
#endif
	  : p_cur_carrier_( nullptr )
	  , up_carrier_( src.p_cur_carrier_->mk_clone_by_copy_construction( &p_cur_carrier_, buff_ ) )
	{
	}

	constrained_any( constrained_any&& src )
#if __cpp_concepts >= 201907L
		requires RequiresCopy || RequiresMove
#endif
	  : p_cur_carrier_( nullptr )
	  , up_carrier_( src.p_cur_carrier_->mk_clone_by_move_construction( &p_cur_carrier_, buff_ ) )
	{
	}

	constrained_any& operator=( const constrained_any& rhs )
#if __cpp_concepts >= 201907L
		requires RequiresCopy
#endif
	{
		if ( this == &rhs ) return *this;

		if ( this->type() == rhs.type() ) {
			up_carrier_ = rhs.p_cur_carrier_->copy_my_value_to_other( *p_cur_carrier_, &p_cur_carrier_, buff_ );
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
			up_carrier_ = rhs.p_cur_carrier_->move_my_value_to_other( *p_cur_carrier_, &p_cur_carrier_, buff_ );
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
	  : p_cur_carrier_( nullptr )
	  , up_carrier_( construct_value_carrier_info<VT>( &p_cur_carrier_, buff_, std::forward<Args>( args )... ) )
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
#if __cpp_concepts >= 201907L
		requires RequiresCopy || RequiresMove
#endif
	{
		if ( this == &src ) return;

		if ( ( up_carrier_ != nullptr ) && ( src.up_carrier_ != nullptr ) ) {
			up_carrier_.swap( src.up_carrier_ );
			std::swap( p_cur_carrier_, src.p_cur_carrier_ );
		} else if ( ( up_carrier_ == nullptr ) && ( src.up_carrier_ != nullptr ) ) {
			std::unique_ptr<value_carrier_keeper_t> up_keeper = std::move( src.up_carrier_ );

			src.up_carrier_ = p_cur_carrier_->mk_clone_by_move_construction( &src.p_cur_carrier_, src.buff_ );

			destruct_value_carrier();
			up_carrier_    = std::move( up_keeper );
			p_cur_carrier_ = up_carrier_.get();
		} else if ( ( up_carrier_ != nullptr ) && ( src.up_carrier_ == nullptr ) ) {
			std::unique_ptr<value_carrier_keeper_t> up_keeper = std::move( up_carrier_ );

			up_carrier_ = src.p_cur_carrier_->mk_clone_by_move_construction( &p_cur_carrier_, buff_ );

			src.destruct_value_carrier();
			src.up_carrier_    = std::move( up_keeper );
			src.p_cur_carrier_ = src.up_carrier_.get();
		} else {   // ( up_carrier_ == nullptr ) && ( src.up_carrier_ == nullptr )
			unsigned char                           backup_buff_[impl::sso_buff_size];
			value_carrier_keeper_t*                 p_backup_cur_carrier_;
			std::unique_ptr<value_carrier_keeper_t> up_backup_keeper;

			up_backup_keeper = p_cur_carrier_->mk_clone_by_move_construction( &p_backup_cur_carrier_, backup_buff_ );
			destruct_value_carrier();

			up_carrier_ = src.p_cur_carrier_->mk_clone_by_move_construction( &p_cur_carrier_, buff_ );
			src.destruct_value_carrier();

			src.up_carrier_ = p_backup_cur_carrier_->mk_clone_by_move_construction( &src.p_cur_carrier_, src.buff_ );

			if ( up_backup_keeper == nullptr ) {
				p_backup_cur_carrier_->~value_carrier_keeper_t();
			} else {
				up_backup_keeper.reset();
			}
		}
	}

	void reset() noexcept
	{
		reconstruct_value_carrier_info<void>();
	}

	template <class T, class... Args,
	          typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  !is_related_type_of_constrained_any<typename impl::remove_cvref<T>::type>::value &&
				  impl::is_acceptable_value_type<VT, ConstrainAndOperationArgs...>::value &&
				  std::is_constructible<VT, Args...>::value>::type* = nullptr>
	std::decay_t<T>& emplace( Args&&... args )
	{
		std::decay_t<T>* p_ans = reconstruct_value_carrier_info<VT>( std::forward<Args>( args )... );

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
			carrier_t& ref_src = dynamic_cast<carrier_t&>( *( p_cur_carrier_ ) );   // TODO: should be static_cast
			ref_src.ref()      = std::forward<T>( rhs );
			return *this;
		}

		reconstruct_value_carrier_info<VT>( std::forward<T>( rhs ) );

		return *this;
	}

	bool has_value() const noexcept
	{
		return ( this->type() != typeid( void ) );
	}

	const std::type_info& type() const noexcept
	{
		return p_cur_carrier_->get_type_info();
	}

	template <typename SpecializedOperatorIF>
	SpecializedOperatorIF* get_special_operation_if() noexcept
	{
		return dynamic_cast<SpecializedOperatorIF*>( p_cur_carrier_ );
	}
	template <typename SpecializedOperatorIF>
	const SpecializedOperatorIF* get_special_operation_if() const noexcept
	{
		return dynamic_cast<const SpecializedOperatorIF*>( p_cur_carrier_ );
	}

private:
	using value_carrier_keeper_t = impl::value_carrier_if<RequiresCopy, RequiresMove>;

	template <typename T>
	static constexpr bool is_possible_sso = ( sizeof( value_carrier_t<T> ) < yan::impl::sso_buff_size ) &&
	                                        std::is_nothrow_move_constructible<value_carrier_t<T>>::value;

	void destruct_value_carrier( void )
	{
		if ( up_carrier_ == nullptr ) {
			p_cur_carrier_->~value_carrier_keeper_t();
		} else {
			up_carrier_.reset();
		}
	}

	template <typename T, class... Args, typename std::enable_if<!is_possible_sso<T>>::type* = nullptr>
	static auto construct_value_carrier_info( value_carrier_keeper_t** pp_k, unsigned char* p_buff, Args&&... args ) -> std::unique_ptr<value_carrier_t<T>>
	{
		auto up_ans = std::make_unique<value_carrier_t<T>>( std::in_place_type_t<T> {}, std::forward<Args>( args )... );
		*pp_k       = up_ans.get();
		return up_ans;
	}

	template <typename T, class... Args, typename std::enable_if<is_possible_sso<T>>::type* = nullptr>
	static auto construct_value_carrier_info( value_carrier_keeper_t** pp_k, unsigned char* p_buff, Args&&... args ) -> std::unique_ptr<value_carrier_t<T>>
	{
		*pp_k = new ( p_buff ) value_carrier_t<T>( std::in_place_type_t<T> {}, std::forward<Args>( args )... );
		return nullptr;
	}

	template <typename T, class... Args, typename std::enable_if<!is_possible_sso<T>>::type* = nullptr>
	std::decay_t<T>* reconstruct_value_carrier_info( Args&&... args )
	{
		auto up_vc = std::make_unique<value_carrier_t<T>>( std::in_place_type_t<T> {}, std::forward<Args>( args )... );
		destruct_value_carrier();
		std::decay_t<T>* p_ans = &( up_vc->ref() );
		p_cur_carrier_         = up_vc.get();
		up_carrier_            = std::move( up_vc );
		return p_ans;
	}

	template <typename T, class... Args, typename std::enable_if<is_possible_sso<T>>::type* = nullptr>
	std::decay_t<T>* reconstruct_value_carrier_info( Args&&... args )
	{
		value_carrier_t<T> tmp( std::in_place_type_t<T> {}, std::forward<Args>( args )... );
		destruct_value_carrier();
		auto             p_vc  = new ( buff_ ) value_carrier_t<T>( std::move( tmp ) );
		std::decay_t<T>* p_ans = &( p_vc->ref() );
		p_cur_carrier_         = p_vc;
		return p_ans;
	}

	template <typename T>
	auto static_cast_T_carrier() const -> const value_carrier_t<T>*
	{
		if ( this->type() != typeid( T ) ) {
			return nullptr;
		}

		return static_cast<const value_carrier_t<T>*>( p_cur_carrier_ );
	}

	template <typename T>
	auto static_cast_T_carrier() -> value_carrier_t<T>*
	{
		if ( this->type() != typeid( T ) ) {
			return nullptr;
		}

		return static_cast<value_carrier_t<T>*>( p_cur_carrier_ );
	}

	unsigned char                           buff_[impl::sso_buff_size];
	value_carrier_keeper_t*                 p_cur_carrier_;
	std::unique_ptr<value_carrier_keeper_t> up_carrier_;

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
			// In case that this is default constructed constrained_any
			// it does not have special_operation_less_if.
			// In this case, because this less operation means void < void, it is always false.
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
			// In case that this is default constructed constrained_any
			// it does not have special_operation_equal_to_if.
			// In this case, because this equal_to operation means void == void, it is always true.
			return true;
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
			// In case that this is default constructed constrained_any
			// it does not have special_operation_hash_value_if.
			// In this case, because this hash_value operation means hash_value(void), it is always 0.
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
 * On the other hand, this constrained any does not have copy constructor and copy assigner.
 */
using move_only_any = constrained_any<impl::special_operation_movable>;

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
template <typename T, typename std::enable_if<std::is_same<T, weak_ordering_any>::value>::type* = nullptr>
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
template <typename T, typename std::enable_if<std::is_same<T, unordered_key_any>::value>::type* = nullptr>
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
template <typename T, typename std::enable_if<std::is_same<T, keyable_any>::value>::type* = nullptr>
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
template <typename T, typename std::enable_if<std::is_same<T, keyable_any>::value>::type* = nullptr>
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
