# License scope and provenance — RAFCODEΦ OpenPGP FS v1

| Boundary | Source/owner | Scope | Status |
|---|---|---|---|
| Existing website, content, templates | [OpenPGP/openpgp.org](https://github.com/OpenPGP/openpgp.org) and contributors | Existing notices/rights preserved; do not relicense | INHERITED / REVIEW_REQUIRED |
| Minimal Mistakes theme | Michael Rose and upstream | MIT per upstream README and package metadata | INHERITED MIT |
| Website images | Upstream image authors | Upstream README says CC0 except as noted | VERIFY_PER_ASSET |
| `rafcode/openpgp_fs/**` | Rafael Melo Reis (RAFCODEΦ), newly authored code | MIT license in this directory | AUTHORIAL SCOPED |
| `.github/workflows/rafpgp-fs-gate.yml` | Rafael Melo Reis (RAFCODEΦ), newly authored scoped CI gate | SPDX MIT header | AUTHORIAL SCOPED |
| OpenPGP format | IETF RFC 9580 | Public interoperability specification, not a code license | STANDARDS REFERENCE |

Do not change upstream credits, copyright headers, third-party licenses, or the Git history. The top-level site content has **no demonstrated blanket MIT grant** from the checked README. A fork is not a new chain of title. This scope file is a provenance record, not a legal opinion or external certification.

Proposed changes are intentionally isolated: no edits to `_config.yml`, inherited assets, build/deployment configuration, DNS, keys or upstream pages. No real cryptographic operation is implemented by these sources.
