// clang-format off
#pragma once

#ifdef __cplusplus
namespace mechanism_configuration
{
extern "C" {
#endif

  const char* getVersionString()
  {
    return "1.0.0";
  }
  unsigned getVersionMajor()
  {
    return 1;
  }
  unsigned getVersionMinor()
  {
    return 0+0;
  }
  unsigned getVersionPatch()
  {
    return 0+0;
  }
  unsigned getVersionTweak()
  {
    return +0;
  }

#ifdef __cplusplus
}
}
#endif
