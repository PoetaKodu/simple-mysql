# Simple MySQL

A simple C++ wrapper for C MySQL connector.

## Installation

Uses [pacc](https://github.com/PoetaKodu/pacc) package manager.

```shell
pacc install PoetaKodu/simple-mysql --save
```

## Usage

```cpp
#include <SimpleMySQL/Everything.hpp>
#include <iostream>

namespace sql = simple_mysql;

int main()
{
	sql::Connection conn;
	conn.connect("localhost", "root", "mypassword", "dbname");

	auto result = conn.query("SELECT `id`, `first_name`, `last_name`, `grade` FROM `students`");

	std::cout << "Loaded " << result.rowCount() << " students.\n";
	while(auto row = result.nextRow())
	{
		std::cout << '[', row[0] << "]: " << row[1] << ' ' << row[2] << ", grade: " << row[3] << '\n';
	}
}
```