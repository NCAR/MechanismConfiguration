// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <concepts>
#include <cstdint>
#include <map>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace mechanism_configuration::types
{

  /// @brief A JSON-like value that holds an unknown (user-defined) property.
  ///        Scalars keep their type, and sequences and maps keep their structure.
  struct PropertyValue
  {
    using Array = std::vector<PropertyValue>;
    using Object = std::map<std::string, PropertyValue>;
    using Variant = std::variant<std::nullptr_t, bool, std::int64_t, double, std::string, Array, Object>;

    Variant value{ nullptr };

    PropertyValue() = default;

    /// @brief Constructs from any type that the variant accepts, for example nullptr, true, 3, 0.5, "text",
    ///        an Array, or an Object. An int becomes std::int64_t, and a string literal becomes std::string.
    template<class T>
      requires(!std::same_as<std::remove_cvref_t<T>, PropertyValue> && std::constructible_from<Variant, T &&>)
    PropertyValue(T&& v)
        : value(std::forward<T>(v))
    {
    }

    bool IsNull() const
    {
      return std::holds_alternative<std::nullptr_t>(value);
    }
    bool IsBool() const
    {
      return std::holds_alternative<bool>(value);
    }
    bool IsInt() const
    {
      return std::holds_alternative<std::int64_t>(value);
    }
    bool IsDouble() const
    {
      return std::holds_alternative<double>(value);
    }
    bool IsString() const
    {
      return std::holds_alternative<std::string>(value);
    }
    bool IsArray() const
    {
      return std::holds_alternative<Array>(value);
    }
    bool IsObject() const
    {
      return std::holds_alternative<Object>(value);
    }

    bool AsBool() const
    {
      return std::get<bool>(value);
    }
    std::int64_t AsInt() const
    {
      return std::get<std::int64_t>(value);
    }
    /// @brief Returns the value as a double. Integers are converted.
    double AsDouble() const
    {
      if (IsInt())
        return static_cast<double>(AsInt());
      return std::get<double>(value);
    }
    const std::string& AsString() const
    {
      return std::get<std::string>(value);
    }
    const Array& AsArray() const
    {
      return std::get<Array>(value);
    }
    const Object& AsObject() const
    {
      return std::get<Object>(value);
    }

    /// @brief Returns the value as one line of YAML flow-style text, for example {"a": [1, "x"]}.
    ///        Strings are double-quoted, so the text is also valid JSON for finite numbers.
    std::string ToString() const;

    bool operator==(const PropertyValue& other) const = default;

    /// @brief True only when the value is a string equal to `other`
    bool operator==(std::string_view other) const
    {
      return IsString() && AsString() == other;
    }
    bool operator==(const char* other) const
    {
      return *this == std::string_view(other);
    }
  };

  inline std::ostream& operator<<(std::ostream& os, const PropertyValue& value)
  {
    return os << value.ToString();
  }

  /// @brief Unknown properties of an object. Keys keep their two-underscore (__) prefix.
  using UnknownProperties = std::unordered_map<std::string, PropertyValue>;

}  // namespace mechanism_configuration::types
