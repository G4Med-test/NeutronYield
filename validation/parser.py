#!/usr/bin/env python

# NeutronYield portal parser: metadata() reads the macro, parse() the
# "Scorer_<angle>_deg" neutron-energy histograms written to the ROOT file
# named by the macro's own /analysis/setFileName.
#
# Ported from geant-config-generator/tests/geant4/NeutronYield/parser.py
# (gts.BaseParser + PyROOT, merging repeated-seed jobs) to the module-level
# metadata()/parse() API expected by ci-workflows/validation/export.py,
# reading ROOT with uproot (no PyROOT on the runner, no job-group merge:
# one macro now means one run, so the original's sum-of-NEVENTS-across-jobs
# normalization reduces to this one job's own NEVENTS).
import math
import os

from geantval import getJSON, one_command, single_run

TARGET_NAMES = {"Aluminum": "Al", "Carbon": "C", "Iron": "Fe", "G4_WATER": "Water"}


def metadata(commands):
    nevents = single_run(commands)
    filename = one_command(commands, "/analysis/setFileName")
    gps_particle = one_command(commands, "/gps/particle")
    if gps_particle == "ion":
        ion = one_command(commands, "/gps/ion").split()
        if ion[:2] != ["6", "12"]:
            raise ValueError("This validation is configured for C12 ions")
        particle = "C12"
    elif gps_particle == "proton":
        particle = "proton"
    else:
        raise ValueError("Unexpected /gps/particle: " + gps_particle)
    return {"TEST": "NeutronYield",
            "MODEL": one_command(commands, "/NeutronYield/PhysicsList/AddPhysics"),
            "MATERIAL": one_command(commands, "/NeutronYield/Geometry/Target/SetMaterial"),
            "PARTICLE": particle,
            "ENERGY": one_command(commands, "/gps/ene/mono"),
            "NEVENTS": nevents,
            "OUTPUT": filename if filename.endswith(".root") else filename + ".root"}


def deg2rad(angle):
    return angle * math.pi / 180.


def solid_angle(angle, acceptance):
    if angle in (0, 180):
        omega = (math.cos(deg2rad(angle)) - math.cos(deg2rad(2 * acceptance))) * 2. * math.pi
    else:
        omega = (math.cos(deg2rad(angle - acceptance)) - math.cos(deg2rad(angle + acceptance))) * 2. * math.pi
    return abs(omega)


def extract(hist):
    edges = hist.axis().edges(flow=False).tolist()
    values = hist.values(flow=False).tolist()
    errors = hist.errors(flow=False).tolist()
    return edges[:-1], edges[1:], values, errors


def parse(job):
    import uproot

    particle = job["PARTICLE"]
    target = TARGET_NAMES[job["MATERIAL"]]
    energy = float(job["ENERGY"].split()[0])

    filename = os.path.join(job["path"], job["OUTPUT"])
    with uproot.open(filename) as root:
        names = [k.split(";")[0] for k in root.keys(cycle=False, recursive=False)]
        for name in names:
            if particle == "C12" and "." in name:
                continue  # only the integer-angle histograms are filled for the C12 beam
            angle = float(name.split("_")[1])
            if particle == "C12":
                angle = angle * 15
                sa = 1.317e-3
            else:
                sa = solid_angle(angle, 2)

            binlow, binhigh, values, errors = extract(root[name])
            scale = 1.0 / float(job["NEVENTS"]) / sa
            binContent = [v * scale for v in values]
            binErrors = [e * scale for e in errors]

            yield getJSON(job, "histogram",
                          mctool_name="GEANT4",
                          mctool_model=job["MODEL"],
                          observableName="neutron yield",
                          targetName=target,
                          beamParticle=particle,
                          beamEnergies=[energy],
                          parameters=[{"names": "angle", "values": "%s deg" % angle}],
                          secondaryParticle="None",
                          title="NeutronYield Scorer",
                          xAxisName="Neutron energy (MeV)",
                          yAxisName="Neutron yield (n/Mev/sr/pr.particle)",
                          binEdgeLow=binlow,
                          binEdgeHigh=binhigh,
                          binContent=binContent,
                          yStatErrorsPlus=binErrors,
                          yStatErrorsMinus=binErrors)
