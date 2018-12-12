#include <utility>
#include <vector>
#include <map>

#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <CTData.h>

// This macro prints all coincidence data to a csv
// No cuts are applied
//
// Format for csv (one row):
//  name, Q2, target,
//  hmsAngle, shmsAngle, hmsMomentum, shmsMomentum,
//  hDelta, pDelta, hBeta, pBeta,
//  hTheta, pTheta, W, eMiss, pMiss, pMissZ,
//  hCerNPE, hCalEtottracknorm, hCalEtot, hCalEprtrack, hCalEprtracknorm,
//  pHGCerNPE, pNGCerNpe, pCalEtottracknorm, pCalEprtracknorm,
//  epCoinTimeROC1, epCoinTimeROC2
void convert_to_csv() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");

    std::vector<TString> kinematics = {"LH2_Q2_8", "LH2_Q2_10", "LH2_Q2_12", "LH2_Q2_14",
                                       "C12_Q2_8", "C12_Q2_10", "C12_Q2_12", "C12_Q2_14"};

    // Loop over kinematics and print to CSVs
    for (auto &k: kinematics) {
        std::cout << k << std::endl;
        TChain* chain = data->GetChain(k);

        std::cout << "Entries: " << chain->GetEntries() << std::endl;

        // Data from the json database ---------------------------------------
        TString  name         = k;
        Double_t Q2           = data->GetQ2(k);
        TString  target       = data->GetTarget(k);

        Double_t hmsAngle     = data->GetHMSAngle(k);
        Double_t shmsAngle    = data->GetSHMSAngle(k);
        Double_t hmsMomentum  = data->GetHMSMomentum(k);
        Double_t shmsMomentum = data->GetSHMSMomentum(k);

        // Data from the rootfiles --------------------------------------------
        TTreeReader reader(chain);
        // Delta
        TTreeReaderValue<double> hDelta(reader, "H.gtr.dp");
        TTreeReaderValue<double> pDelta(reader, "P.gtr.dp");

        // Beta
        TTreeReaderValue<double> hBeta(reader, "H.gtr.beta");
        TTreeReaderValue<double> pBeta(reader, "P.gtr.beta");

        // Scattering angles in radians
        TTreeReaderValue<double> hTheta(reader, "H.kin.primary.scat_ang_rad");
        TTreeReaderValue<double> pTheta(reader, "P.kin.secondary.xangle");

        // Invariant mass, missing momentum
        TTreeReaderValue<double> W(reader,      "H.kin.primary.W");
        TTreeReaderValue<double> eMiss(reader,  "P.kin.secondary.emiss");
        TTreeReaderValue<double> pMiss(reader,  "P.kin.secondary.pmiss");
        TTreeReaderValue<double> pMissZ(reader, "P.kin.secondary.pmiss_z");

        // HMS PID
        TTreeReaderValue<double> hCerNPE(reader,           "H.cer.npeSum");
        TTreeReaderValue<double> hCalEtottracknorm(reader, "H.cal.etottracknorm");
        TTreeReaderValue<double> hCalEtot(reader,          "H.cal.etot");
        TTreeReaderValue<double> hCalEprtrack(reader,      "H.cal.eprtrack");
        TTreeReaderValue<double> hCalEprtracknorm(reader,  "H.cal.eprtracknorm");

        // SHMS PID
        TTreeReaderValue<double> pHGCerNPE(reader,         "P.hgcer.npeSum");
        TTreeReaderValue<double> pNGCerNPE(reader,         "P.ngcer.npeSum");
        TTreeReaderValue<double> pCalEtottracknorm(reader, "P.cal.etottracknorm");
        TTreeReaderValue<double> pCalEprtracknorm(reader,  "P.cal.eprtracknorm");

        // Coincidence time
        TTreeReaderValue<double> epCoinTimeROC1 (reader, "CTime.epCoinTime_ROC1");
        TTreeReaderValue<double> epCoinTimeROC2 (reader, "CTime.epCoinTime_ROC2");

        // Write to file ------------------------------------------------------
        TString filename = Form("/work/hallc/e1206107/jmatter/csv/%s.csv", k.Data());
        ofstream file;
        file.open(filename.Data());

        // header
        // std::cout << "name,"
        file << "name,"
             << "Q2,"
             << "target,"
             << "hmsAngle,"
             << "shmsAngle,"
             << "hmsMomentum,"
             << "shmsMomentum,"
             << "hDelta,"
             << "pDelta,"
             << "hBeta,"
             << "pBeta,"
             << "hTheta,"
             << "pTheta,"
             << "W,"
             << "eMiss,"
             << "pMiss,"
             << "pMissZ,"
             << "hCerNPE,"
             << "hCalEtottracknorm,"
             << "hCalEtot,"
             << "hCalEprtrack,"
             << "hCalEprtracknorm,"
             << "pHGCerNPE,"
             << "pNGCerNPE,"
             << "pCalEtottracknorm,"
             << "pCalEprtracknorm,"
             << "epCoinTimeROC1,"
             << "epCoinTimeROC2"
             << std::endl;

        // Go back to the first tree in the chain. GetEntries() previously moved it
        chain->LoadTree(-1);

        // Print each event
        int counter=0;
        while (reader.Next()) {
            if (counter%1000==0) { std::cout << counter << std::endl; }

            // std::cout << name
            file << name
                 << "," << Q2
                 << "," << target
                 << "," << hmsAngle
                 << "," << shmsAngle
                 << "," << hmsMomentum
                 << "," << shmsMomentum
                 << "," << *hDelta
                 << "," << *pDelta
                 << "," << *hBeta
                 << "," << *pBeta
                 << "," << *hTheta
                 << "," << *pTheta
                 << "," << *W
                 << "," << *eMiss
                 << "," << *pMiss
                 << "," << *pMissZ
                 << "," << *hCerNPE
                 << "," << *hCalEtottracknorm
                 << "," << *hCalEtot
                 << "," << *hCalEprtrack
                 << "," << *hCalEprtracknorm
                 << "," << *pHGCerNPE
                 << "," << *pNGCerNPE
                 << "," << *pCalEtottracknorm
                 << "," << *pCalEprtracknorm
                 << "," << *epCoinTimeROC1
                 << "," << *epCoinTimeROC2
                 << std::endl;

            counter++;
        }
        file.close();
    }
}
