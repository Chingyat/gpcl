//
// sqlite.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2020 Zhengyi Fu (tsingyat at outlook dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef GPCL_EXT_SQLITE_HPP
#define GPCL_EXT_SQLITE_HPP

#include <gpcl/detail/utility.hpp>
#include <gpcl/ext/detail/config.hpp>
#include <gpcl/zstring.hpp>
#include <system_error>

#ifdef GPCL_EXT_ENABLE_SQLITE
#include <sqlite3.h>
#endif

namespace gpcl::ext::sqlite {

#ifdef GPCL_EXT_ENABLE_SQLITE

class database;
class query;
class statement;
class bind_proxy;
class step_result;

GPCL_DECL auto sqlite_error_category() -> std::error_category const &;

/// 數據庫連接
class database final
{
public:
  /// 創建數據庫連接
  GPCL_DECL explicit database(czstring<> dbname = ":memory:");

  database(database &&other) noexcept
      : db_(detail::exchange(other.db_, nullptr))
  {
  }

  /// 銷毀數據庫連接
  GPCL_DECL ~database() noexcept;

  /// 運行 SQL 語句
  GPCL_DECL auto execute(czstring<> sql) -> void;

  using native_handle_type = sqlite3 *;
  auto native_handle() -> native_handle_type
  {
    assert(db_);
    return db_;
  }

private:
  sqlite3 *db_;
};

/// \fn gpcl_sqlite_bind
///
/// These functions are a customization point.
///
/// In order to be able to *bind* user customized types,
/// you can define `gpcl_sqlite_bind` function in your namespace.
/// This function will be called through ADL.
///

GPCL_DECL auto gpcl_sqlite_bind(const bind_proxy &proxy, int col,
                                sqlite3_int64 number) -> void;
GPCL_DECL auto gpcl_sqlite_bind(const bind_proxy &proxy, int col, int number)
    -> void;
GPCL_DECL auto gpcl_sqlite_bind(const bind_proxy &proxy, int col,
                                const char *str) -> void;
GPCL_DECL auto gpcl_sqlite_bind(const bind_proxy &proxy, int col, double number)
    -> void;

inline auto gpcl_sqlite_bind(const bind_proxy &proxy, int col,
                             unsigned long long number) -> void
{
  gpcl_sqlite_bind(proxy, col, static_cast<sqlite3_int64>(number));
}

inline auto gpcl_sqlite_bind(const bind_proxy &proxy, int col,
                             unsigned int number) -> void
{
  gpcl_sqlite_bind(proxy, col, static_cast<sqlite3_int64>(number));
}

namespace detail {
GPCL_DECL void bind_static_string(const bind_proxy &proxy, int col,
                                  const char *str);
} // namespace detail

template <std::size_t N>
GPCL_DECL_INLINE auto gpcl_sqlite_bind(const bind_proxy &proxy, int col,
                                       const char(&&str)[N]) -> void
{
  detail::bind_static_string(proxy, col, str);
}

auto cpp_sqlite_get(const step_result &result, int col, int &number) -> void;
auto cpp_sqlite_get(const step_result &result, int col, double &number) -> void;
auto cpp_sqlite_get(const step_result &result, int col, std::string &number)
    -> void;

class bind_proxy final
{
  sqlite3_stmt *stmt_;
  int col_;

public:
  explicit bind_proxy(sqlite3_stmt *stmt, int column)
      : stmt_(stmt),
        col_(column)
  {
  }

  bind_proxy(bind_proxy &&) = default;

  /// Bind the next parameter
  template <typename T>
  GPCL_DECL_INLINE auto operator<<(T &&x) && -> bind_proxy
  {
    using sqlite::gpcl_sqlite_bind;
    gpcl_sqlite_bind(*this, col_++, gpcl::detail::forward<T>(x));
    return gpcl::detail::move(*this);
  }

  [[nodiscard]] auto column() const -> int { return col_; }
  [[nodiscard]] auto statement_handle() const noexcept -> sqlite3_stmt *
  {
    return stmt_;
  }
};

class step_result final
{
  sqlite3_stmt *stmt_;
  int col_;

public:
  explicit step_result(sqlite3_stmt *stmt, int column)
      : stmt_(stmt),
        col_(column)
  {
  }

  step_result(step_result &&) = default;

  /// Fetch the next result
  template <typename T>
  GPCL_DECL_INLINE auto operator>>(T &x) && -> step_result
  {
    cpp_sqlite_get(*this, col_, x);
    col_++;
    return gpcl::detail::move(*this);
  }

  explicit operator bool() const { return stmt_; }

  [[nodiscard]] auto column() const noexcept -> int { return col_; }

  [[nodiscard]] auto statement_handle() const noexcept -> sqlite3_stmt *
  {
    return stmt_;
  }
};

/// A prepared statement
class statement final
{
  sqlite3_stmt *stmt_ = nullptr;

public:
  statement() = default;

  /// Create a prepared statement
  GPCL_DECL statement(database &db, const char *sql);

  statement(statement &&other) noexcept : stmt_(other.stmt_)
  {
    other.stmt_ = nullptr;
  }

  /// Destroy a statement
  GPCL_DECL ~statement();

  /// Assigment
  statement &operator=(statement &&other) noexcept
  {
    this->~statement();
    new (this) statement(gpcl::detail::move(other));
    return *this;
  }

  /// Execute the statement
  GPCL_DECL auto step() -> step_result;

  /// Reset the statement to prepare for the next execution
  GPCL_DECL auto reset() -> void;

  /// Get the parameter binder
  auto binder(int start_col = 1) -> bind_proxy
  {
    return bind_proxy{stmt_, start_col};
  }

  using native_handle_type = sqlite3_stmt *;
  auto native_handle() -> native_handle_type { return stmt_; }
};

/// RAII guard for transaction
class transaction final
{
  database *db_{};

public:
  /// Create an inactive transaction
  transaction() = default;

  /// Begin a transaction for db
  GPCL_DECL explicit transaction(database &db);

  /// Move constructor
  GPCL_DECL transaction(transaction &&other) noexcept;

  /// Disable copy
  transaction(const transaction &) = delete;
  transaction &operator=(const transaction &) = delete;

  /// Destructor
  ///
  /// If the transaction is active, it will be rolled back.
  GPCL_DECL ~transaction() noexcept;

  /// Move asignment operator
  GPCL_DECL transaction &operator=(transaction &&other) noexcept;

  /// Begin a transaction for db
  ///
  /// \pre The transaction must be inactive.
  /// \post The transaction shall be active
  GPCL_DECL void begin(database &db);

  /// Commit the transaction.
  ///
  /// \pre The transaction shall be active.
  /// \post The transaction shall be inactive.
  GPCL_DECL void commit();

  /// Rollback the transaction
  ///
  /// \pre The transaction shall be active.
  /// \post The transaction shall be inactive.
  GPCL_DECL void rollback() noexcept;
};
#endif

} // namespace gpcl::ext::sqlite

#ifdef GPCL_HEADER_ONLY
#include <gpcl/ext/impl/sqlite.ipp>
#endif

#endif // GPCL_EXT_SQLITE_HPP
