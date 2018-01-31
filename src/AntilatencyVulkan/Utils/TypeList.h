#pragma once

struct NullType {};

template <typename T, typename... S>
struct TypeList
{
	using Head = T;
	using Tail = TypeList<S...>;
};

template <typename T>
struct TypeList<T>
{
	using Head = T;
	using Tail = NullType;
};

template <typename T, typename TL>
struct type_id
{
	enum { value = 1 + type_id<T, typename TL::Tail>::value };
};

template <typename T, typename... S>
struct type_id<T, TypeList<T, S...>>
{
	enum { value = 0 };
};

template <int I, typename TL>
struct type_at
{
	using type = typename type_at<I - 1, typename TL::Tail>::type;
};

template <typename Head, typename... S>
struct type_at<0, TypeList<Head, S...>>
{
	using type = Head;
};

template <typename TL>
struct length
{
        enum { value =  1 + length<typename TL::Tail>::value };
};

template <>
struct length<NullType>
{
	enum { value = 0 };
};
