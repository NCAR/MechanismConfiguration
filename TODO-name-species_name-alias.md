# TODO: Accept `name` or `species name` in reaction components (drop `species_name`)

Status: **not started** — design agreed, implementation pending.

## Goal

In `v1`, reaction-component species references currently require the key
`"species name"` (`validation::species_name`). Development dropped this in favor
of `name`. We want reaction components to accept **either** `name` **or**
`"species name"`, requiring **exactly one** (error if both, error if neither).

## Key facts

- `ParseSpecies` (`src/v1/mechanism_parsers.cpp:27`) **already** requires `name` —
  no change needed there.
- The `"species name"` key is only used by **`ParseReactionComponent`**
  (`src/v1/mechanism_parsers.cpp:220`), which every reaction's reactant/product
  list flows through. Fixing it there covers all reaction types at once.
- Validation constants (`include/mechanism_configuration/v1/validation.hpp`):
  - `name = "name"` (line 15)
  - `species_name = "species name"` (line 38)
- Error code `MutuallyExclusiveOption` already exists (`errors.hpp:38`) for the
  "both present" case.

## Chosen approach: extend `ValidateSchema` with an "exactly one of" group

Preferred over a local XOR check in the parser because it centralizes both the
"exactly one must be present" logic AND the "don't flag these as invalid keys"
bookkeeping, is reusable for future v0->v1 aliases, and (via a default arg)
leaves the ~50 existing `ValidateSchema` callers untouched.

### 1. `include/mechanism_configuration/validate_schema.hpp`

```cpp
Errors ValidateSchema(
    const YAML::Node& object,
    const std::vector<std::string>& required_keys,
    const std::vector<std::string>& optional_keys,
    const std::vector<std::vector<std::string>>& exactly_one_of = {});
```

### 2. `src/validate_schema.cpp`

After the existing missing-required block (~line 55), add a per-group check.
NOTE: `set_intersection`/`set_difference` require sorted ranges — sort copies of
each group.

```cpp
    // Mutually-exclusive required groups: exactly one member of each group must appear
    for (const auto& group : exactly_one_of)
    {
      std::vector<std::string> present;
      std::set_intersection(
          object_keys.begin(), object_keys.end(),
          group.begin(), group.end(),          // group must be sorted (sort a copy)
          std::back_inserter(present));

      if (present.empty())
        errors.push_back({ ErrorCode::RequiredKeyNotFound,
            mc_fmt::format("{} error: Exactly one of {} is required.", error_location, /* join(group) */) });
      else if (present.size() > 1)
        errors.push_back({ ErrorCode::MutuallyExclusiveOption,
            mc_fmt::format("{} error: Only one of {} may be specified.", error_location, /* join(group) */) });
    }
```

Then where `invalid_keys` is computed (~lines 58-72), build the allowed set as
`required ∪ optional ∪ all group members` before the `set_difference`, so a valid
alias is not flagged `InvalidKey`.

### 3. `src/v1/mechanism_parsers.cpp` — `ParseReactionComponent` (~lines 224-233)

```cpp
    const std::vector<std::string> reaction_component_optional_keys = { validation::coefficient };
    const std::vector<std::vector<std::string>> one_of = { { validation::name, validation::species_name } };

    auto validate = ValidateSchema(object, /*required=*/{}, reaction_component_optional_keys, one_of);
    errors.insert(errors.end(), validate.begin(), validate.end());
    if (validate.empty())
    {
      std::string species_name = object[validation::name]
          ? object[validation::name].as<std::string>()
          : object[validation::species_name].as<std::string>();
      component.name = species_name;
      // ... existing coefficient / unknown_properties handling ...
    }
```

## Why not a local XOR in the parser

A local version (both keys in `optional_keys`, XOR checked inside
`ParseReactionComponent` under `if (validate.empty())`) has a trap: when a
component has neither alias AND a junk key, `ValidateSchema` reports only
`InvalidKey` and you never reach the missing-required branch.

The existing `bad_reaction_component` fixture is exactly this case — its key is
`"Species name"` (capital S), matching neither `name` nor `"species name"`. The
local approach yields **1** error (`InvalidKey`), breaking the `size()==2`
assertion. The `ValidateSchema` approach yields `RequiredKeyNotFound` (empty
group) + `InvalidKey`, preserving **2** errors in the order the test expects.

## Watch out for

- Sorted ranges required for `set_intersection`/`set_difference` (sort copies of
  groups + the allowed set, matching how `required`/`optional` are already sorted).
- When both aliases are present, `MutuallyExclusiveOption` is pushed and `validate`
  is non-empty, so the value-read branch is correctly skipped.
- After implementing, rebuild the affected reaction tests and confirm error sets
  still line up (especially `first_order_loss` / `photolysis` `bad_reaction_component`).

## Related (separate) open item

`first_order_loss_parser.cpp:106` and `photolysis_parser.cpp:109` do
`... .reactants = reactants.second[0];` which segfaults when the reactant list is
empty (out-of-bounds). Recommended guard: assign only `if (!reactants.second.empty())`.
This is the cause of the 2 currently-failing v1 reaction tests. Tracked separately.
