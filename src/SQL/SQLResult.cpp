#include SIMPLEMYSQL_PCH


#include <SimpleMySQL/SQL/SQLResult.hpp>

namespace simple_mysql
{

////////////////////////////////////////////////////////////////////////////
Result::Result(Handle & result_)
	: _result{ &result_ }, _row{}
{
}

////////////////////////////////////////////////////////////////////////////
Result::~Result()
{
	if (_result)
		mysql_free_result(_result);
}

////////////////////////////////////////////////////////////////////////////
Result::Handle & Result::getResult() const
{
	return *_result;
}

////////////////////////////////////////////////////////////////////////////
Result::Row Result::getCurrentRow() const
{
	return _row;
}

////////////////////////////////////////////////////////////////////////////
uint64_t Result::rowCount() const
{
	return mysql_num_rows(_result);
}

////////////////////////////////////////////////////////////////////////////
bool Result::jumpToNextRow()
{
	return (_row = mysql_fetch_row(_result)) != nullptr;
}

}
