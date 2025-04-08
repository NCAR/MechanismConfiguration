// clang-format off
#pragma once

#ifdef __cplusplus
namespace mechanism_configuration
{
extern "C" {
#endif

  const char* getVersionString()
  {
    return "0.1.1";
  }
  unsigned getVersionMajor()
  {
    return 0;
  }
  unsigned getVersionMinor()
  {
    return 1+0;
  }
  unsigned getVersionPatch()
  {
    return 1+0;
  }
  unsigned getVersionTweak()
  {
    return +0;
  }

#ifdef __cplusplus
}
}
#endif
