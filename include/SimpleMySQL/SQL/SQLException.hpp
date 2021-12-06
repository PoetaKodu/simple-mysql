#pragma once

#include SIMPLEMYSQL_PCH

#include <stdexcept>
#include <string>

namespace simple_mysql
{

struct Exception : std::runtime_error
{
	/// <summary>
	/// Initializes a new instance of the <see cref="Exception"/> struct.
	/// </summary>
	/// <param name="message_">The message.</param>
	Exception(std::string message_)
		: std::runtime_error( std::move(message_) )
	{
	}
};

}