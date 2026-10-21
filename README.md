# NeutronYield

**Neutron yield from 113 and 256 MeV proton beams and 290 MeV/u carbon-ion
beams**, benchmarked against Satoh et al., *Nucl. Instrum. Methods B* (2016),
*"Distributions of neutron yields and doses from a thick target bombarded
with 290-MeV/nucleon carbon ions"* (and companion proton measurements).

---

## 🔬 What this test does

Proton beams (113 MeV, 256 MeV) on Aluminum/Carbon/Iron targets, and a
carbon-12 beam (3480 MeV total = 290 MeV/u × 12) on G4_WATER, are simulated;
the double-differential neutron yield (angle/energy) produced in the target
is scored and compared to the Satoh et al. thick-target measurements.

## 📚 Reference

- Source: `geant-validation-tests/src/NeutronYield`.
- Satoh D. et al., *Nucl. Instrum. Methods B* 366 (2016) 149-155.

## Import notes

- `main.cc`, `include/`, `src/` come from `NeutronYield.cc`/`include`/`src`.
- `macro/` contains the 16 macros found directly in that source tree,
  covering all three slide energies: `*Iron113MeV.mac`/`*Carbon113MeV.mac`/
  `*Aluminum113MeV.mac`, `*256MeV.mac` variants, and
  `carbon_*_Water290MeVu.mac`.
- `validation/parser.py` was **rewritten** for the `metadata()`/`parse()`
  module API that `ci-workflows/validation/export.py` actually calls (the
  first version, copied unmodified from
  `geant-config-generator/tests/geant4/NeutronYield/parser.py`, used the old
  `gts.BaseParser` class API and would have failed CI the same way
  `fanoCavity` initially did). See `validation/README.md` for details. The
  vendored `gts` package is no longer needed and was removed.
