#include <utility>
#include <vector>
#include <map>

#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <CTData.h>

// This macro prints all coincidence data to a csv
// No cuts are applied
//
// Format for csv is on line 114
void convert_to_csv() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_pass3.json");

    std::vector<TString> kinematics = data->GetNames();
    // std::vector<TString> kinematics = {"LH2_Q2_8", "LH2_Q2_10", "LH2_Q2_12", "LH2_Q2_14",
    //                                    "C12_Q2_8", "C12_Q2_10", "C12_Q2_12", "C12_Q2_14"};

    // Loop over kinematics and print to CSVs
    for (auto &k: kinematics) {
        std::cout << k << std::endl;
        TChain* chain = data->GetChain(k);


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

        // SHMS Tracking
        TTreeReaderValue<double> pGtrX(reader,    "P.gtr.x");
        TTreeReaderValue<double> pGtrY(reader,    "P.gtr.y");
        TTreeReaderValue<double> pGtrTh(reader,   "P.gtr.th");
        TTreeReaderValue<double> pGtrPh(reader,   "P.gtr.ph");
        TTreeReaderValue<double> pGtrXfp(reader,  "P.gtr.x_fp");
        TTreeReaderValue<double> pGtrYfp(reader,  "P.gtr.y_fp");
        TTreeReaderValue<double> pGtrXpfp(reader, "P.gtr.xp_fp");
        TTreeReaderValue<double> pGtrYpfp(reader, "P.gtr.yp_fp");

        TTreeReaderValue<double> pTrPruneSelect(reader, "P.tr.PruneSelect");

        // HMS Tracking
        TTreeReaderValue<double> hGtrX(reader,    "H.gtr.x");
        TTreeReaderValue<double> hGtrY(reader,    "H.gtr.y");
        TTreeReaderValue<double> hGtrTh(reader,   "H.gtr.th");
        TTreeReaderValue<double> hGtrPh(reader,   "H.gtr.ph");
        TTreeReaderValue<double> hGtrXfp(reader,  "H.gtr.x_fp");
        TTreeReaderValue<double> hGtrYfp(reader,  "H.gtr.y_fp");
        TTreeReaderValue<double> hGtrXpfp(reader, "H.gtr.xp_fp");
        TTreeReaderValue<double> hGtrYpfp(reader, "H.gtr.yp_fp");

        TTreeReaderValue<double> hTrPruneSelect(reader, "H.tr.PruneSelect");

        //THcReactionPoint
        TTreeReaderValue<double> reactOK(reader, "P.react.ok");
        TTreeReaderValue<double> reactX(reader,  "P.react.x");
        TTreeReaderValue<double> reactY(reader,  "P.react.y");
        TTreeReaderValue<double> reactZ(reader,  "P.react.z");

        //THcRasteredBeam
        TTreeReaderValue<double> rbX(reader,  "P.rb.x");
        TTreeReaderValue<double> rbY(reader,  "P.rb.y");
        TTreeReaderValue<double> rbXp(reader, "P.rb.th");
        TTreeReaderValue<double> rbYp(reader, "P.rb.ph");
        TTreeReaderValue<double> rbPx(reader, "P.rb.px");
        TTreeReaderValue<double> rbPy(reader, "P.rb.py");
        TTreeReaderValue<double> rbPz(reader, "P.rb.pz");

        // Scattering angles in radians
        TTreeReaderValue<double> hTheta(reader, "H.kin.primary.scat_ang_rad");
        TTreeReaderValue<double> pTheta(reader, "P.kin.secondary.xangle");

        // Invariant mass, missing momentum
        TTreeReaderValue<double> W(reader,      "H.kin.primary.W");
        TTreeReaderValue<double> eMiss(reader,  "P.kin.secondary.emiss");
        TTreeReaderValue<double> eMissNuc(reader,  "P.kin.secondary.emiss_nuc");
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

        TTreeReaderValue<double> hCalE1(reader, "H.cal.1pr.eplane");
        TTreeReaderValue<double> hCalE2(reader, "H.cal.2ta.eplane");
        TTreeReaderValue<double> hCalE3(reader, "H.cal.3ta.eplane");
        TTreeReaderValue<double> hCalE4(reader, "H.cal.4ta.eplane");


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
        TTreeReaderValue<double> pHodBetanotrack(reader,   "P.hod.betanotrack");
        TTreeReaderValue<double> pHodGoodscinhit(reader,   "P.hod.goodscinhit");
        TTreeReaderValue<double> pInsideDipoleExit(reader, "P.hod.InsideDipoleExit");
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
        TTreeReaderValue<double> pDCntrack(reader,  "P.dc.ntrack");

        // HMS DC and hodo variables
        TTreeReaderValue<double> hHodBetanotrack(reader,   "H.hod.betanotrack");
        TTreeReaderValue<double> hHodGoodscinhit(reader,   "H.hod.goodscinhit");
        TTreeReaderValue<double> hInsideDipoleExit(reader, "H.hod.InsideDipoleExit");
        TTreeReaderValue<double> hDC1x1nhit(reader, "H.dc.1x1.nhit");
        TTreeReaderValue<double> hDC1u2nhit(reader, "H.dc.1u2.nhit");
        TTreeReaderValue<double> hDC1u1nhit(reader, "H.dc.1u1.nhit");
        TTreeReaderValue<double> hDC1v1nhit(reader, "H.dc.1v1.nhit");
        TTreeReaderValue<double> hDC1x2nhit(reader, "H.dc.1x2.nhit");
        TTreeReaderValue<double> hDC1v2nhit(reader, "H.dc.1v2.nhit");
        TTreeReaderValue<double> hDC2x1nhit(reader, "H.dc.2x1.nhit");
        TTreeReaderValue<double> hDC2u2nhit(reader, "H.dc.2u2.nhit");
        TTreeReaderValue<double> hDC2u1nhit(reader, "H.dc.2u1.nhit");
        TTreeReaderValue<double> hDC2v1nhit(reader, "H.dc.2v1.nhit");
        TTreeReaderValue<double> hDC2x2nhit(reader, "H.dc.2x2.nhit");
        TTreeReaderValue<double> hDC2v2nhit(reader, "H.dc.2v2.nhit");
        TTreeReaderValue<double> hDCntrack(reader,  "H.dc.ntrack");

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
             << "pGtrX,"
             << "pGtrY,"
             << "pGtrTh,"
             << "pGtrPh,"
             << "pGtrXfp,"
             << "pGtrYfp,"
             << "pGtrXpfp,"
             << "pGtrYpfp,"
             << "pTrPruneSelect,"
             << "hGtrX,"
             << "hGtrY,"
             << "hGtrTh,"
             << "hGtrPh,"
             << "hGtrXfp,"
             << "hGtrYfp,"
             << "hGtrXpfp,"
             << "hGtrYpfp,"
             << "hTrPruneSelect,"
             << "reactOK,"
             << "reactX,"
             << "reactY,"
             << "reactZ,"
             << "rbX,"
             << "rbY,"
             << "rbXp,"
             << "rbYp,"
             << "rbPx,"
             << "rbPy,"
             << "rbPz,"
             << "hTheta,"
             << "pTheta,"
             << "W,"
             << "eMiss,"
             << "eMissNuc,"
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
             << "hCalE1,"
             << "hCalE2,"
             << "hCalE3,"
             << "hCalE4,"
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
             << "pInsideDipoleExit,"
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
             << "pDCntrack,"
             << "hHodBetanotrack,"
             << "hHodGoodscinhit,"
             << "hInsideDipoleExit,"
             << "hDC1x1nhit,"
             << "hDC1u2nhit,"
             << "hDC1u1nhit,"
             << "hDC1v1nhit,"
             << "hDC1x2nhit,"
             << "hDC1v2nhit,"
             << "hDC2x1nhit,"
             << "hDC2u2nhit,"
             << "hDC2u1nhit,"
             << "hDC2v1nhit,"
             << "hDC2x2nhit,"
             << "hDC2v2nhit,"
             << "hDCntrack"
             << std::endl;

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
                 << "," << *pGtrX
                 << "," << *pGtrY
                 << "," << *pGtrTh
                 << "," << *pGtrPh
                 << "," << *pGtrXfp
                 << "," << *pGtrYfp
                 << "," << *pGtrXpfp
                 << "," << *pGtrYpfp
                 << "," << *pTrPruneSelect
                 << "," << *hGtrX
                 << "," << *hGtrY
                 << "," << *hGtrTh
                 << "," << *hGtrPh
                 << "," << *hGtrXfp
                 << "," << *hGtrYfp
                 << "," << *hGtrXpfp
                 << "," << *hGtrYpfp
                 << "," << *hTrPruneSelect
                 << "," << *reactOK
                 << "," << *reactX
                 << "," << *reactY
                 << "," << *reactZ
                 << "," << *rbX
                 << "," << *rbY
                 << "," << *rbXp
                 << "," << *rbYp
                 << "," << *rbPx
                 << "," << *rbPy
                 << "," << *rbPz
                 << "," << *hTheta
                 << "," << *pTheta
                 << "," << *W
                 << "," << *eMiss
                 << "," << *eMissNuc
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
                 << "," << *hCalE1
                 << "," << *hCalE2
                 << "," << *hCalE3
                 << "," << *hCalE4
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
                 << "," << *pInsideDipoleExit
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
                 << "," << *hHodBetanotrack
                 << "," << *hHodGoodscinhit
                 << "," << *hInsideDipoleExit
                 << "," << *hDC1x1nhit
                 << "," << *hDC1u2nhit
                 << "," << *hDC1u1nhit
                 << "," << *hDC1v1nhit
                 << "," << *hDC1x2nhit
                 << "," << *hDC1v2nhit
                 << "," << *hDC2x1nhit
                 << "," << *hDC2u2nhit
                 << "," << *hDC2u1nhit
                 << "," << *hDC2v1nhit
                 << "," << *hDC2x2nhit
                 << "," << *hDC2v2nhit
                 << "," << *hDCntrack
                 << std::endl;

            counter++;
        }
        file.close();

        std::cout << "Entries written: " << counter << std::endl;
    }
}
