// Copyright (C) 2023–2026 University Corporation for Atmospheric Research
//                         University of Illinois at Urbana-Champaign
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef MECH_CONFIG_USE_FMT
#  include <fmt/format.h>
namespace mc_fmt = fmt;
#else
#  include <format>
namespace mc_fmt = std;
#endif
