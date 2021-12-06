#pragma once

#include SIMPLEMYSQL_PCH

#include <SimpleMySQL/SQL/SQLException.hpp>

namespace simple_mysql
{

class Result;
class Query;

struct ConnectionException : Exception
{
	using Exception::Exception;
};

class Connection
{
public:	

	/// <summary>
	/// Initializes a new instance of the <see cref="Connection"/> class.
	/// </summary>
	Connection();
	
	/// <summary>
	/// Initializes a new instance of the <see cref="Connection"/> class. Connects to the MySQL database.
	/// </summary>
	/// <param name="host_">The host.</param>
	/// <param name="user_">The user.</param>
	/// <param name="password_">The password.</param>
	/// <param name="database_">The database.</param>
	Connection(std::string_view host_, std::string_view user_, std::string_view password_, std::string_view database_);

	/// <summary>
	/// Finalizes an instance of the <see cref="Connection"/> class.
	/// </summary>
	~Connection();

	/// <summary>
	/// Connects to the MySQL database.
	/// </summary>
	/// <param name="host_">The host.</param>
	/// <param name="user_">The user.</param>
	/// <param name="password_">The password.</param>
	/// <param name="database_">The database.</param>
	/// <exception>ConnectionException</exception>
	void connect(std::string_view host_, std::string_view user_, std::string_view password_, std::string_view database_);

	/// <summary>
	/// Runs the specified query.
	/// </summary>
	/// <param name="query_">The query.</param>
	/// <returns>
	///		Query result.
	/// </returns>
	Result query(std::string_view query_) const;
	
	/// <summary>
	/// Selects the database.
	/// </summary>
	/// <param name="database_">The database.</param>
	void selectDatabase(std::string_view database_);
	
	/// <summary>
	/// Closes connection with database.
	/// </summary>
	void close();
	
	/// <summary>
	/// Determines whether this instance is connected.
	/// </summary>
	/// <returns>
	///		<c>true</c> if this instance is connected; otherwise, <c>false</c>.
	/// </returns>
	bool isConnected() const;
	
	/// <summary>
	/// Determines whether this instance is connected to database.
	/// </summary>
	/// <returns>
	///		<c>true</c> if this instance is connected to database; otherwise, <c>false</c>.
	/// </returns>
	bool isConnectedToDatabase() const;
	
	/// <summary>
	/// Returns the internal SQL instance.
	/// </summary>
	/// <returns>The internal SQL instance.</returns>
	MYSQL& getInternalSQLInstance() const;

	/// <summary>
	/// Returns the last inserted row id.
	/// </summary>
	auto insertId() const {
		return mysql_insert_id(&_connection);
	}

private:

	mutable MYSQL	_connection;
	bool			_connected;
	std::string 	_host;
	std::string 	_user;
	std::string 	_password;
	std::string 	_database;
};

}