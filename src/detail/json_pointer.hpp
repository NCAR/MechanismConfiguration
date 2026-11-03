// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mechanism_configuration/errors.hpp>

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

namespace mechanism_configuration
{
  // JSON Pointers (RFC 6901) identify the item in a configuration document that causes an
  // error. The empty string is the whole document. The paths use only the fixed keys of the
  // format and array indexes, which contain no '~' or '/', so the keys need no escape.

  /// @brief Appends an object key to a JSON Pointer.
  inline void AppendSegment(std::string& path, std::string_view key)
  {
    path += '/';
    path += key;
  }

  /// @brief Appends an array index to a JSON Pointer.
  inline void AppendSegment(std::string& path, std::size_t index)
  {
    path += '/';
    path += std::to_string(index);
  }

  /// @brief Appends keys and indexes to a JSON Pointer, for example ChildPath(path, "species", 2).
  template<typename... Segments>
  std::string ChildPath(std::string_view parent, const Segments&... segments)
  {
    std::string path(parent);
    (AppendSegment(path, segments), ...);
    return path;
  }

  /// @brief Makes a JSON Pointer from the root of the document, for example RootPath("reactions", 3).
  template<typename... Segments>
  std::string RootPath(const Segments&... segments)
  {
    return ChildPath("", segments...);
  }

  /// @brief Gives a path to each error that has no path.
  inline void SetMissingPaths(Errors& errors, const std::string& path, std::size_t first = 0)
  {
    for (std::size_t i = first; i < errors.size(); ++i)
      if (!errors[i].path)
        errors[i].path = path;
  }

  /// @brief Gives the path of an item to the errors that the check of the item adds.
  ///        Create one at the start of the loop body over a list. When it goes out of
  ///        scope, also after a `continue`, the new errors without a path get the path.
  class ItemErrorPath
  {
   public:
    ItemErrorPath(Errors& errors, std::string path)
        : errors_(errors),
          first_(errors.size()),
          path_(std::move(path))
    {
    }
    ~ItemErrorPath()
    {
      SetMissingPaths(errors_, path_, first_);
    }
    ItemErrorPath(const ItemErrorPath&) = delete;
    ItemErrorPath& operator=(const ItemErrorPath&) = delete;

   private:
    Errors& errors_;
    std::size_t first_;
    std::string path_;
  };
}  // namespace mechanism_configuration
