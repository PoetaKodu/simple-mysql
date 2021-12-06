#pragma once

#include SIMPLEMYSQL_PCH

#include <SimpleMySQL/Proxy.hpp>

namespace simple_mysql
{
class Connection;

namespace ut
{

/// <summary>
/// Returns Unix timestamp obtained using SQL connection.
/// </summary>
uint64_t getUnixTimestamp(Connection const& conn_);


/// <summary>
/// Converts forbidden SQL characters to escape codes.
/// </summary>
size_t escapeString(Connection const& conn_, char* output_, size_t maxLen_, std::string_view input_);

/// <summary>
/// Converts forbidden SQL characters to escape codes.
/// </summary>
std::string escapeString(Connection const& conn_, std::string_view input_);

/// <summary>
/// Converts forbidden SQL characters to escape codes.
/// </summary>
template <size_t MaxLen = 1024>
auto escapeString(Connection const& conn_, std::string_view input_) -> std::array<char, MaxLen>
{
	std::array<char, MaxLen> result;
	escapeString(conn_, result.data(), MaxLen, input_);
	return result;
}

}

}