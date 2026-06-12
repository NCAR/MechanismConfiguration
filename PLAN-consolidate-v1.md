# Plan: Consolidate on one parser (rename development → v1) for issue #269

Status: **in progress.** Stage 1 done (uncommitted). Execute remaining stages one
at a time; each ends green (builds + tests pass). Commits gated on review.

## Goal (from issue #269)

A single, canonical parser exposing **`parse()`** and **`validate()`**, returning
`std::expected<Mechanism, Errors>`, with both structural and semantic errors
reported. Dispatch by version. This is preparation for the aerosol work (a later
branch), which builds on top of this engine.

Decisions already made:
- **Drop `.map()` / `.map_error()`** (AC#2). GCC 12's `std::expected` has no
  monadic ops; not worth a toolchain bump or shims right now.
- **Drop v2/development as a separate version.** There is no meaningful schema
  difference; maintaining two engines isn't justified.
- **Keep v0** as its own legacy path (community still has v0 files).
- **`validate()` works on the canonical `Mechanism`, not just YAML**, so an
  in-code mechanism can be validated. Line numbers are preserved for file inputs
  via **Option 2** (shared rule predicates + two thin walkers) — see Validation
  architecture below. No source-location field is added to the domain types.

## Core idea

The `development` engine already has the two-phase `Validate()` / `Parse()` split
we want. Rather than retrofit v1's combined parser, **rename `development` → `v1`**
and delete the old combined v1 parsers. The renamed engine becomes the single v1
implementation; top-level `parse()` dispatches v0 / v1.

## Validation architecture (decided: Option 2)

Validation is sliced into layers so the same rules cover both an in-code
`Mechanism` and one parsed from a file, while files still get line numbers.

1. **Deserialization (YAML-only, at parse time).** Unknown/mistyped keys, wrong
   scalar types, and the `name`/`species name` alias collapse. These have *no
   in-code analog* (by the time you hold a `Mechanism`, the typo'd key is gone and
   the alias has collapsed to `.name`), so they live only at the parse boundary and
   are not duplicated. They keep native line numbers from `node.Mark()`.

2. **Structural + semantic (the rules).** "Required-ness" is re-expressed as
   **populated-ness on the struct** (e.g. `mechanism.species` non-empty, a reaction
   has ≥1 reactant), plus referential checks (reactions reference known species,
   phase membership, duplicates). The *rules* are written once as **pure
   predicates** over plain values — not over YAML, not over the struct:

   ```cpp
   bool SpeciesExists(std::string_view name, const std::vector<types::Species>&);
   // ...one per rule
   ```

   Two **thin walkers** call these predicates:
   - **File walker** traverses the YAML, calls the predicates, and attaches
     `node.Mark()` on failure → real `line:col` in messages.
   - **`Mechanism` walker** (`validate(const Mechanism&)`) traverses the struct,
     calls the *same* predicates, reports without a location.

   The validation *logic* is single-sourced (the predicates); only the traversal
   exists in two small drivers. Domain types stay clean — **no `source` field.**

Resulting composition:

```
parse(file): YAML --(1) deserialize--> Mechanism --(2) file walker--> Errors(+line:col)
in-code:     Mechanism --------------------------- (2) Mechanism walker --> Errors
```

Honest cost: two traversal drivers (loop-over-YAML vs loop-over-struct). Accepted
to keep line numbers without polluting the types.

## The one hard prerequisite: `name` / `species name` alias

Schemas differ in exactly one headline place (verified against fixtures + the
`validation` headers):

- **v1 community files** use `species name:` for reaction-component species
  (`v1::validation::species_name = "species name"`).
- **development** expects `name:` (top-level `validation` has no `species_name`).

Everything else checked aligns (`reactants` / `products` / `gas phase` match).

So if we rename development→v1 as-is, **every existing community v1 file breaks.**
The fix is the alias work in `TODO-name-species_name-alias.md`: make the
reaction-component parser accept *either* `name` or `species name` (exactly one),
via an `exactly_one_of` extension to `ValidateSchema`. This MUST land (and be
verified against current v1 fixtures) before the rename.

---

## Stages

### Stage 0 — Wire up top-level `parse()` dispatch (optional warm-up)

`src/parse.cpp` currently `throw`s for v0/v1 and has dead `f()` + commented code.
Independent of the rename, it can be made real:
- missing version field → v0; major 0 → v0; major 1 → current v1 engine
- delete `f()` + commented blocks; re-enable the v0 include
- fix `parse.hpp` (remove the duplicate `parse(path)` decl)

This is low-risk and proves the dispatch shape before the rename churns it.
(Optional — can also be folded into Stage 3.)

### Stage 1 — `name` / `species name` alias  *(prerequisite)*  — ✅ DONE (uncommitted)

Implemented per `TODO-name-species_name-alias.md`:
- `ValidateSchema` gained a defaulted `exactly_one_of` group parameter
  (`validate_schema.{hpp,cpp}`): zero → `RequiredKeyNotFound`, >1 →
  `MutuallyExclusiveOption`; group members count as allowed keys.
- `species_name = "species name"` added to top-level `validation.hpp`.
- `ValidateReactantsOrProducts` requires exactly one of `{name, species name}`.
- New `GetReactionComponentName(node)` helper (development/utils); the 34
  component-name reads across validators + the parser read route through it.

**Verified:** new fixtures `arrhenius/species_name_alias.{yaml,json}` and
`species_name_conflict.{yaml,json}`, plus tests `AcceptsSpeciesNameAlias` and
`RejectsBothNameAndSpeciesName`. Full suite **45/45 green**; development parses
`species name`.

### Stage 2 — Rename development → v1

Mechanical, large diff. Expect build-fix iterations (like the string_view sweep).
- `git mv src/development/* src/v1/*` (delete the old combined v1 parsers they
  replace: `src/v1/reactions/*_parser.cpp`, `mechanism_parsers.{hpp,cpp}`,
  old `v1/parser.{hpp,cpp}`).
- `git mv include/.../development/* include/.../v1/*`.
- Namespace sweep `mechanism_configuration::development` → `mechanism_configuration::v1`.
- Reconcile validation namespaces: the engine uses top-level `validation`
  (string_view). Fold/drop the old `v1::validation` (the one with `species_name`).
- Update `CMakeLists.txt` files and all include paths.

Exit: library builds.

### Stage 3 — Top-level `parse()` / `validate()`

- `parse(path)` / `parse(string)`: dispatch missing-version/0 → v0, version 1 →
  renamed engine. No v2 branch. Returns `std::expected<Mechanism, Errors>` with
  all errors (structural + semantic).
- Refactor validation per the **Validation architecture** above:
  - Extract the per-reaction semantic checks into **pure rule predicates** over
    plain values.
  - Keep the **file walker** (YAML, attaches `node.Mark()`) for parse-time errors.
  - Add the **`Mechanism` walker** `validate(const Mechanism&) -> Errors` calling
    the same predicates (no locations).
- `validate(...)`: public, operates on the canonical `Mechanism`.
- Fix `parse.hpp` (duplicate decl; add `parse(string)` + `validate`).

Exit: `parse()` / `validate()` work end-to-end for both file and in-code inputs.

### Stage 4 — Tests + backward-compat proof

- Migrate the renamed engine's tests (former development tests) under v1.
- **Critical:** run the existing **v1 community fixtures** through the renamed
  engine. This is the real proof the rename + alias preserved the community format.
  Keep these fixtures as the regression net.
- Remove/merge now-duplicate test targets; update test CMakeLists.

Exit: full suite green; v1 community fixtures parse unchanged.

---

## Open decisions to resolve along the way

1. ~~**`validate()` signature**~~ — RESOLVED: `validate(const Mechanism&)` with
   Option 2 (shared predicates + two walkers). See Validation architecture.
2. **`types::` namespace** (#269 wording): ticket muses about
   `mechanism_configuration::Arrhenius` (no `types::`). Current canonical is
   `mechanism_configuration::types::Arrhenius`. Lowest churn is to keep `types::`;
   confirm whether the ticket truly wants it dropped.

## Risks / notes

- Stage 2 is the big, noisy one — pure mechanical churn, validated by Stage 4
  fixtures. Do it on its own commit so it's easy to review/revert.
- The current full suite is **45/45 green** — that is the baseline each stage must
  preserve. Re-run `ctest` at every stage exit.
- Related artifact: `TODO-name-species_name-alias.md` (Stage 1 spec).
