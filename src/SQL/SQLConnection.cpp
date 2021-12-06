#include SIMPLEMYSQL_PCH

#include <SimpleMySQL/SQL/SQLConnection.hpp>

#include <SimpleMySQL/SQL/SQLResult.hpp>
#include <SimpleMySQL/SQL/SQLException.hpp>

namespace simple_mysql
{

///////////////////////////////////////////////////////////////////////////////////////
Connection::Connection()
	: _connection{ }, _connected{ false }
{
	mysql_init(&_connection);
}

///////////////////////////////////////////////////////////////////////////////////////
Connection::Connection(std::string_view host_, std::string_view user_, std::string_view password_, std::string_view database_)
	: _connection{ }, _connected{ false }
{
	this->connect(host_, user_, password_, database_);
}

///////////////////////////////////////////////////////////////////////////////////////
Connection::~Connection()
{
	if (this->isConnected())
	{
		this->close();
	}
}

///////////////////////////////////////////////////////////////////////////////////////
void Connection::connect(std::string_view host_, std::string_view user_, std::string_view password_, std::string_view database_)
{
	// # Assertion note:
	// You have to be disconnected from database. Fix your code.
	assert(!this->isConnected());

	if (mysql_real_connect(&_connection,
			std::string(host_).c_str(),
			std::string(user_).c_str(),
			std::string(password_).c_str(),
			std::string(database_).c_str(),
			0, nullptr, 0)
		== nullptr) // default port
	{
		throw ConnectionException(mysql_error(&_connection));
	}
	_connected = true;
	_host		= host_;
	_user		= user_;
	_password	= password_;
	_database	= database_;
}

///////////////////////////////////////////////////////////////////////////////////////
Result Connection::query(std::string_view query_) const
{
	// # Assertion note:
	// You need a database connection to query it. Fix your code.
	assert(this->isConnectedToDatabase());

	if (!query_.empty())
	{
		if (mysql_real_query(&_connection, query_.data(), query_.length()) == 1)
		{
			throw Exception(mysql_error(&_connection));
		}
		
		if (auto res = mysql_store_result(&_connection))
			return Result{ *res };
	}
	return {};
}

///////////////////////////////////////////////////////////////////////////////////////
void Connection::selectDatabase(std::string_view database_)
{
	// # Assertion note:
	// You need a connection to select database. Fix your code.
	assert(this->isConnected());

	// Use in-place string for efficient conversion
	std::array<char, 1024> db;

	// Ensure that the database name is not too long
	assert(database_.length() < db.size() && "Database name is too long (exceeds internal limit of 1024 - 1)");

	{
		auto numBytes	= std::min(database_.size(), db.size() - 1);
		auto endIt		= std::copy_n(database_.begin(), numBytes, db.begin());
		*endIt = '\0';
	}

	if ( mysql_select_db(&_connection, db.data()) == 1 )
		throw Exception(std::string("Database selection failed: ") + mysql_error(&_connection));

	_database = database_;
}

///////////////////////////////////////////////////////////////////////////////////////
void Connection::close()
{
	// # Assertion note:
	// You need to be connected to the database. Fix your code.
	assert(this->isConnected());

	mysql_close(&_connection);
	_connected = false;
}

///////////////////////////////////////////////////////////////////////////////////////
bool Connection::isConnected() const
{
	return _connected;
}

///////////////////////////////////////////////////////////////////////////////////////
bool Connection::isConnectedToDatabase() const
{
	return this->isConnected() && !_database.empty();
}

///////////////////////////////////////////////////////////////////////////////////////
MYSQL& Connection::getInternalSQLInstance() const
{
	return _connection;
}

}
