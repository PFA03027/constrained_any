# yan::constrained_any
std::any is very useful. But it is too free.<br>
yan::constrained_any introduce the constrain. and then std::set, std::map and so on are able to store the instance of yan::constrained_any as the key value.

yan::constrained_any has mostly same interface as std::any.<br>
On the other hand, an acceptable type is limited by the template parameter named ConstrainAndOperationArgs.

Due to ConstrainAndOperationArgs, yan::constrained_any is able to add specialized member functions by the template parameter named ConstrainAndOperationArgs like mixin.

And also, ConstrainAndOperationArgs is template parameter pack, so you can add multiple specialized member functions.<br>

#### Remark
* constrained_any requires C++17 or later.
* constrained_any is provided by header only.

# pre-defined type aliased from yan::constrained_any
## yan::weak_ordering_any
yan::weak_ordering_any is a type aliased from yan::constrained_any with the constraint of impl::special_operation_less\<T\>.

This constraint "impl::special_operation_less\<T\>" requires the type T to be weakly orderable like blow;
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
yan::unordered_key_any is a type aliased from yan::constrained_any with 2 constraints, impl::special_operation_equal_to\<T\>, impl::special_operation_hash_value\<T\>.<br>
These constraints requires below;

* T is comparable by operator==(T, T)<br> this constraint is required by impl::special_operation_equal_to\<T\>
* T is hashable by std::hash\<T\><br> this constraint is required by impl::special_operation_hash_value\<T\>

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
According to these constraints, yan::unordered_key_any is able to apply the comparison operator == by the member function named "equal_to()" and also std::hash\<yan::unordered_key_any\> by the member function named "hash_value()".

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

## yan::keyable_any
yan::keyable_any is a type aliased from yan::constrained_any with 3 constraints of below;<br>

* T is comparable by operator<(T, T)<br> this constraint is required by impl::special_operation_less\<T\>
* T is comparable by operator==(T, T)<br> this constraint is required by impl::special_operation_equal_to\<T\>
* T is hashable by std::hash\<T\><br> this constraint is required by impl::special_operation_hash_value\<T\>

These constraints are the same as yan::unordered_key_any and yan::weak_ordering_any.<br>
So, please refer to the above description of yan::unordered_key_any and yan::weak_ordering_any.<br>

According to these constraints, yan::keyable_any is able to apply the comparison operator == and \< by the member function named "equal_to()" and "less()".
And also, it is able to hash by the member function named "hash_value()".<br>
Therefore, you can compare by operator== and \< and hash by std::hash like below;
```cpp
    yan::keyable_any a = 1;
    yan::keyable_any b = 2;
    if (a == b) {
        // a is equal to b
    } else {
        // a is not equal to b
    }
    if (a < b) {
        // a is less than b
    } else {
        // a is greater than or equal to b
    }
    std::hash<yan::keyable_any> h;
    std::size_t hash = h(a);
```
And also, you can use the key of not only std::set, std::map but also std::unordered_set or std::unordered_map like below;
```cpp
    std::set<yan::keyable_any> s;
    s.insert(1);
    s.insert(2);
    s.insert(3);
    for (const auto& e : s) {
        std::cout << e << std::endl;
    }
```

## yan::no_constrained_any
yan::no_constrained_any is a type aliased from yan::constrained_any without any constraint and any special operation.<br>
This is mostly same as std::any.<br>


# yan::constrained_any is fundamental type of constrained any
yan::constrained_any is a generalized any type that has the template parameter pack named ConstrainAndOperationArgs for the multiple constraints and the specialized member functions.<br> This paramter pack ConstrainAndOperationArgs supports to composite the multiple specialized operators mixin.

```cpp
namespace yan {
    template <bool RequiresCopy,
              template <class> class... ConstrainAndOperationArgs>
    class constrained_any;
}
```
above is the definition of yan::constrained_any. And it has 2 template paramters<br>

1. RequiresCopy: if true, input type is requires copy constructible and copy assignable.<br> And, the copy constructor and copy assignment operator of constrained_any are enabled. Otherwise, they are deleted from constrained_any.
2. ConstrainAndOperationArgs: this template parameter pack adds the multiple constrains and the specialized member functions to yan::constrained_any.<br>
    The each type in ConstrainAndOperationArgs requires 2 rules;
    1. Each type in ConstrainAndOperationArgs should support one template parameter T.<br>(There is 3 meta function classes are prepared. Please see the section "Utility" for the detail.)
        * Case 1: T is the type of the concrete constrained_any type.
        * Case 2: T is the type of internal value carrier class of constrained_any. This case, T has the member function named "ref()" that returns the reference of the actual stored type of constrained_any.
        * Case 3: T is the type of input value type. This case expect to test that type is satisfied by the constraint or not. and that test result is stored in the static constexper bool member variable named "constraint_check_result".
    2. The type of ConstrainAndOperationArgs should have static constexper bool member variable named "constraint_check_result". and it should be true if the type is acceptable by the constraint.<br>

Sample implementation of ConstrainAndOperationArgs is in sample/sample_of_constrained_any.cpp.<br>
impl::special_operation_less\<T\> and impl::special_operation_equal_to\<T\> is also the reference to implement your own constraint and specialized operation.

### additional constraints
To avoid the circular template parameter dependency, implicit type conversion loop infinitely and unexpected acceptance for value type that is not satisfied by the constraint, yan::constrained_any does not accept std::any and the different specialised type of yan::constrained_any.

## Constructor
```cpp
constrained_any();                                      // (1)
constrained_any(const constrained_any&);                // (2) 
constrained_any(constrained_any&&);                     // (3)

template <typename T>
constrained_any(T&& value);                             // (4)

template <class T, class... Args>
explicit constrained_any( std::in_place_type_t<T>, Args&&... args ) // (5)
```
### abstruction of constructor
1. default constructor. status becomes to have no value.
2. copy constructor. if template parameter RequiresCopy is true, this copy constructor is enabled.
3. move constructor. move the value from the other constrained_any.
4. Type std::decay_t\<T\> that satisfies Constrain\<std::decay_t\<T\>\>::value == true does std::forward\<T\> to *this.
5. constrained_any constructs the value of type T that satisfies Constrain\<T\>::value == true with  Args... args internally.

## Destructor
```cpp
~constrained_any();
```
### abstruction of destructor
destructor. if the value is not empty, it destructs the value.

## Assignment operator
```cpp
constrained_any& operator=(const constrained_any&);    // (1)
constrained_any& operator=(constrained_any&&);         // (2)

template <typename T>
constrained_any& operator=(T&& value);                // (3)
```
### abstruction of assignment operator
1. copy assignment operator. if template parameter RequiresCopy is true, this copy assignment operator is enabled.
2. move assignment operator. move the value from the other constrained_any.
3. Store the value as the type std::decay_t\<T\> that satisfies Constrain\<std::decay_t\<T\>\>::value == true by std::forward\<T\>.

## Member function
```cpp
void swap( constrained_any& src );                   // (1)
void reset() noexcept;                               // (2)
bool has_value() const;                              // (3)

template <class T, class... Args>
std::decay_t<T>& emplace( Args&&... args );          // (4)

const std::type_info& type() const noexcept;         // (5)

template <class T>
T* get_special_operation_if() noexcept;              // (6)

template <class T>
const T* get_special_operation_if() const noexcept;  // (7)
```
### abstruction of member function
1. swap the value with src.
2. reset the value. if the value is not empty, it destructs the value. then, the status becomes to have no value, this means has_value() == false.
3. if the value is empty, it returns false. otherwise, it returns true.
4. construct the value of type T that satisfies Constrain\<T\>::value == true with Args... args directly. and then, the status becomes to have value.<br> If *this has value, it destructs the value, and then constructs the new value.
5. return the type info of the value. if the value is empty, it returns typeid(void).
6. return the pointer of the T. if the value is empty, it returns nullptr.<br> This function is used to get interface class of one of ConstrainAndOperationArgs. This interface class is able to communicate constrained_any and the stored actual value type via internal carrier class that has member type "value_type" and member function "ref()".
7. return the pointer of the const T. if the value is empty, it returns nullptr. please see (6) for the puprpose of this function.

## Non member function
```cpp
namespace yan {
    template <class T,
              bool RequiresCopy,
              template <class> class... ConstrainAndOperationArgs,
              class... Args>
    constrained_any<RequiresCopy, ConstrainAndOperationArgs...> make_constrained_any( Args&&... args );  // (1)

    template <class T,
              class SpecializedConstraintAny,
              class... Args>
    SpecializedConstraintAny make_constrained_any( Args&&... args );  // (2)

    template <class T>
    T constrained_any_cast( const constrained_any& operand );    // (3)

    template <class T>
    T constrained_any_cast( constrained_any& operand );          // (4)

    template <class T>
    T constrained_any_cast( constrained_any&& operand );         // (5)

    template <class T>
    const T* constrained_any_cast( const constrained_any* operand ) noexcept;   // (6)

    template <class T>
    T* constrained_any_cast( constrained_any* operand ) noexcept;   // (7)
}
```
### abstruction of non member function
1. make_constrained_any constructs the value of type T with Args... args internally. and then, returns the constrained_any\<RequiresCopy, ConstrainAndOperationArgs...\> that has the value.
2. make_constrained_any constructs the value of type T that satisfies Constrain of SpecializedConstraintAny with Args... args internally. and then, returns the SpecializedConstraintAny that has the value.<br> The type of SpecializedConstraintAny should be the specialized class of constrained_any.
3. Specify the type held by the constrained_any object to get a copy or reference to the value.<br> To get a copy, use constrained_any _cast\<int\>(x),<br> and to get a reference, use constrained_any _cast\<int&\>(x).<br> If you specify an incorrect type, the exception std::bad_any_cast will be thrown.
4. see (3)
5. see (3)
6. Specify the type held by the constrained_any object to get a pointer to the value.<br> If you specify an incorrect type, it returns nullptr.
7. see (6)

### Requirements
as the pre-condition, using U = remove_cv_t<remove_reference_t<T>>.

3. std::is_constructible_v\<T, const U&\> == true.
4. std::is_constructible_v\<T, U&\> == true.
5. std::is_constructible_v\<T, U\> == true.
6. std::is_void_v\<T\> == false
7. see (6)

## Utility
### yan::is_specialized_of_constrained_any
```cpp
namespace yan {
    template <typename T>
    struct is_specialized_of_constrained_any<T> {
        static constexpr bool value = if true, T is specialized type of constrained_any.;
    };
}
```
#### abstruction of yan::is_specialized_of_constrained_any
yan::is_specialized_of_constrained_any is a type trait that checks whether the type T is specialized type of constrained_any or not.<br>
If T is specialized type of constrained_any, static member variable "value" is true, otherwise  false.<br>
This helps to implement ConstrainAndOperationArgs class.

### yan::is_value_carrier_of_constrained_any
```cpp
namespace yan {
    template <typename T>
    struct is_value_carrier_of_constrained_any<T> {
        static constexpr bool value = if true, T is value carrier of constrained_any.;
    };
}
```
#### abstruction of yan::is_value_carrier_of_constrained_any
yan::is_value_carrier_of_constrained_any is a type trait that checks whether the type T is value carrier of constrained_any or not.<br>
If T is value carrier of constrained_any, static member variable "value" is true, otherwise  false.<br>
This helps to implement ConstrainAndOperationArgs class.

### yan::is_related_type_of_constrained_any
```cpp
namespace yan {
    template <typename T>
    struct is_related_type_of_constrained_any<T> {
        static constexpr bool value = is_specialized_of_constrained_any<T>::value ||
                                      is_value_carrier_of_constrained_any<T>::value;
    };
}
```
#### abstruction of yan::is_related_type_of_constrained_any
yan::is_related_type_of_constrained_any is a type trait that checks whether the type T is related type of constrained_any or not.<br>
If T is a related type of constrained_any, static member variable "value" is true, otherwise  false.<br>
This helps to implement ConstrainAndOperationArgs class.

## How to implement your own constraint any
please refer to the sample/sample_of_constrained_any.cpp.<br>
besically you need to implement the following three things;
1. implement your own constraint to make easy to implement the constraint check in (4)
2. prepare the interface class of the ConstrainAndOperation. This interface class help to communicate between constrained_any and the actual value type.
3. implement your own ConstrainAndOperation class accepting one template parameter that is used by CRTP to get actual value type and indentify it is base class of constrained_any or not.
4. implement static constexpr bool member variable named "constraint_check_result" in the class of (2).<br>
   This member variable should be true if the type is acceptable by the constraint.<br>
   And also, this member variable should be false if the type is not acceptable by the constraint. 
5. implement your own constrainted_any

## ToDo
* use concept to adapt the constraint and specialized operator
  
