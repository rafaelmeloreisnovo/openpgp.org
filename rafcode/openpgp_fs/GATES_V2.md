# RAFCODEΦ OpenPGP: typed gates, provenance and rollback — V2

**Producer:** `rafaelmeloreisnovo/openpgp.org`, default branch `master`.
**Scope:** authorial `rafcode/openpgp_fs/**` only; inherited website, Jekyll,
licenses, deployment and other `rafcode/**` modules remain untouched.
**Policy:** SOURCE ≠ ARTIFACT ≠ EXECUTION ≠ EVIDENCE ≠ CLAIM.

## Material discrepancy / urgent governance finding

1. PR [#1](https://github.com/rafaelmeloreisnovo/openpgp.org/pull/1) was
   merged 2026-11-07T08:57:51Z, merge commit
   `80b37538f62e7cd9b1467698c387578272fe6676`, despite its
   `DRAFT/HOLD` description. Provider CI run
   [42354919457](https://github.com/rafaelmeloreisnovo/openpgp.org/actions/runs/42354919457)
   completed `success` for its **scoped** host/cross-object gate.
2. PR [#2](https://github.com/rafaelmeloreisnovo/openpgp.org/pull/2) was
   merged 2026-11-07T08:59:54Z, merge commit
   `ed0aeef38047ed18eeff2e2afe5f054d91792666`; independent bare-metal
   scope and gates must not be conflated with packet framing tests.
3. GitHub API `GET /rulesets` returned `[]` during this inspection.
   Branch-protection endpoint returned 403 for this connector: actual
   branch-protection state is `TOKEN_VAZIO`, not `DISABLED`.
4. A workflow's PASS does not block merging unless an effective branch rule
   requires that exact check. Markdown `HOLD` is a statement, not enforcement.

## Typed unknowns and promotion gates

| Priority | Gate / token | Current evidence | Promotable only with |
|---|---|---|---|
| P0 | `MERGE_ENFORCEMENT=TOKEN_VAZIO` | no rulesets; protection API 403 | owner checks effective branch protection and required status checks; readback |
| P0 | `CI_NEW_HEAD=TOKEN_VAZIO` | this successor has not run yet | exact-head workflow success, no stale SHA |
| P1 | `FUZZ_PROPERTY=LOCAL_PASS_SCOPED` | deterministic 131072 spans, exhaustive 16384 header cases | CI exact-head readback; true coverage-guided fuzz remains `NOT_RUN` |
| P1 | `ASAN_UBSAN=LOCAL_PASS_SCOPED` | tested hosted compiler builds | CI exact-head sanitizer jobs success |
| P1 | `DIFFERENTIAL=TOKEN_VAZIO` | no independent parser cross-check | independent implementation vectors with provenance |
| P1 | `LICENSE_REVIEW=TOKEN_VAZIO` | scoped MIT, site rights inherited | independent rights/asset inventory |
| P1 | `ANDROID_ARMV7=TOKEN_VAZIO` | cross-compiled objects, no device execution | Motorola physical run + raw receipt and hash |
| P1 | `CRYPTO_IMPL=NOT_IMPLEMENTED` | framing only | separately authorized implementation + cryptographic review |
| P2 | `PERFORMANCE=TOKEN_VAZIO` | no benchmark comparison | reproducible same-device baseline + delta |
| P2 | `RELEASE=BLOCKED` | missing review, hardware and integration | all required gates + human promotion decision |

`TOKEN_VAZIO` is an unknown, not zero or false. `NOT_RUN` and
`NOT_IMPLEMENTED` are explicit known statuses. No false promotion from
`LOCAL_PASS_SCOPED` to production readiness.

## Hotfix surface and reversible route

- Preserve master history and all upstream notices. Do not force-push.
- This successor corrects verification behavior **only**; no packet decoder
  logic, website deployment, dependencies, firmware, or public claims changed.
- Tests fail closed for missing host tools, invalid `nm` exit status, unresolved
  imports, missing compiler matrix, framing contradiction and sanitizer faults.
- Owner should enable branch/ruleset required-check enforcement and review
  restrictions separately in GitHub settings; a CI file cannot self-enforce it.
- Rollback if successor is merged and regresses: **new audited revert commit**
  limited to the successor diff (not hard reset, not history deletion), plus
  corrective successor receipt. Until review, keep the successor PR DRAFT/HOLD.
- Receipt: append-only Git SHA, exact-head CI run link and conclusion, Drive
  μWRITE pointer, private Supabase event and its readback. Mark security claim
  false until independent verification.

**Normative reference:** [RFC 9580](https://www.rfc-editor.org/rfc/rfc9580.html),
new-format definite-length packet header portion only.
