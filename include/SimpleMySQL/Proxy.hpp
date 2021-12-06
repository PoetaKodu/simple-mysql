#pragma once

#include SIMPLEMYSQL_PCH

#include <SimpleMySQL/SQL/SQLResult.hpp>

namespace simple_mysql
{

namespace details
{

template <typename T, typename U>
concept IsTransformer = requires(T t) {
	{ t(U{}) } -> std::convertible_to<U>;
};

}

template <typename TFieldType>
struct AsIsDeserializer;

#define DEFAULT_DESERIALIZER_BEGIN(Type, ResultName, ColumnIndexName, OwnerName, OutName) \
	template <> \
	struct AsIsDeserializer< Type > \
	{ \
		template <typename TFieldOwner> \
		bool operator()(Result::Row ResultName, size_t& ColumnIndexName, TFieldOwner& OwnerName, Type& OutName) const

#define DEFAULT_DESERIALIZER_END() };

#define DEFAULT_DESERIALIZER_INLINE(Type, Expr) \
	DEFAULT_DESERIALIZER_BEGIN(Type, result, columnIndex, owner, out) \
	{ \
		Expr; \
		++columnIndex; \
		return true; \
	} \
	DEFAULT_DESERIALIZER_END()

DEFAULT_DESERIALIZER_INLINE(float,				out = std::stof( std::string(result[columnIndex]) ) );
DEFAULT_DESERIALIZER_INLINE(double,				out = std::stod( std::string(result[columnIndex]) ) );
DEFAULT_DESERIALIZER_INLINE(uint16_t,			out = std::stoul( std::string(result[columnIndex]) ) );
DEFAULT_DESERIALIZER_INLINE(uint32_t,			out = std::stoul( std::string(result[columnIndex]) ) );
DEFAULT_DESERIALIZER_INLINE(uint64_t,			out = std::stoull( std::string(result[columnIndex]) ) );
DEFAULT_DESERIALIZER_INLINE(int16_t,			out = std::stol( std::string(result[columnIndex]) ) );
DEFAULT_DESERIALIZER_INLINE(int32_t,			out = std::stol( std::string(result[columnIndex]) ) );
DEFAULT_DESERIALIZER_INLINE(int64_t,			out = std::stoll( std::string(result[columnIndex]) ) );
DEFAULT_DESERIALIZER_INLINE(std::string_view,	out = result[columnIndex]);
DEFAULT_DESERIALIZER_INLINE(std::string,		out = result[columnIndex]);

#undef DEFAULT_DESERIALIZER_BEGIN
#undef DEFAULT_DESERIALIZER_END
#undef DEFAULT_DESERIALIZER_INLINE

template <size_t MaxLen>
struct AsIsDeserializer< std::array<char, MaxLen> >
{
	using OutputType = std::array<char, MaxLen>;

	template <typename TFieldOwner>
	bool operator()(Result::Row result_, size_t& columnIndex_, TFieldOwner& owner_, OutputType& out_) const
	{
		auto p = std::strncpy(out_.data(), result_[columnIndex_], MaxLen);
		return (p != result_[columnIndex_]);
	}
};

template <
		typename TFieldOwner,
		typename TFieldType,
		typename TDeserializer = AsIsDeserializer< std::remove_cvref_t<TFieldType> >
	>
struct FieldProxy
{
	using RawFieldType			= std::remove_cvref_t<TFieldType>;
	using FieldPtrType			= TFieldType TFieldOwner::*;
	using Deserializer			= TDeserializer;

	using RawReadTransformFn	= TFieldType(Result::Row, size_t&, TFieldOwner&);
	using RawWriteTransformFn	= TFieldType(Result::Row, size_t&, TFieldOwner&);
	using ReadTransformFn		= std::function<RawReadTransformFn>;
	using WriteTransformFn		= std::function<RawWriteTransformFn>;

	FieldPtrType        member;
	Deserializer const* deserializer = nullptr;

	constexpr bool read(Result::Row result_, size_t& columnIndex_, TFieldOwner& owner_) const
	{
		if constexpr (std::is_default_constructible_v<Deserializer>)
		{
			Deserializer const& d = deserializer ? *deserializer : Deserializer{};
			return d( result_, columnIndex_, owner_, owner_.*member );
		}
		else
		{
			if (deserializer)
				return (*deserializer)( result_, columnIndex_, owner_, owner_.*member );
			return false;
		}
	}
};

template <
		typename TFieldOwner,
		typename TFieldType,
		typename TDeserializer = AsIsDeserializer< std::remove_cvref_t<TFieldType> >
	>
auto makeProxy(TFieldType TFieldOwner::* field, TDeserializer const* deserializer=nullptr)
{
	return FieldProxy<TFieldOwner, TFieldType>( field, deserializer );
}

template <
		typename TFieldOwner,
		typename TFieldType,
		typename TDeserializer = AsIsDeserializer< std::remove_cvref_t<TFieldType> >
	>
auto makeProxy(TFieldType TFieldOwner::* field, TDeserializer const& deserializer)
{
	if constexpr (details::IsTransformer<TDeserializer, TFieldType>)
	{
		auto V = [deserializer](Result::Row result, size_t& columnIndex, TFieldOwner& owner, TFieldType& out)
				{
					auto d = AsIsDeserializer< std::remove_cvref_t<TFieldType> >{};
					if ( d(result, columnIndex, owner, out) )
					{
						out = deserializer(out);
						return true;
					}
					return false;
				};

		return makeProxy( field, V );
	}
	else
	{
		return FieldProxy<TFieldOwner, TFieldType, TDeserializer>( field, &deserializer );
	}
}
template <auto field>
inline auto const DefaultProxy = makeProxy(field);


namespace details
{

// https://stackoverflow.com/q/16387354/4386320
// For each tuple type:
template<int... Is>
struct seq { };

template<int N, int... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

template<int... Is>
struct gen_seq<0, Is...> : seq<Is...> { };

template<typename T, typename F, int... Is>
void for_each(T&& t, F f, seq<Is...>)
{
	auto l = { (f(std::get<Is>(t)), 0)... };
}

template<typename... Ts, typename F>
void for_each_in_tuple(std::tuple<Ts...> const& t, F f)
{
	for_each(t, f, gen_seq<sizeof...(Ts)>());
}


// Conversion ptr-to-member to `DefaultProxy<ptr-to-member>` (for convenience)
template <typename T>
struct ConvertMemberToDefaultImpl;

template <typename T>
	requires (!std::is_member_pointer_v<T>)
struct ConvertMemberToDefaultImpl<T>
{
	using Type = T;
};

template <typename T>
	requires (std::is_member_pointer_v<T>)
struct ConvertMemberToDefaultImpl<T>
{
	using Type = decltype( makeProxy( std::declval<T>() ) );
};

template <typename T>
using ConvertMemberToDefault = typename ConvertMemberToDefaultImpl<T>::Type;

constexpr inline auto wrapWithDefault(auto&& value)
{
	if constexpr( std::is_member_pointer_v< decltype(value) > )
		return DefaultProxy<value>;
	else
		return value;
}


}

struct FieldGroupReadResult
{
	bool	success;
	size_t	numRead;
	size_t	columnIndex;

	operator bool() const { return success; }
};

// Group of fields
template <typename... Ts>
struct FieldGroup
{
	enum RunPolicy
	{
		IgnoreFailures  = 0,
		StopOnFailure   = 1,
	};

	template <typename... Us>
	FieldGroup(Us &&... us_)
		:
		proxies( std::make_tuple( details::wrapWithDefault( std::forward<Us>(us_) )... ) )
	{
	}

	std::tuple<Ts...> proxies;

	template <typename U>
	FieldGroupReadResult read(Result::Row values_, U& object_, size_t baseIndex_ = 0, RunPolicy policy_ = IgnoreFailures) const
	{
		FieldGroupReadResult result;

		result.numRead		= 0;
		result.columnIndex	= baseIndex_;
		result.success		= true;

		bool stopped = false;
		details::for_each_in_tuple(proxies,
			[&] (auto const& t)
			{
				if (!result.success) return;

				bool r = t.read(values_, result.columnIndex, object_);
				if (!r) {
					if (policy_ == StopOnFailure)
						result.success = false;
					return;
				}

				++result.numRead;
			});
		
		return result;
	}

	constexpr size_t size() const
	{
		return std::tuple_size_v< std::tuple<Ts...> >;
	}
};

// A necessary deduction guide:
template <typename... Ts>
FieldGroup(Ts&&...) -> FieldGroup< std::remove_cvref_t< details::ConvertMemberToDefault<Ts> >...>;

}