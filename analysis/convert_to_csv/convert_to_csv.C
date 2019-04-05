#include <utility>
#include <vector>
#include <map>

#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <CTData.h>

// This macro prints all coincidence data to a csv
// No cuts are applied
//
// Format for csv is on line 82
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
        TTreeReaderValue<double> hCalEtot(reader,          "H.cal.etot");
        TTreeReaderValue<double> hCalEtotnorm(reader,      "H.cal.etotnorm");
        TTreeReaderValue<double> hCalEtrack(reader,        "H.cal.etrack");
        TTreeReaderValue<double> hCalEtracknorm(reader,    "H.cal.etracknorm");
        TTreeReaderValue<double> hCalEprtrack(reader,      "H.cal.eprtrack");
        TTreeReaderValue<double> hCalEprtracknorm(reader,  "H.cal.eprtracknorm");
        TTreeReaderValue<double> hCalEtottracknorm(reader, "H.cal.etottracknorm");


        // SHMS PID
        TTreeReaderValue<double> pHGCerNPE(reader,         "P.hgcer.npeSum");
        TTreeReaderValue<double> pNGCerNPE(reader,         "P.ngcer.npeSum");
        TTreeReaderValue<double> pCalEtot(reader,          "P.cal.etot");
        TTreeReaderValue<double> pCalEtotnorm(reader,      "P.cal.etotnorm");
        TTreeReaderValue<double> pCalEtrack(reader,        "P.cal.etrack");
        TTreeReaderValue<double> pCalEtracknorm(reader,    "P.cal.etracknorm");
        TTreeReaderValue<double> pCalEprtrack(reader,      "P.cal.eprtrack");
        TTreeReaderValue<double> pCalEprtracknorm(reader,  "P.cal.eprtracknorm");
        TTreeReaderValue<double> pCalEtottracknorm(reader, "P.cal.etottracknorm");

        // Coincidence time
        TTreeReaderValue<double> epCoinTimeROC1 (reader, "CTime.epCoinTime_ROC1");
        TTreeReaderValue<double> epCoinTimeROC2 (reader, "CTime.epCoinTime_ROC2");

        // SHMS DC and hodo variables
        TTreeReaderValue<double> pHodBetanotrack(reader, "P.hod.betanotrack");
        TTreeReaderValue<double> pHodGoodscinhit(reader, "P.hod.goodscinhit");
        TTreeReaderValue<double> pDC1x1nhit(reader, "P.dc.1x1.nhit");
        TTreeReaderValue<double> pDC1u2nhit(reader, "P.dc.1u2.nhit");
        TTreeReaderValue<double> pDC1u1nhit(reader, "P.dc.1u1.nhit");
        TTreeReaderValue<double> pDC1v1nhit(reader, "P.dc.1v1.nhit");
        TTreeReaderValue<double> pDC1x2nhit(reader, "P.dc.1x2.nhit");
        TTreeReaderValue<double> pDC1v2nhit(reader, "P.dc.1v2.nhit");
        TTreeReaderValue<double> pDC2x1nhit(reader, "P.dc.2x1.nhit");
        TTreeReaderValue<double> pDC2u2nhit(reader, "P.dc.2u2.nhit");
        TTreeReaderValue<double> pDC2u1nhit(reader, "P.dc.2u1.nhit");
        TTreeReaderValue<double> pDC2v1nhit(reader, "P.dc.2v1.nhit");
        TTreeReaderValue<double> pDC2x2nhit(reader, "P.dc.2x2.nhit");
        TTreeReaderValue<double> pDC2v2nhit(reader, "P.dc.2v2.nhit");
        TTreeReaderValue<int>    pDCntrack(reader,  "P.dc.ntrack");

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
             << "hCalEtot,"
             << "hCalEtotnorm,"
             << "hCalEtrack,"
             << "hCalEtracknorm,"
             << "hCalEprtrack,"
             << "hCalEprtracknorm,"
             << "hCalEtottracknorm,"
             << "pHGCerNPE,"
             << "pNGCerNPE,"
             << "pCalEtot,"
             << "pCalEtotnorm,"
             << "pCalEtrack,"
             << "pCalEtracknorm,"
             << "pCalEprtrack,"
             << "pCalEprtracknorm,"
             << "pCalEtottracknorm,"
             << "epCoinTimeROC1,"
             << "epCoinTimeROC2,"
             << "pHodBetanotrack,"
             << "pHodGoodscinhit,"
             << "pDC1x1nhit,"
             << "pDC1u2nhit,"
             << "pDC1u1nhit,"
             << "pDC1v1nhit,"
             << "pDC1x2nhit,"
             << "pDC1v2nhit,"
             << "pDC2x1nhit,"
             << "pDC2u2nhit,"
             << "pDC2u1nhit,"
             << "pDC2v1nhit,"
             << "pDC2x2nhit,"
             << "pDC2v2nhit,"
             << "pDCntrack"
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
                 << "," << *hCalEtot
                 << "," << *hCalEtotnorm
                 << "," << *hCalEtrack
                 << "," << *hCalEtracknorm
                 << "," << *hCalEprtrack
                 << "," << *hCalEprtracknorm
                 << "," << *hCalEtottracknorm
                 << "," << *pHGCerNPE
                 << "," << *pNGCerNPE
                 << "," << *pCalEtot
                 << "," << *pCalEtotnorm
                 << "," << *pCalEtrack
                 << "," << *pCalEtracknorm
                 << "," << *pCalEprtrack
                 << "," << *pCalEprtracknorm
                 << "," << *pCalEtottracknorm
                 << "," << *epCoinTimeROC1
                 << "," << *epCoinTimeROC2
                 << "," << *pHodBetanotrack
                 << "," << *pHodGoodscinhit
                 << "," << *pDC1x1nhit
                 << "," << *pDC1u2nhit
                 << "," << *pDC1u1nhit
                 << "," << *pDC1v1nhit
                 << "," << *pDC1x2nhit
                 << "," << *pDC1v2nhit
                 << "," << *pDC2x1nhit
                 << "," << *pDC2u2nhit
                 << "," << *pDC2u1nhit
                 << "," << *pDC2v1nhit
                 << "," << *pDC2x2nhit
                 << "," << *pDC2v2nhit
                 << "," << *pDCntrack
                 << std::endl;

            counter++;
        }
        file.close();
    }
}
