/*============================================================================
Copyright 2017 Koichi Murakami

Distributed under the OSI-approved BSD License (the "License");
see accompanying file LICENSE for details.

This software is distributed WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the License for more information.
============================================================================*/
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "runaction.h"
#include "simdata.h"

// --------------------------------------------------------------------------
namespace {

} // end of namespace

// --------------------------------------------------------------------------
RunAction::RunAction()
  : simdata_(nullptr)
{
}

// --------------------------------------------------------------------------
RunAction::~RunAction()
{
}

// --------------------------------------------------------------------------
void RunAction::BeginOfRunAction(const G4Run*)
{
  simdata_-> Initialize();
}

// --------------------------------------------------------------------------
void RunAction::EndOfRunAction(const G4Run* run)
{
  ShowRunSummary(run);
}

// --------------------------------------------------------------------------
void RunAction::ShowRunSummary(const G4Run* run)
{
  // # of processed events
  int nevents_to_be = run-> GetNumberOfEventToBeProcessed();
  int nevents = run-> GetNumberOfEvent();

  // Edep information
  double edep_cal = simdata_-> GetEdep() / nevents / MeV;

  // steo count
  double spe = simdata_-> GetStepCount() / (double)nevents;

  G4cout << G4endl;
  G4cout << "=============================================================="
            << G4endl;
  G4cout << " Run Summary" << G4endl
            << " - # events processd = " << nevents << " / "
            << nevents_to_be << G4endl
            << " *** Physics regression ***" << G4endl
            << " - edep in cal per event = " << edep_cal << " MeV/event"
            << G4endl
            << " - steps per event = " << spe << " steps/event"
            << G4endl;
  G4cout << "=============================================================="
            << G4endl << G4endl;
}
