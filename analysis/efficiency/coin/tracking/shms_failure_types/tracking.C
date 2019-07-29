#include <utility>
#include <fstream>
#include <vector>
#include <map>

#include <TH1.h>
#include <TCut.h>
#include <TCanvas.h>

#include <CTData.h>
#include <CTCuts.h>

void tracking() {
    // ------------------------------------------------------------------------
    // Load our data and cuts
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Which kinematics
    std::vector<TString> kinematics = data->GetNames();

    // Save
    TString csvFilename = Form("shms_tracking_failures.csv");

    TString drawFormat = "P.hod.goodscinhit>>%s(2,0,2)";

    TCut dc1nonzero = "(P.dc.1x1.nhit + P.dc.1u2.nhit + P.dc.1u1.nhit + P.dc.1v1.nhit + P.dc.1x2.nhit + P.dc.1v2.nhit) > 0";
    TCut dc2nonzero = "(P.dc.2x1.nhit + P.dc.2u2.nhit + P.dc.2u1.nhit + P.dc.2v1.nhit + P.dc.2x2.nhit + P.dc.2v2.nhit) > 0";

    // ------------------------------------------------------------------------
    // Calculate and print

    // Open file
    std::ofstream ofs;
    ofs.open(csvFilename.Data());

    // Print header
    TString printme = Form("kinematics,target,Q2,collimator,cut,should,did,ratio");
    ofs << printme << std::endl;
    std::cout << printme << std::endl;

    TCut shouldCut, didCut;
    TChain *chain;
    TString shouldHistoName, didHistoName;
    TString drawMe, printMe, name;
    TH1 *hDid, *hShould;
    Int_t nDid, nShould;
    // Loop over kinematics
    for (auto const &k : kinematics)   {
        chain = data->GetChain(k);

        // --------------------------------------
        // "Regular" should/did
        name = "BothHitsLessThan21";
        shouldCut = cuts->Get("pScinShouldh");
        didCut    = shouldCut && "P.dc.ntrack>0";
        shouldHistoName = Form("h_%s_should_%s", name.Data(), k.Data());
        didHistoName = Form("h_%s_did_%s", name.Data(), k.Data());
        // Draw should
        drawMe = Form(drawFormat.Data(), shouldHistoName.Data());
        chain->Draw(drawMe.Data(), shouldCut, "goff");
        hShould = (TH1*) gDirectory->Get(shouldHistoName);
        // Draw did
        drawMe = Form(drawFormat.Data(), didHistoName.Data());
        chain->Draw(drawMe.Data(), didCut, "goff");
        hDid = (TH1*) gDirectory->Get(didHistoName);
        // Get counts
        nShould = hShould->Integral(1,2);
        nDid = hDid->Integral(1,2);
        printme = Form("%s,%s,%.1f,%s,%s,%d,%d,%f",
                        k.Data(), data->GetTarget(k).Data(), data->GetQ2(k), data->GetCollimator(k).Data(),
                        name.Data(), nShould, nDid, double(nDid)/nShould);
        ofs << printme << std::endl;
        std::cout << printme << std::endl;

        // --------------------------------------
        // should/did for zero hits in both
        name = "ZeroHitsBoth";
        shouldCut = cuts->Get("pScinShouldh") && !dc1nonzero && !dc2nonzero;
        didCut    = shouldCut && "P.dc.ntrack>0";
        shouldHistoName = Form("h_%s_should_%s", name.Data(), k.Data());
        didHistoName = Form("h_%s_did_%s", name.Data(), k.Data());
        // Draw should
        drawMe = Form(drawFormat.Data(), shouldHistoName.Data());
        chain->Draw(drawMe.Data(), shouldCut, "goff");
        hShould = (TH1*) gDirectory->Get(shouldHistoName);
        // Draw did
        drawMe = Form(drawFormat.Data(), didHistoName.Data());
        chain->Draw(drawMe.Data(), didCut, "goff");
        hDid = (TH1*) gDirectory->Get(didHistoName);
        // Get counts
        nShould = hShould->Integral(1,2);
        nDid = hDid->Integral(1,2);
        printme = Form("%s,%s,%.1f,%s,%s,%d,%d,%f",
                        k.Data(), data->GetTarget(k).Data(), data->GetQ2(k), data->GetCollimator(k).Data(),
                        name.Data(), nShould, nDid, double(nDid)/nShould);
        ofs << printme << std::endl;
        std::cout << printme << std::endl;

        // --------------------------------------
        // should/did for nonzero hits in both
        name = "NonzeroHitsBoth";
        shouldCut = cuts->Get("pScinShouldh") && dc1nonzero && dc2nonzero;
        didCut    = shouldCut && "P.dc.ntrack>0";
        shouldHistoName = Form("h_%s_should_%s", name.Data(), k.Data());
        didHistoName = Form("h_%s_did_%s", name.Data(), k.Data());
        // Draw should
        drawMe = Form(drawFormat.Data(), shouldHistoName.Data());
        chain->Draw(drawMe.Data(), shouldCut, "goff");
        hShould = (TH1*) gDirectory->Get(shouldHistoName);
        // Draw did
        drawMe = Form(drawFormat.Data(), didHistoName.Data());
        chain->Draw(drawMe.Data(), didCut, "goff");
        hDid = (TH1*) gDirectory->Get(didHistoName);
        // Get counts
        nShould = hShould->Integral(1,2);
        nDid = hDid->Integral(1,2);
        printme = Form("%s,%s,%.1f,%s,%s,%d,%d,%f",
                        k.Data(), data->GetTarget(k).Data(), data->GetQ2(k), data->GetCollimator(k).Data(),
                        name.Data(), nShould, nDid, double(nDid)/nShould);
        ofs << printme << std::endl;
        std::cout << printme << std::endl;

        // --------------------------------------
        // should/did for nonzero hits in DC1
        name = "NonzeroHitsDC1";
        shouldCut = cuts->Get("pScinShouldh") && dc1nonzero && !dc2nonzero;
        didCut    = shouldCut && "P.dc.ntrack>0";
        shouldHistoName = Form("h_%s_should_%s", name.Data(), k.Data());
        didHistoName = Form("h_%s_did_%s", name.Data(), k.Data());
        // Draw should
        drawMe = Form(drawFormat.Data(), shouldHistoName.Data());
        chain->Draw(drawMe.Data(), shouldCut, "goff");
        hShould = (TH1*) gDirectory->Get(shouldHistoName);
        // Draw did
        drawMe = Form(drawFormat.Data(), didHistoName.Data());
        chain->Draw(drawMe.Data(), didCut, "goff");
        hDid = (TH1*) gDirectory->Get(didHistoName);
        // Get counts
        nShould = hShould->Integral(1,2);
        nDid = hDid->Integral(1,2);
        printme = Form("%s,%s,%.1f,%s,%s,%d,%d,%f",
                        k.Data(), data->GetTarget(k).Data(), data->GetQ2(k), data->GetCollimator(k).Data(),
                        name.Data(), nShould, nDid, double(nDid)/nShould);
        ofs << printme << std::endl;
        std::cout << printme << std::endl;

        // --------------------------------------
        // should/did for nonzero hits in DC2
        name = "NonzeroHitsDC2";
        shouldCut = cuts->Get("pScinShouldh") && !dc1nonzero && dc2nonzero;
        didCut    = shouldCut && "P.dc.ntrack>0";
        shouldHistoName = Form("h_%s_should_%s", name.Data(), k.Data());
        didHistoName = Form("h_%s_did_%s", name.Data(), k.Data());
        // Draw should
        drawMe = Form(drawFormat.Data(), shouldHistoName.Data());
        chain->Draw(drawMe.Data(), shouldCut, "goff");
        hShould = (TH1*) gDirectory->Get(shouldHistoName);
        // Draw did
        drawMe = Form(drawFormat.Data(), didHistoName.Data());
        chain->Draw(drawMe.Data(), didCut, "goff");
        hDid = (TH1*) gDirectory->Get(didHistoName);
        // Get counts
        nShould = hShould->Integral(1,2);
        nDid = hDid->Integral(1,2);
        printme = Form("%s,%s,%.1f,%s,%s,%d,%d,%f",
                        k.Data(), data->GetTarget(k).Data(), data->GetQ2(k), data->GetCollimator(k).Data(),
                        name.Data(), nShould, nDid, double(nDid)/nShould);
        ofs << printme << std::endl;
        std::cout << printme << std::endl;
    }
    ofs.close();
}
