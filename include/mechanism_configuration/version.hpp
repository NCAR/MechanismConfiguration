// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

// clang-format off
#pragma once

#ifdef __cplusplus
namespace mechanism_configuration
{
extern "C" {
#endif

  inline const char* getVersionString()
  {
    return "2.0.0";
  }
  inline unsigned getVersionMajor()
  {
    return 2;
  }
  inline unsigned getVersionMinor()
  {
    return 0+0;
  }
  inline unsigned getVersionPatch()
  {
    return 0+0;
  }
  inline unsigned getVersionTweak()
  {
    return +0;
  }

#ifdef __cplusplus
}
}
#endif
