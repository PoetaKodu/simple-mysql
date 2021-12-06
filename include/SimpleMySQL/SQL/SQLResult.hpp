#pragma once

#include SIMPLEMYSQL_PCH

namespace simple_mysql
{

/// <summary>
/// Represents executed SQL query result.
/// </summary>
class Result
{
public:
	using Handle 	= MYSQL_RES;
	using Row 		= MYSQL_ROW;

	/// <summary>
	/// Initializes a new instance of the <see cref="Result"/> class.
	/// </summary>
	Result() = default;

	/// <summary>
	/// Initializes a new instance of the <see cref="Result"/> class.
	/// </summary>
	/// <param name="result_">The result of the executed query.</param>
	explicit Result(Handle & result_);

	/// <summary>
	/// Finalizes an instance of the <see cref="Result"/> class.
	/// </summary>
	~Result();

	/// <summary>
	/// Returns the underlying SQL result.
	/// </summary>
	/// <returns>The underlying SQL result.</returns>
	Handle & getResult() const;

	/// <summary>
	/// Returns the current result row.
	/// </summary>
	/// <returns>The current result row.</returns>
	Row getCurrentRow() const;

	/// <summary>
	/// Returns number of rows the result has.
	/// </summary>
	/// <returns></returns>
	uint64_t rowCount() const;

	/// <summary>
	/// Jumps to next row.
	/// </summary>
	/// <returns>
	///		<c>true</c> if jumped and didn't reach end; otherwise, <c>false</c>.
	/// </returns>
	bool jumpToNextRow();

	/// <summary>
	/// Returns the next row or nullptr if there is no more rows.
	/// </summary>
	inline Row nextRow() {
		if (jumpToNextRow())
			return getCurrentRow();
		return Row{};
	}

	/// <summary>
	/// Determines whether this instance isn't empty.
	/// </summary>
	/// <returns>
	///		<c>true</c> if result isn't empty; otherwise, <c>false</c>.
	/// </returns>
	bool isEmpty() const {
		return _result != nullptr || this->rowCount() == 0;
	}

private:
	Handle *_result{};
	Row _row{};
};

}