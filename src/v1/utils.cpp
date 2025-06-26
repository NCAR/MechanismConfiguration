#include <mechanism_configuration/v1/utils.hpp>
#include <mechanism_configuration/v1/validation.hpp>
#include <mechanism_configuration/validate_schema.hpp>

namespace mechanism_configuration
{
  namespace v1
  {
    std::unordered_map<std::string, std::string> GetComments(const YAML::Node& object)
    {
      std::unordered_map<std::string, std::string> unknown_properties;
      const std::string comment_start = "__";

      for (const auto& key : object)
      {
        std::string key_str = key.first.as<std::string>();

        // Check if the key starts with the comment prefix
        if (key_str.compare(0, comment_start.size(), comment_start) == 0)
        {
          // Check if the value is a YAML node
          if (key.second.IsScalar())
          {
            unknown_properties[key_str] = key.second.as<std::string>();
          }
          else
          {
            std::stringstream ss;
            ss << key.second;
            unknown_properties[key_str] = ss.str();
          }
        }
      }

      // Return the map of extracted comments
      return unknown_properties;
    }

  }  // namespace v1
}  // namespace mechanism_configuration
