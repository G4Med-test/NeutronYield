# Portal JSON parser

Rewritten from `geant-config-generator/tests/geant4/NeutronYield/parser.py`
(old `gts.BaseParser` + PyROOT, merging repeated-seed job groups) for the
module-level `metadata()`/`parse()` API that
`ci-workflows/validation/export.py` actually calls. ROOT is read with
`uproot` (no PyROOT on the runner); there is no job-group merge any more —
one macro means one run, so the original's "sum of NEVENTS across all jobs
in the group" becomes just this one job's own `NEVENTS`.

`config.json` selects all 14 self-contained macros already present in
`geant-validation-tests/src/NeutronYield` (`vis.mac`/`run_option.mac` are
not validation macros and are excluded) — no macro generation was needed:
proton beams at 113/256 MeV on Aluminum/Carbon/Iron (`BIC`/`Shielding`
physics lists), and a carbon-12 beam at 3480 MeV total (290 MeV/u × 12) on
water — matching all three slide energies.

The original `IPARTICLE` key was a config-generator-only templating label,
not a macro command; `metadata()` instead derives the beam particle (proton
vs C12) directly from the macro's own `/gps/particle`/`/gps/ion` commands —
self-contained, can't drift out of sync.

`HistoManager.cc` books 23 `Scorer_<angle>_deg` histograms, 10 with decimal
angle names (`0.0`, `7.5`, …) and 13 with integer names (`0`-`12`, scaled
×15° for the carbon beam); `SetH1Activation(ih, false)` leaves all of them
inactive by default; only the ones each macro explicitly reconfigures via
`/analysis/h1/set` end up actually filled. The original parser's own
filtering — skip any `"."`-named histogram for the C12 beam, otherwise
process whatever `GetListOfKeys()` returns with no filtering at all — is
reproduced exactly via `uproot`'s `keys()`; for proton runs this can include
unfilled/zero-content histograms beyond the ones the macro configured, same
as it could in the original.
