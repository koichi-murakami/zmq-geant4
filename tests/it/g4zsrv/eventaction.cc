/*============================================================================
Copyright 2017 Koichi Murakami

Distributed under the OSI-approved BSD License (the "License");
see accompanying file LICENSE for details.

This software is distributed WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the License for more information.
============================================================================*/
#include "G4Event.hh"
#include "eventaction.h"

// --------------------------------------------------------------------------
namespace {

void ShowProgress(int nprocessed) {
  G4cout << "[MESSAGE] event-loop check point: "
        << nprocessed << " events processed." << std::endl;
}

} // end of namespace

// --------------------------------------------------------------------------
EventAction::EventAction()
{
}

// --------------------------------------------------------------------------
EventAction::~EventAction()
{
}

// --------------------------------------------------------------------------
void EventAction::BeginOfEventAction(const G4Event* event)
{
}

// --------------------------------------------------------------------------
void EventAction::EndOfEventAction(const G4Event* event)
{
  int ievent = event-> GetEventID();
  const int kKiloEvents = 1000;

  if ( ievent % kKiloEvents == 0 && ievent != 0 ) {
    int event_in_mega = ievent / kKiloEvents;
    ::ShowProgress(ievent);
  }
}
