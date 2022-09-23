#ifndef NODEC__TYPE_TRAITS_HPP_
#define NODEC__TYPE_TRAITS_HPP_

#include <type_traits>

namespace nodec {

/**
 *
 * Waiting for C++17
 */
template<class... _Types>
using void_t = void;

/*
    NOTE:
        * https://github.com/skypjack/entt/blob/master/src/entt/core/type_traits.hpp
*/

/**
 * @brief A class to use to push around lists of types, nothing more.
 * @tparam Types Types provided by the type list.
 */
template<typename... Types>
struct type_list {
    //! @brief Type list type.
    using type = type_list;

    //! @brief Compile-time number of elements in the type list.
    static constexpr auto size = sizeof...(Types);
};

//! @brief Primay template isn't define on purpose.
template<std::size_t, typename>
struct type_list_element;

/**
 * @brief Privides compile-time indexed access to the types of a type list.
 * @tparam Index Index of the type to return.
 * @tparam First First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<std::size_t Index, typename First, typename... Other>
struct type_list_element<Index, type_list<First, Other...>>
    : type_list_element<Index - 1u, type_list<Other...>> {};

/**
 * @brief Provides compile-time indexed access to the types of a type list.
 * @tparam First First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<typename First, typename... Other>
struct type_list_element<0u, type_list<First, Other...>> {
    /*! @brief Searched type. */
    using type = First;
};

/**
 * @brief Helper type.
 * @tparam Index Index of the type to return.
 * @tparam List Type list to search into.
 */
template<std::size_t Index, typename List>
using type_list_element_t = typename type_list_element<Index, List>::type;

/*! @brief Primary template isn't defined on purpose. */
template<typename, typename>
struct type_list_index;

/**
 * @brief Provides compile-time type access to the types of a type list.
 * @tparam Type Type to look for and for which to return the index.
 * @tparam First First type provided by the type list.
 * @tparam Other Other types provided by the type list.
 */
template<typename Type, typename First, typename... Other>
struct type_list_index<Type, type_list<First, Other...>> {
    /**
     * @brief Unsigned integer type.
     * @note applyed stl like naming rule.
     */
    using value_type = std::size_t;

    static constexpr value_type value = 1u + type_list_index<Type, type_list<Other...>>::value;
};

/**
 * @brief Provides compile-time type access to the types of a type list.
 * @tparam Type Type to look for and for which to return the index.
 * @tparam Other Other types provided by the type list.
 */
template<typename Type, typename... Other>
struct type_list_index<Type, type_list<Type, Other...>> {
    static_assert(type_list_index<Type, type_list<Other...>>::value == sizeof...(Other), "Non-unique type");
    /*! @brief Unsigned integer type. */
    using value_type = std::size_t;
    /*! @brief Compile-time position of the given type in the sublist. */
    static constexpr value_type value = 0u;
};

/**
 * @brief Provides compile-time type access to the types of a type list.
 * @tparam Type Type to look for and for which to return the index.
 */
template<typename Type>
struct type_list_index<Type, type_list<>> {
    /*! @brief Unsigned integer type. */
    using value_type = std::size_t;
    /*! @brief Compile-time position of the given type in the sublist. */
    static constexpr value_type value = 0u;
};

/**
 * @brief Helper variable template.
 * @tparam List Type list.
 * @tparam Type Type to look for and for which to return the index.
 */
template<typename Type, typename List>
constexpr std::size_t type_list_index_v = type_list_index<Type, List>::value;

/**
 * @brief Transcribes the constness of a type to another type.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
struct constness_as {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::remove_const_t<To>;
};

/*! @copydoc constness_as */
template<typename To, typename From>
struct constness_as<To, const From> {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = const To;
};

/**
 * @brief Alias template to facilitate the transcription of the constness.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template<typename To, typename From>
using constness_as_t = typename constness_as<To, From>::type;

/*
    Note:
        * <https://www.reddit.com/r/cpp/comments/fblqwd/is_printable_check_if_a_value_is_printable_at/>

*/

template<class OSTREAM, class T, class = void>
struct can_stream_out : std::false_type {};

template<class OSTREAM, class T>
struct can_stream_out<OSTREAM, T, void_t<decltype(std::declval<OSTREAM &>() << std::declval<const T &>())>> : std::true_type {
};

// std::conjunction from C++17
template<class...>
struct conjunction : std::true_type {};

template<class B>
struct conjunction<B> : B {};

template<class B, class... Bs>
struct conjunction<B, Bs...>
    : std::conditional<bool(B::value), conjunction<Bs...>, B>::type {};

} // namespace nodec

#endif