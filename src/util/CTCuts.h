#ifndef CTCUTS_H
#define CTCUTS_H

#include <map>

#include <TCut.h>
#include <TString.h>

class CTCuts {
    public:
        ~CTCuts();
        CTCuts();

        TCut GetCoinCut(TString target) { return fCoinCuts[target]; };

        // Tracking efficiency cuts
        TCut GetPTrackShouldCut()  { return fPTrackShould; }
        TCut GetPTrackDidCut()     { return fPTrackDid;    }
        TCut GetHTrackShouldCut()  { return fHTrackShould; }
        TCut GetHTrackDidCut()     { return fHTrackDid;    }

        // Cherenkov efficiency cuts
        TCut GetPCerShouldCut() { return fPCerShould; }
        TCut GetPCerDidCut()    { return fPCerDid;    }
        TCut GetHCerShouldCut() { return fHCerShould; }
        TCut GetHCerDidCut()    { return fHCerDid;    }

        // Calorimeter efficiency cuts
        TCut GetPCalShouldCut() { return fPCalShould; }
        TCut GetPCalDidCut()    { return fPCalDid;    }
        TCut GetHCalShouldCut() { return fHCalShould; }
        TCut GetHCalDidCut()    { return fHCalDid;    }

    private:
        // fCoinCuts is a map that takes a string specifying the target and returns a TCut
        std::map<TString, TCut> fCoinCuts;

        // Tracking efficiency cuts
        TCut fPTrackShould;
        TCut fPTrackDid;
        TCut fHTrackShould;
        TCut fHTrackDid;

        // Cherenkov efficiency cuts
        TCut fPCerShould;
        TCut fPCerDid;
        TCut fHCerShould;
        TCut fHCerDid;

        // Calorimeter efficiency cuts
        TCut fPCalShould;
        TCut fPCalDid;
        TCut fHCalShould;
        TCut fHCalDid;
};

#endif
