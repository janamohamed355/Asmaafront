#pragma once
#include <exception>
#include <iostream>
#include <string>

// Try multiple common include paths for MySQL Connector/C++ headers.
// This avoids a hard #error and provides a helpful compile-time message
// if the headers are not found in the usual locations.
#if __has_include(<mysql_driver.h>) && __has_include(<mysql_connection.h>)
#include <mysql_driver.h>
#include <mysql_connection.h>
#elif __has_include(<mysql/mysql_driver.h>) && __has_include(<mysql/mysql_connection.h>)
#include <mysql/mysql_driver.h>
#include <mysql/mysql_connection.h>
#elif __has_include(<mysql/cppconn/mysql_driver.h>) && __has_include(<mysql/cppconn/mysql_connection.h>)
#include <mysql/cppconn/mysql_driver.h>
#include <mysql/cppconn/mysql_connection.h>
#else
#if defined(_MSC_VER)
#pragma message("Warning: mysql_driver.h/mysql_connection.h not found. Add MySQL Connector/C++ include directory to your project settings.")
#else
#warning "mysql_driver.h/mysql_connection.h not found. Add MySQL Connector/C++ include directory to your project settings."
#endif
#endif

// Prefer including the real cppconn driver/connection headers when available.
// If they're not available on the include path, provide minimal declarations
// so code that references these types can still be parsed/compiled (useful for
// IntelliSense or builds that don't link the real connector).
#if __has_include(<cppconn/driver.h>) && __has_include(<cppconn/connection.h>)
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#else
// Minimal fallback declarations for sql namespace (adds SQLException to fix E0135)
namespace sql {
    class SQLException : public std::exception {
    public:
        SQLException(const std::string& msg = "", int errorCode = 0)
            : message(msg), errorCode(errorCode) {}
        virtual ~SQLException() noexcept {}
        virtual const char* what() const noexcept override { return message.c_str(); }
        int getErrorCode() const noexcept { return errorCode; }

    private:
        std::string message;
        int errorCode;
    };

    class ResultSet {
    public:
        virtual ~ResultSet() noexcept {}
        virtual bool next() = 0;
        virtual std::string getString(const std::string& columnLabel) = 0;
        virtual int getInt(const std::string& columnLabel) = 0;
        virtual double getDouble(const std::string& columnLabel) = 0;
    };

    class Statement {
    public:
        virtual ~Statement() noexcept {}
        virtual ResultSet* executeQuery(const std::string& query) = 0;
        virtual int executeUpdate(const std::string& query) = 0;
    };

    class PreparedStatement {
    public:
        virtual ~PreparedStatement() noexcept {}
        virtual void setString(int parameterIndex, const std::string& value) = 0;
        virtual void setInt(int parameterIndex, int value) = 0;
        virtual void setDouble(int parameterIndex, double value) = 0;
        virtual int executeUpdate() = 0;
        virtual ResultSet* executeQuery() = 0;
    };

    class Connection {
    public:
        virtual ~Connection() noexcept {}
        virtual void setSchema(const char* schema) = 0;
        virtual Statement* createStatement() = 0;
        virtual PreparedStatement* prepareStatement(const std::string& query) = 0;
    };

    class Driver {
    public:
        virtual ~Driver() noexcept {}
        virtual Connection* connect(const char* host, const char* user, const char* password) = 0;
    };
}
#endif

// Forward declarations for mysql namespace only if mysql headers weren't included.
// If the full mysql headers are present above, these declarations are redundant but harmless.
#if !(__has_include(<mysql_driver.h>) && __has_include(<mysql_connection.h>)) \
 && !(__has_include(<mysql/mysql_driver.h>) && __has_include(<mysql/mysql_connection.h>)) \
 && !(__has_include(<mysql/cppconn/mysql_driver.h>) && __has_include(<mysql/cppconn/mysql_connection.h>))
namespace sql {
    namespace mysql {
        class MySQL_Driver;
        sql::Driver* get_mysql_driver_instance();
    }
}
#endif

class DbConnection {
public:
    sql::Connection* con = nullptr;

    // Compatibility: some files call getConnect(), others call connect()
    sql::Connection* getConnect() {
        return connect();
    }

    // دالة لفتح الاتصال
    sql::Connection* connect() {
        try {
            // Use the abstract sql::Driver type instead of the concrete MySQL_Driver forward declaration.
            sql::Driver* driver = sql::mysql::get_mysql_driver_instance();

            con = driver->connect("tcp://127.0.0.1:3306", "root", "jana12345");

            con->setSchema("hospital_db");
            std::cout << "Connected Successfully!" << std::endl;

            return con;
        }
        catch (sql::SQLException& e) {
            std::cout << "MySQL Error: " << e.what() << std::endl;
            std::cout << "Error Code: " << e.getErrorCode() << std::endl;
            return nullptr;
        }
        catch (std::exception& e) {
            std::cout << "General Error: " << e.what() << std::endl;
            return nullptr;
        }
    }

    void close() {
        if (con != nullptr) {
            delete con;
            con = nullptr;
        }
    }
};
