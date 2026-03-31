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
and according to this constraint, yan::weak_ordering_any is able to apply the comparison operator \< by the member function named "less".
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
    s.insert(std::string("2"));
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
    s.insert(std::string("2"));
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
    s.insert(std::string("2"));
    s.insert(3);
    for (const auto& e : s) {
        std::cout << e << std::endl;
    }
```

## yan::copyable_any
yan::copyable_any is a type aliased from yan::constrained_any without any constraint and any special operation.<br>
This is mostly same as std::any.<br>

## yan::move_only_any
yan::move_only_any is a type aliased from yan::constrained_any that requires move constructible.
Therefore, yan::move_only_any support move constructor but not support copy constructor.

# How to Hold Types with Polymorphism
yan::constrained_any allows access to the value only when the type specified in yan::constrained_any_cast (including std::any_cast for std::any) exactly matches the type being held. Normally, since type information is determined at the design stage, this is sufficient.
However, this means that when you want to hide implementation classes derived from an I/F class, etc., to achieve polymorphism, you cannot access the I/F class. Also, it cannot be applied to designs that perform dependency injection using the I/F class.
When applying such objects that hide implementation classes using the I/F class to yan::constrained_any, you should hold a smart pointer to the I/F class in yan::constrained_any (*). This way, you can hold the value with the type information of the I/F class.

```cpp
class PolymorphicTestBase {
public:
	virtual ~PolymorphicTestBase()   = default;
	virtual std::string print() const = 0;
};

class Derived : public PolymorphicTestBase {
public:
	void print() const override
	{
		std::cout << "Derived::print()" << std::endl;
	}
};

void foo( void )
{
    // 実装クラスへのポインタではなく、I/Fクラスへの共有ポインタを保持する。
	auto sut = yan::make_constrained_any<yan::copyable_any, std::shared_ptr<PolymorphicTestBase>>( std::make_shared<Derived>() );

    if( sut.type() != typeid( std::shared_ptr<PolymorphicTestBase> ) ) {
        return; // keep type is not expected
    }

    // I/Fクラスへの共有ポインタを取り出す
    std::shared_ptr<PolymorphicTestBase> sp_v = yan::constrained_any_cast<std::shared_ptr<PolymorphicTestBase>>( sut );

    // 取り出した共有ポインタ経由で仮想関数を呼び出す。
    sp_v->print();  // "Derived::print()"と出力される
}
```
(*) If the object's lifetime is guaranteed (e.g., global variables), raw pointers can also be held.

### Why yan::constrained_any_cast<T>() does not support polymorphism
If yan::constrained_any_cast<T>() supported polymorphism, it would be easy to implement code that causes slicing by copying values with the I/F class type.

Furthermore, since the true type information of the implementation class is hidden, it is impossible to extract the held value as an object while maintaining polymorphism. As a result, in addition to the yan::constrained_any variable for holding the object, you need to prepare a pointer or reference to access the I/F class. (This also means that additional lifetime management of variables is required because value holding and access are separated.)

As a result, if you need to prepare additional variables to hold pointer information, it is sufficient for yan::constrained_any to hold pointer information as well.

Moreover, if holding pointers, it is easy to implement extraction of objects with I/F class type information without causing slicing.

In this way, not supporting polymorphism in yan::constrained_any_cast<T>() leads to type-safe implementation. Furthermore, by holding pointer information with smart pointers, memory-safe implementation can also be achieved.

This is the reason why it is better for yan::constrained_any_cast<T>() not to support polymorphism.

# How to Hold Types with Polymorphism
yan::constrained_anyは、yan::constrained_any_cast(std::any用のstd::any_castを含む)で指定した型と保持してる型と正確に一致する場合に、値へのアクセスが可能になる仕様です。通常、型情報が設計段階で決定されているので、十分な仕様です。
一方で、I/Fクラスから派生した実装クラスを隠蔽したい場合など、多態性を実現するためにI/Fクラスへのアクセスができないことを意味します。また、I/Fクラスを用いた依存性注入を行うような設計にも適用できません。
このようなI/Fクラスを用いた実装クラスを隠蔽したオブジェクトをyan::constrained_anyに適用する場合は、I/Fクラスへのスマートポインタをyan::constrained_anyに保持させるようにします(*)。こうすることで、I/Fクラスの型情報で値を保持できるようになります。
(*)グローバル変数など、オブジェクトの生存が保証されているならば、生ポインタで保持することも可能です。

### yan::constrained_any_cast\<T\>()が多態性をサポートしない理由について
yan::constrained_any_cast\<T\>()が多態性をサポートを行った場合、I/Fクラスの型で値をコピーしてしまうようなスライシングを発生させる実装が容易に可能になります。
また、実装クラスの真の型情報が隠蔽されている以上、保持している値を多態性を保ったままの状態でオブジェクトとしてを取り出すことできません。その結果、オブジェクトを保持するためのyan::constrained_any型の変数に加えて、I/Fクラスへアクセスするためのポインタ、あるいは参照を用意する必要があります。（値の保持とアクセスが分離してしまうため、変数の寿命管理が追加で必要となることも意味します。）
結果として、ポインタ情報を保持する変数を追加で用意するのであれば、yan::constrained_anyに保持する情報もポインタで十分です。
そして、ポインタを保持するのであれば、スライシングを発生させない構造のまま、I/Fクラスの型情報でオブジェクトを取り出す実装が容易に実現できます。
このように、yan::constrained_any_cast\<T\>()が多態性をサポートをしない方が、型安全な実装を導けます。そのうえで、スマートポインタでポインタ情報を保持すれば、メモリ安全な実装も実現します。
これが、yan::constrained_any_cast\<T\>()が多態性をサポートをしない方がよい理由です。


# yan::constrained_any is fundamental type of constrained any
yan::constrained_any is a generalized any type that has the template parameter pack named ConstrainAndOperationArgs for the multiple constraints and the specialized member functions.<br> This paramter pack ConstrainAndOperationArgs supports to composite the multiple specialized operators mixin.

```cpp
namespace yan {
    template <template <class> class... ConstrainAndOperationArgs>
    class constrained_any;
}
```
above is the definition of yan::constrained_any. And it has any template paramters.

## Template parameter pack: ConstrainAndOperationArgs
This template parameter pack adds the multiple constrains and the specialized member functions to yan::constrained_any.<br>

1. The each constraint type in ConstrainAndOperationArgs should support one template parameter T.
    * Case 1: T is the type of the concrete constrained_any type.
    * Case 2: T is the type of internal value carrier class of constrained_any. This case, T has the member function named "ref()" that returns the reference of the actual stored type of constrained_any.
    * Case 3: T is the type of input value type. This case expect to test that type is satisfied by the constraint or not. and that test result is stored in the static constexper bool member variable named "constraint_check_result".

   There is 3 meta function classes are prepared to identify above 3 cases. Please see the section "Utility" for the detail.

2. The each constraint in ConstrainAndOperationArgs is refered below 3 non-static constexper bool member variable;
   * require_copy_constructible
   * require_move_constructible
   * constraint_check_result

   If a constraint does not have above, yan::constrained_any just does not consider to make a constrainted any.
   1. require_copy_constructible<br>
   If a constraint type requires copy constructible to an input type, please define and set value true.<br>
   If any constraints in ConstrainAndOperationArgs define and set true, copy/move constructor of constrained_any are available.
   2. require_move_constructible<br>
   If a constraint type requires move constructible to an input type, please define and set value true.<br>
   If any constraints in ConstrainAndOperationArgs define and set true, move constructor of constrained_any are available.
   3. constraint_check_result<br>
   If a constraint type requires a constraint, please define and set value true or false according to an input type.<bt>
        * true: an input type satisfies a constraint.
        * false: an input type does not satisfy a constraint.

      If all of constraints defined constraint_check_result in ConstrainAndOperationArgs are true, an input type is acceptable type as an input type of constrained_any.

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
    template <class SpecializedConstraintAny,
              class T,
              class... Args>
    SpecializedConstraintAny make_constrained_any( Args&&... args );  // (1)

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
### abstruction of non member function
1. `make_constrained_any` constructs the value of type `T` that satisfies Constrain of `SpecializedConstraintAny` with `Args...` args internally. and then, returns the `SpecializedConstraintAny` that has the value.<br> The type of `SpecializedConstraintAny` should be the specialized class of `constrained_any`.
2. Specify the type held by the `constrained_any` object to get a copy or reference to the value.<br> To get a copy, use `constrained_any _cast<T>(x)`,<br> and to get a reference, `use constrained_any _cast<int&>(x)`.<br> If you specify an incorrect type, the exception `std::bad_any_cast` will be thrown.
3. see (2)
4. see (2)
5. Specify the type held by the constrained_any object to get a pointer to the value.<br> If you specify an incorrect type, it returns `nullptr`.
6. see (5)

### Requirements
as the pre-condition, using U = remove_cv_t\<remove_reference_t\<T\>\>.

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
1. Implement your own constraint to make easy to implement the constraint check in (4)
2. Prepare the interface class of the ConstrainAndOperation. This interface class help to communicate between constrained_any and the actual value type.
3. Implement your own ConstrainAndOperation class accepting one template parameter that is used by CRTP to get actual value type and indentify it is base class of constrained_any or not.
4. Optionally, implement 3 static constexpr bool member variable named "require_copy_constructible"/"require_move_constructible"/"constraint_check_result" in the class of above (3).<br>
   Especially, "constraint_check_result" member variable should be true if the type is acceptable by the constraint. Otherwise, set false.
5. implement your own constrainted_any

## ToDo
* use concept to adapt the constraint and specialized operator
  
## Remark
move constructor/move assigner spec of std::any in C++ library does not specify post condition of source of move like has_value() is true or not after move.<br>

Current constrained_any implementation is;<br>
Even if constrained_any is after move constructor/assigner, it is has_value() == true and value it self is applied move constructor/assigner if it has.<br>
But, it leads complex implementation.

On the other hand, if constrained_any is has_value() == false after move constructor/assigner, constrained_any implementation is simpler than above meybe.

From above point of view, there is a trade-off, and I don't know which is better implementation now.
 