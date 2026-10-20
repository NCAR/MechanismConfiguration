// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#include "detail/property_value.hpp"

#include <cstdint>

namespace mechanism_configuration
{
  namespace
  {
    types::PropertyValue ScalarToPropertyValue(const YAML::Node& node)
    {
      // A quoted scalar has the non-specific tag "!", so it is always a string
      if (node.Tag() == "!")
        return types::PropertyValue(node.Scalar());

      std::int64_t integer{};
      if (YAML::convert<std::int64_t>::decode(node, integer))
        return types::PropertyValue(integer);
      double number{};
      if (YAML::convert<double>::decode(node, number))
        return types::PropertyValue(number);
      bool boolean{};
      if (YAML::convert<bool>::decode(node, boolean))
        return types::PropertyValue(boolean);
      return types::PropertyValue(node.Scalar());
    }

    void Emit(YAML::Emitter& out, const types::PropertyValue& value)
    {
      if (value.IsNull())
        out << YAML::Null;
      else if (value.IsBool())
        out << value.AsBool();
      else if (value.IsInt())
        out << value.AsInt();
      else if (value.IsDouble())
        out << value.AsDouble();
      else if (value.IsString())
        out << value.AsString();
      else if (value.IsArray())
      {
        out << YAML::BeginSeq;
        for (const auto& item : value.AsArray())
          Emit(out, item);
        out << YAML::EndSeq;
      }
      else
      {
        out << YAML::BeginMap;
        for (const auto& [key, item] : value.AsObject())
        {
          out << YAML::Key << key << YAML::Value;
          Emit(out, item);
        }
        out << YAML::EndMap;
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
        for (const auto& item : node)
          array.push_back(ToPropertyValue(item));
        return types::PropertyValue(std::move(array));
      }
      case YAML::NodeType::Map:
      {
        types::PropertyValue::Object object;
        for (const auto& item : node)
          object.emplace_back(item.first.as<std::string>(), ToPropertyValue(item.second));
        return types::PropertyValue(std::move(object));
      }
      default: return types::PropertyValue(nullptr);
    }
  }

  std::string types::PropertyValue::ToString() const
  {
    // Flow style with double-quoted strings gives one line that is also valid JSON
    YAML::Emitter out;
    out.SetMapFormat(YAML::Flow);
    out.SetSeqFormat(YAML::Flow);
    out.SetStringFormat(YAML::DoubleQuoted);
    out.SetNullFormat(YAML::LowerNull);
    out.SetBoolFormat(YAML::TrueFalseBool);
    Emit(out, *this);
    return out.c_str();
  }
}  // namespace mechanism_configuration
