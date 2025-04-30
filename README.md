# yan::constrained_any
std::any is very useful. But it is too free.<br>
yan::constrained_any introduce the constrain. and then std::set, std::map and so on are able to store the instance of yan::constrained_any as the key value.

yan::constrained_any has mostly same interface as std::any.<br>
On the other hand, an acceptable type is limited by the template parameter named Constraint.

Due to Constraint, yan::constrained_any is able to add specialized member functions by the template parameter named SpecializedOperator like mixin.

constrained_any requires C++17 or later.

# pre-defined type aliased from yan::constrained_any
## yan::weak_ordering_any
yan::weak_ordering_any is a type aliased from yan::constrained_any with the constraint of impl::is_weak_orderable\<T\>.

This constraint "impl::is_weak_orderable\<T\>" requires the type T to be weakly orderable like blow;
```cpp
    T a;
    T b;
    if (a < b) {
        // a is less than b
    } else {
        // a is greater than or equal to b
    }
```
and according to this constraint, yan::weak_ordering_any is alble to apply the comparison operator \< by the member function named "less".
Therefore, you can compare by less like below;
```cpp
    yan::weak_ordering_any a = 1;
    yan::weak_ordering_any b = 2;
    if (a < b) {
        // a is less than b
    } else {
        // a is greater than or equal to b
    }
```
And also, you can use yan::weak_ordering_any as the key of std::set or std::map like below;
```cpp
    std::set<yan::weak_ordering_any> s;
    s.insert(1);
    s.insert(2);
    s.insert(3);
    for (const auto& e : s) {
        std::cout << e << std::endl;
    }
```

## yan::unordered_key_any
yan::unordered_key_any is a type aliased from yan::constrained_any with the constraint of impl::is_acceptable_as_unordered_key\<T\>.
This constraint "impl::is_acceptable_as_unordered_key\<T\>" requires 2 constrains
* T is comparable by operator==(T, T)
* T is hashable by std::hash\<T\>

The first constraint requires the type T to be comparable like below;
```cpp
    T a;
    T b;
    if (a == b) {
        // a is equal to b
    } else {
        // a is not equal to b
    }
```
The second constraint requires the type T to be hashable like below;
```cpp
    T a;
    std::hash<T> h;
    std::size_t hash = h(a);
```
According to these constraints, yan::unordered_key_any is able to apply the comparison operator == by the member function named "equal_to()" and also hash by the member function named "hash_value()".

Therefore, you can compare by operator== and hash by std::hash like below;
```cpp
    yan::unordered_key_any a = 1;
    yan::unordered_key_any b = 2;
    if (a == b) {
        // a is equal to b
    } else {
        // a is not equal to b
    }
    std::hash<yan::unordered_key_any> h;
    std::size_t hash = h(a);
```
And also, you can use the key of std::unordered_set or std::unordered_map like below;
```cpp
    std::unordered_set<yan::unordered_key_any> s;
    s.insert(1);
    s.insert(2);
    s.insert(3);
    for (const auto& e : s) {
        std::cout << e << std::endl;
    }
```

## yan::constrained_any
yan::constrained_any is a generalized any type that has the constraint of the template parameter named Constraint. And also, it has the template parameter named SpecializedOperator for the specialized member functions.


```cpp
namespace yan {
    template <bool RequiresCopy = true, template <class> class Constraint = no_constrained, template <class> class SpecializedOperator = no_specialoperation>
    class constrained_any;
}
```
above is the definition of yan::constrained_any. And it has three template paramters<br>

1. RequiresCopy: if true, input type is requires copy constructible and copy assignable.<br> And, the copy constructor and copy assignment operator of constrained_any are enabled. Otherwise, they are deleted from constrained_any.
2. Constraint: the constraint of the input type. Constraint\<T\>::value should be available.<br> If Constraint\<T\>::value == true, T is acceptable type, otherwise constrained_any does not accept.<br> The default is yan::no_constrained, which means no constraint like std::any.
3. SpecializedOperator: this type adds the specialized member functions to yan::constrained_any.<br> SpecializedOperator should be the derived class of yan::special_operation_if.<br> The default is yan::no_specialoperation, which means no specialized member functions like std::any.

### additional constraints
To avoid the circular template parameter dependency and unexpected acceptance for value type that is not satisfied by the constraint, yan::constrained_any does not accept std::any and the specialised type of yan::constrained_any.

### Constructor
```cpp
constrained_any();                                      // (1)
constrained_any(const constrained_any&);                // (2) 
constrained_any(constrained_any&&);                     // (3)

template <typename T>
constrained_any(T&& value);                             // (4)

template <class T, class... Args>
explicit constrained_any( std::in_place_type_t<T>, Args&&... args ) // (5)
```
#### abstruction of constructor
1. default constructor. status becomes to have no value.
2. copy constructor. if template parameter RequiresCopy is true, this copy constructor is enabled.
3. move constructor. move the value from the other constrained_any.
4. Type std::decay_t\<T\> that satisfies Constrain\<std::decay_t\<T\>\>::value == true does std::forward\<T\> to *this.
5. constrained_any constructs the value of type T that satisfies Constrain\<T\>::value == true with  Args... args internally.

### Destructor
```cpp
~constrained_any();
```
#### abstruction of destructor
destructor. if the value is not empty, it destructs the value.

### Assignment operator
```cpp
constrained_any& operator=(const constrained_any&);    // (1)
constrained_any& operator=(constrained_any&&);         // (2)

template <typename T>
constrained_any& operator=(T&& value);                // (3)
```
#### abstruction of assignment operator
1. copy assignment operator. if template parameter RequiresCopy is true, this copy assignment operator is enabled.
2. move assignment operator. move the value from the other constrained_any.
3. Store the value as the type std::decay_t\<T\> that satisfies Constrain\<std::decay_t\<T\>\>::value == true by std::forward\<T\>.

### Member function
```cpp
void swap( constrained_any& src );                                      // (1)
void reset() noexcept;                                                  // (2)
bool has_value() const;                                                 // (3)

template <class T, class... Args>
std::decay_t<T>& emplace( Args&&... args );                             // (4)

const std::type_info& type() const noexcept;                            // (5)
special_operation_if* get_special_operation_if() noexcept;              // (6)
const special_operation_if* get_special_operation_if() const noexcept;  // (7)
```
#### abstruction of member function
1. swap the value with src.
2. reset the value. if the value is not empty, it destructs the value. then, the status becomes to have no value, this means has_value() == false.
3. if the value is empty, it returns false. otherwise, it returns true.
4. construct the value of type T that satisfies Constrain\<T\>::value == true with Args... args directly. and then, the status becomes to have value.<br> If *this has value, it destructs the value, and then constructs the new value.
5. return the type info of the value. if the value is empty, it returns typeid(void).
6. return the pointer of the special_operation_if. if the value is empty, it returns nullptr.
7. return the pointer of the const special_operation_if. if the value is empty, it returns nullptr.

### Non member function
```cpp
namespace yan {
    template <class T, bool RequiresCopy, template <class> class Constraint = no_constrained, template <class> class SpecializedOperator = no_specialoperation, class... Args>
    constrained_any<RequiresCopy, Constraint, SpecializedOperator> make_constrained_any( Args&&... args );  // (1)

    template <class T>
    T constrained_any_cast( const constrained_any& operand );    // (2)

    template <class T>
    T constrained_any_cast( constrained_any& operand );          // (3)

    template <class T>
    T constrained_any_cast( constrained_any&& operand );         // (4)

    template <class T>
    const T* constrained_any_cast( const constrained_any* operand ) noexcept;   // (5)

    template <class T>
    T* constrained_any_cast( constrained_any* operand ) noexcept;   // (6)
}
```
#### abstruction of non member function
1. make_constrained_any constructs the value of type T that satisfies Constrain\<T\>::value == true with Args... args internally. and then, returns the constrained_any\<RequiresCopy, Constraint, SpecializedOperator\> that has the value.
2. Specify the type held by the constrained_any object to get a copy or reference to the value.<br> To get a copy, use constrained_any _cast\<int\>(x),<br> and to get a reference, use constrained_any _cast\<int&\>(x).<br> If you specify an incorrect type, the exception std::bad_any_cast will be thrown.
3. see (2)
4. see (2)
5. Specify the type held by the constrained_any object to get a pointer to the value.<br> If you specify an incorrect type, it returns nullptr.
6. see (5)

#### Requirements
as the pre-condition, using U = remove_cv_t<remove_reference_t<T>>.

2. std::is_constructible_v\<T, const U&\> == true.
3. std::is_constructible_v\<T, U&\> == true.
4. std::is_constructible_v\<T, U\> == true.
5. std::is_void_v\<T\> == false
6. see (5)

## Utility
### yan::special_operation_if
```cpp
namespace yan {
    struct special_operation_if {
        virtual ~special_operation_if() = default;

        virtual void specialized_operation_callback( void* )       = 0;
        virtual void specialized_operation_callback( void* ) const = 0;
    };
}
```
#### abstruction of yan::special_operation_if
yan::special_operation_if is an interface for template parameter SpecializedOperator of yan::constrained_any.<br>
This interface has two pure virtual functions named specialized_operation_callback. The first one is for non-const and the second one is for const.<br>
The derived class of this interface should implement these two functions.

sample of yan::special_operation_if is in sample/sample_of_constrained_any.cpp.

### yan::is_callable_ref
```cpp
namespace yan {
    template <typename T>
    struct is_callable_ref;
}
```

#### abstruction of utility
yan::is_callable_ref is a type trait that checks whether the type T is callable ref() or not. If T is callable ref(), it returns true, otherwise it returns false.<br>
The purpose of this type trait is to check derived class of yan::special_operation_if is callable ref() or not.<br>
If true, the derived class is value carrier of yan::constrained_any and it is possible to get actual type of the value.<br>
If false, the derived class is not value carrier of yan::constrained_any and this means T is concrete type of yan::constrained_any.

## How to implement your own constraint any
please refer to the sample/sample_of_constrained_any.cpp.<br>
besically you need to implement the following three things;
1. implement your own constraint
2. implement your own specialized operator<br> the derived class of yan::special_operation_if accepts one template parameter. and it is used by CRTP to get actual value type and indentify it is base class of constrained_any or not. 
3. implement your own constrainted_any

## ToDo
* use concept to adapt the constraint and specialized operator
* To make it easier to realize composition of constraints.
  
