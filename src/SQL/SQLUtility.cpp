#include SIMPLEMYSQL_PCH

#include <SimpleMySQL/SQL/SQLUtility.hpp>

#include <SimpleMySQL/SQL/SQLConnection.hpp>
#include <SimpleMySQL/SQL/SQLResult.hpp>

#include <charconv>


namespace simple_mysql::ut
{

//////////////////////////////////////////////////////////////////
uint64_t getUnixTimestamp(Connection const& conn_)
{
	assert(conn_.isConnected());

	Result res = conn_.query( "SELECT UNIX_TIMESTAMP()" );
	if (!res.jumpToNextRow())
	{
		throw Exception("SQL server returned unexpected value when asked for unix timestamp.");
	}

	uint64_t value;

	std::string_view resultValue = res.getCurrentRow()[0];
	auto [ptr, ec] = std::from_chars(resultValue.data(), resultValue.data() + resultValue.size(), value);

	if (ec == std::errc{})
		return value;

	return 0;
}

//////////////////////////////////////////////////////////////////
size_t escapeString(Connection const& conn_, char* output_, size_t maxLen_, std::string_view input_)
{
	if (input_.empty())
		return 0;

	return mysql_real_escape_string(&conn_.getInternalSQLInstance(), output_, input_.data(), maxLen_);
}

//////////////////////////////////////////////////////////////////
std::string escapeString(Connection const& conn_, std::string_view input_)
{
	if (input_.empty())
		return {};

	std::string result;
	result.resize(input_.length()*2 + 1);

	auto usedLength		= escapeString(conn_, result.data(), result.size() - 1, input_);
	result[usedLength]	= '\0';	

	return result;
}

}
