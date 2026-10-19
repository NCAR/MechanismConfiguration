// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/property_value.hpp"

#include <charconv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <regex>
#include <stdexcept>

namespace mechanism_configuration
{
  namespace
  {
    // YAML 1.2 core schema patterns for plain scalars
    const std::regex kNullPattern{ "^(~|null|Null|NULL)?$" };
    const std::regex kTruePattern{ "^(true|True|TRUE)$" };
    const std::regex kFalsePattern{ "^(false|False|FALSE)$" };
    const std::regex kIntPattern{ "^[-+]?[0-9]+$" };
    const std::regex kFloatPattern{ "^[-+]?(\\.[0-9]+|[0-9]+(\\.[0-9]*)?)([eE][-+]?[0-9]+)?$" };
    const std::regex kInfPattern{ "^([-+]?)\\.(inf|Inf|INF)$" };
    const std::regex kNanPattern{ "^\\.(nan|NaN|NAN)$" };

    types::PropertyValue ScalarToPropertyValue(const YAML::Node& node)
    {
      const std::string& text = node.Scalar();

      // A quoted scalar has the non-specific tag "!", so it is always a string
      if (node.Tag() == "!")
        return types::PropertyValue(text);

      std::smatch match;
      if (std::regex_match(text, kNullPattern))
        return types::PropertyValue(nullptr);
      if (std::regex_match(text, kTruePattern))
        return types::PropertyValue(true);
      if (std::regex_match(text, kFalsePattern))
        return types::PropertyValue(false);
      if (std::regex_match(text, kIntPattern))
      {
        std::int64_t value{};
        const char* begin = text.data() + (text[0] == '+' ? 1 : 0);
        auto [ptr, ec] = std::from_chars(begin, text.data() + text.size(), value);
        if (ec == std::errc() && ptr == text.data() + text.size())
          return types::PropertyValue(value);
        // Out of range for a 64-bit integer: fall through to a double
      }
      if (std::regex_match(text, kFloatPattern) || std::regex_match(text, kIntPattern))
      {
        try
        {
          return types::PropertyValue(std::stod(text));
        }
        catch (const std::out_of_range&)
        {
          // Too large or too small for a double, so keep the text
          return types::PropertyValue(text);
        }
      }
      if (std::regex_match(text, match, kInfPattern))
        return types::PropertyValue(
            match[1] == "-" ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity());
      if (std::regex_match(text, kNanPattern))
        return types::PropertyValue(std::numeric_limits<double>::quiet_NaN());
      return types::PropertyValue(text);
    }

    void AppendJsonString(std::string& out, const std::string& s)
    {
      out += '"';
      for (unsigned char c : s)
      {
        switch (c)
        {
          case '"': out += "\\\""; break;
          case '\\': out += "\\\\"; break;
          case '\b': out += "\\b"; break;
          case '\f': out += "\\f"; break;
          case '\n': out += "\\n"; break;
          case '\r': out += "\\r"; break;
          case '\t': out += "\\t"; break;
          default:
            if (c < 0x20)
            {
              char buffer[7];
              std::snprintf(buffer, sizeof(buffer), "\\u%04x", c);
              out += buffer;
            }
            else
            {
              out += static_cast<char>(c);
            }
        }
      }
      out += '"';
    }

    void AppendJson(std::string& out, const types::PropertyValue& v)
    {
      if (v.IsNull())
        out += "null";
      else if (v.IsBool())
        out += v.AsBool() ? "true" : "false";
      else if (v.IsInt())
        out += std::to_string(v.AsInt());
      else if (v.IsDouble())
      {
        double d = v.AsDouble();
        // JSON has no representation for inf or nan
        if (!std::isfinite(d))
        {
          out += "null";
          return;
        }
        // Use the shortest precision that reads back to the same double
        char buffer[32];
        for (int precision = 15; precision <= 17; ++precision)
        {
          std::snprintf(buffer, sizeof(buffer), "%.*g", precision, d);
          if (std::strtod(buffer, nullptr) == d)
            break;
        }
        std::string text(buffer);
        // Keep a double recognizable as a double when it has no fraction or exponent
        if (text.find_first_of(".eE") == std::string::npos)
          text += ".0";
        out += text;
      }
      else if (v.IsString())
        AppendJsonString(out, v.AsString());
      else if (v.IsArray())
      {
        out += '[';
        bool first = true;
        for (const auto& item : v.AsArray())
        {
          if (!first)
            out += ',';
          first = false;
          AppendJson(out, item);
        }
        out += ']';
      }
      else
      {
        out += '{';
        bool first = true;
        for (const auto& [key, item] : v.AsObject())
        {
          if (!first)
            out += ',';
          first = false;
          AppendJsonString(out, key);
          out += ':';
          AppendJson(out, item);
        }
        out += '}';
      }
    }
  }  // namespace

  types::PropertyValue ToPropertyValue(const YAML::Node& node)
  {
    switch (node.Type())
    {
      case YAML::NodeType::Scalar: return ScalarToPropertyValue(node);
      case YAML::NodeType::Sequence:
      {
        types::PropertyValue::Array array;
        array.reserve(node.size());
        for (const auto& item : node)
          array.push_back(ToPropertyValue(item));
        return types::PropertyValue(std::move(array));
      }
      case YAML::NodeType::Map:
      {
        types::PropertyValue::Object object;
        for (const auto& item : node)
          object[item.first.as<std::string>()] = ToPropertyValue(item.second);
        return types::PropertyValue(std::move(object));
      }
      default: return types::PropertyValue(nullptr);
    }
  }

  std::string types::PropertyValue::ToJson() const
  {
    std::string out;
    AppendJson(out, *this);
    return out;
  }
}  // namespace mechanism_configuration
