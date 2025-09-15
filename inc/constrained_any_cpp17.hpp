/**
 * @file constrained_any_cpp17.hpp
 * @author Teruaki Ata (PFA03027@nifty.com)
 * @brief constrained any type
 * @version 0.1
 * @date 2025-04-26
 *
 * @copyright Copyright (c) 2025, Teruaki Ata (PFA03027@nifty.com)
 *
 */

namespace impl {

template <bool SupportUseCopy, bool SupportUseMove>
struct value_carrier_if;

template <bool SupportUseMove>
struct value_carrier_if<true, SupportUseMove> : public value_carrier_if_common {
	using abst_if_t = value_carrier_if<true, SupportUseMove>;

	virtual std::unique_ptr<abst_if_t> mk_clone_by_copy_construction( void ) const      = 0;
	virtual std::unique_ptr<abst_if_t> mk_clone_by_move_construction( void )            = 0;
	virtual std::unique_ptr<abst_if_t> copy_my_value_to_other( abst_if_t& other ) const = 0;
	virtual std::unique_ptr<abst_if_t> move_my_value_to_other( abst_if_t& other )       = 0;
};

template <>
struct value_carrier_if<false, true> : public value_carrier_if_common {
	using abst_if_t = value_carrier_if<false, true>;

	virtual std::unique_ptr<abst_if_t> mk_clone_by_move_construction( void )      = 0;
	virtual std::unique_ptr<abst_if_t> move_my_value_to_other( abst_if_t& other ) = 0;
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

	std::unique_ptr<abst_if_t> mk_clone_by_copy_construction( void ) const override
	{
		return std::make_unique<value_carrier>( *this );
	}

	std::unique_ptr<abst_if_t> mk_clone_by_move_construction( void ) override
	{
		return std::make_unique<value_carrier>( *this );
	}

	std::unique_ptr<abst_if_t> copy_my_value_to_other( abst_if_t& other ) const override
	{
		return nullptr;
	}
	std::unique_ptr<abst_if_t> move_my_value_to_other( abst_if_t& other ) override
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

	std::unique_ptr<abst_if_t> mk_clone_by_move_construction() override
	{
		return std::make_unique<value_carrier>( *this );
	}

	std::unique_ptr<abst_if_t> move_my_value_to_other( abst_if_t& other ) override
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

	std::unique_ptr<abst_if_t> mk_clone_by_copy_construction( void ) const override
	{
		return std::make_unique<value_carrier>( *this );
	}

	std::unique_ptr<abst_if_t> mk_clone_by_move_construction() override
	{
		return std::make_unique<value_carrier>( std::move( *this ) );
	}

	std::unique_ptr<abst_if_t> copy_my_value_to_other( abst_if_t& other ) const override
	{
		value_carrier& ref_other = dynamic_cast<value_carrier&>( other );
		if constexpr ( std::is_copy_assignable<value_carrier>::value ) {
			ref_other = *this;

			return nullptr;
		} else {
			return mk_clone_by_copy_construction();
		}
	}
	std::unique_ptr<abst_if_t> move_my_value_to_other( abst_if_t& other ) override
	{
		value_carrier& ref_other = dynamic_cast<value_carrier&>( other );
		if constexpr ( std::is_move_assignable<value_carrier>::value ) {
			ref_other = std::move( *this );

			return nullptr;
		} else {
			return mk_clone_by_move_construction();
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

	std::unique_ptr<abst_if_t> mk_clone_by_move_construction( void ) override
	{
		return std::make_unique<value_carrier>( std::move( *this ) );
	}

	std::unique_ptr<abst_if_t> move_my_value_to_other( abst_if_t& other ) override
	{
		value_carrier& ref_other = dynamic_cast<value_carrier&>( other );
		if constexpr ( std::is_move_assignable<value_carrier>::value ) {
			ref_other = std::move( *this );

			return nullptr;
		} else {
			return mk_clone_by_move_construction();
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
class constrained_any_impl : public ConstrainAndOperationArgs<constrained_any_impl<ConstrainAndOperationArgs...>>... {
protected:
	static constexpr bool RequiresCopy = impl::do_any_constraints_requir_copy_constructible<ConstrainAndOperationArgs...>::value;
	static constexpr bool RequiresMove = impl::do_any_constraints_requir_move_constructible<ConstrainAndOperationArgs...>::value;

public:
	~constrained_any_impl()
	{
		destruct_value_carrier();
	}

	constrained_any_impl()
	  : up_carrier_( construct_value_carrier_info<void>() )
	{
	}

	constrained_any_impl( const constrained_any_impl& src )
	  : up_carrier_( src.up_carrier_->mk_clone_by_copy_construction() )
	{
	}

	constrained_any_impl( constrained_any_impl&& src )
	  : up_carrier_( src.up_carrier_->mk_clone_by_move_construction() )
	{
	}

	constrained_any_impl& operator=( const constrained_any_impl& rhs )
	{
		if ( this == &rhs ) return *this;

		if ( this->type() == rhs.type() ) {
			auto up_copy = rhs.up_carrier_->copy_my_value_to_other( *up_carrier_ );
			if ( up_copy != nullptr ) {
				up_carrier_ = std::move( up_copy );
			}
			return *this;
		}

		constrained_any_impl( rhs ).swap( *this );

		return *this;
	}

	constrained_any_impl& operator=( constrained_any_impl&& rhs ) noexcept
	{
		if ( this == &rhs ) return *this;

		if ( this->type() == rhs.type() ) {
			auto up_move = rhs.up_carrier_->move_my_value_to_other( *up_carrier_ );
			if ( up_move != nullptr ) {
				up_carrier_ = std::move( up_move );
			}
			return *this;
		}

		constrained_any_impl( std::move( rhs ) ).swap( *this );

		return *this;
	}

	template <class T, class... Args, typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  impl::is_acceptable_value_type<VT, ConstrainAndOperationArgs...>::value &&
				  std::is_constructible<VT, Args...>::value>::type* = nullptr>
	explicit constrained_any_impl( std::in_place_type_t<T>, Args&&... args )
	  : up_carrier_( construct_value_carrier_info<VT>( std::forward<Args>( args )... ) )
	{
	}

	template <bool Enable = ( RequiresCopy || RequiresMove ), typename std::enable_if<Enable>::type* = nullptr>
	void swap( constrained_any_impl& src )
	{
		if ( this == &src ) return;
		up_carrier_.swap( src.up_carrier_ );
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

protected:
	template <typename T>
	using value_carrier_t = impl::value_carrier<std::decay_t<T>, RequiresCopy, RequiresMove, ConstrainAndOperationArgs...>;

	using value_carrier_keeper_t = impl::value_carrier_if<RequiresCopy, RequiresMove>;

	void destruct_value_carrier( void )
	{
		up_carrier_.reset();
	}

	template <typename T, class... Args>
	static auto construct_value_carrier_info( Args&&... args ) -> std::unique_ptr<value_carrier_t<T>>
	{
		auto up_ans = std::make_unique<value_carrier_t<T>>( std::in_place_type_t<T> {}, std::forward<Args>( args )... );
		return up_ans;
	}

	template <typename T, class... Args>
	auto reconstruct_value_carrier_info( Args&&... args )
	{
		auto up_vc = std::make_unique<value_carrier_t<T>>( std::in_place_type_t<T> {}, std::forward<Args>( args )... );
		destruct_value_carrier();
		if constexpr ( std::is_void<T>::value ) {
			up_carrier_ = std::move( up_vc );
		} else {
			std::decay_t<T>* p_ans = &( up_vc->ref() );
			up_carrier_            = std::move( up_vc );
			return p_ans;
		}
	}

	// for constrained_any_cast
	template <typename T>
	auto cast_T_carrier() const -> const value_carrier_t<T>*
	{
		// dynamic_castとどちらが速いかわからない。。。
#if 0
		if ( this->type() != typeid( T ) ) {
			return nullptr;
		}

		return static_cast<const value_carrier_t<T>*>( up_carrier_.get() );
#else
		return dynamic_cast<const value_carrier_t<T>*>( up_carrier_.get() );
#endif
	}

	// for constrained_any_cast
	template <typename T>
	auto cast_T_carrier() -> value_carrier_t<T>*
	{
		// dynamic_castとどちらが速いかわからない。。。
#if 0
		if ( this->type() != typeid( T ) ) {
			return nullptr;
		}

		return static_cast<value_carrier_t<T>*>( up_carrier_.get() );
#else
		return dynamic_cast<value_carrier_t<T>*>( up_carrier_.get() );
#endif
	}

	std::unique_ptr<value_carrier_keeper_t> up_carrier_;
};

template <template <class> class... ConstrainAndOperationArgs>
class constrained_any : public constrained_any_impl<ConstrainAndOperationArgs...> {
	using base_t = constrained_any_impl<ConstrainAndOperationArgs...>;

public:
	~constrained_any()                                   = default;
	constrained_any( void )                              = default;
	constrained_any( const constrained_any& )            = default;
	constrained_any( constrained_any&& )                 = default;
	constrained_any& operator=( const constrained_any& ) = default;
	constrained_any& operator=( constrained_any&& )      = default;

	template <class T, class... Args, typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  impl::is_acceptable_value_type<VT, ConstrainAndOperationArgs...>::value &&
				  std::is_constructible<VT, Args...>::value>::type* = nullptr>
	explicit constrained_any( std::in_place_type_t<T>, Args&&... args )
	  : constrained_any_impl<ConstrainAndOperationArgs...>( std::in_place_type<T>, std::forward<Args>( args )... )
	{
	}

	template <class T, typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  ( base_t::RequiresCopy || base_t::RequiresMove ) &&
				  !is_related_type_of_constrained_any<typename impl::remove_cvref<T>::type>::value &&
				  impl::is_acceptable_value_type<VT, ConstrainAndOperationArgs...>::value>::type* = nullptr>
	constrained_any( T&& v )
	  : constrained_any( std::in_place_type<std::decay_t<T>>, std::forward<T>( v ) )
	{
	}

	template <bool Enable = ( base_t::RequiresCopy || base_t::RequiresMove ), typename std::enable_if<Enable>::type* = nullptr>
	void swap( constrained_any& src )
	{
		base_t::swap( src );
	}

	bool has_value() const noexcept
	{
		return ( this->type() != typeid( void ) );
	}

	void reset() noexcept
	{
		base_t::template reconstruct_value_carrier_info<void>();
	}

	template <class T, class... Args,
	          typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  !is_related_type_of_constrained_any<typename impl::remove_cvref<T>::type>::value &&
				  impl::is_acceptable_value_type<VT, ConstrainAndOperationArgs...>::value &&
				  std::is_constructible<VT, Args...>::value>::type* = nullptr>
	decltype( auto ) emplace( Args&&... args )
	{
#if 0
		if constexpr ( std::is_void<VT>::value ) {
			reconstruct_value_carrier_info<void>();
			return;
		} else {
			std::decay_t<T>* p_ans   = base_t::template reconstruct_value_carrier_info<VT>( std::forward<Args>( args )... );
			std::decay_t<T>& ref_ans = *p_ans;
			return ref_ans;
		}
#else
		std::decay_t<T>* p_ans   = base_t::template reconstruct_value_carrier_info<VT>( std::forward<Args>( args )... );
		std::decay_t<T>& ref_ans = *p_ans;
		return ref_ans;
#endif
	}

	template <class T, typename VT = std::decay_t<T>,
	          typename std::enable_if<
				  ( base_t::RequiresCopy || base_t::RequiresMove ) &&
				  !is_related_type_of_constrained_any<typename impl::remove_cvref<T>::type>::value &&
				  impl::is_acceptable_value_type<VT, ConstrainAndOperationArgs...>::value>::type* = nullptr>
	constrained_any& operator=( T&& rhs )
	{
		if ( this->type() == typeid( VT ) ) {
			using carrier_t    = impl::value_carrier<VT, base_t::RequiresCopy, base_t::RequiresMove, ConstrainAndOperationArgs...>;
			carrier_t& ref_src = dynamic_cast<carrier_t&>( *( base_t::up_carrier_.get() ) );   // TODO: should be static_cast
			ref_src.ref()      = std::forward<T>( rhs );
			return *this;
		}

		base_t::template reconstruct_value_carrier_info<VT>( std::forward<T>( rhs ) );

		return *this;
	}

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
