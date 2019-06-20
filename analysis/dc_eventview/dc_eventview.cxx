#include <utility>
#include <vector>
#include <limits>
#include <map>

#include <TF1.h>
#include <TCut.h>
#include <TChain.h>
#include <TH1F.h>
#include <TROOT.h>
#include <TRandom.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TSystem.h>
#include <TView.h>
#include <TPad.h>

#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>

#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TTreeReaderValue.h>

#include "Plane.h"

// GOAL: Plot hits in DC along with track found by hcana

TString waitForInput(TString prompt)
{
    std::cout << prompt << std::flush;
    // std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    TString temp;
    std::cin >> temp;

    return temp;
}


void StandaloneApplication(int argc, char** argv) {
    TCanvas *c1 = new TCanvas("c1","c1",20,40,600,800);
    TView *view = TView::CreateView(1);
    view->SetRange(-50,-50,-50,50,50,50);
    view->ShowAxis();

    // ------------------------------------------------------------
    TChain *chain = new TChain("T");
    chain->Add(argv[1]);

    gStyle->SetOptStat(0);

    // ------------------------------------------------------------
    // Geometry from hallc_replay/PARAM/SHMS/DC/pdc_geom.param

    Double_t cminch = 2.54;
    Double_t raddeg = 3.1415926535/180.0;

    // This is the correct order that particles pass through
    std::vector<TString> DC1planes = {"1u1", "1u2", "1x1", "1x2", "1v1", "1v2"};
    std::vector<TString> DC2planes = {"2v2", "2v1", "2x2", "2x1", "2u2", "2u1"};
    std::vector<TString> planes;
    planes.insert(planes.end(), DC1planes.begin(), DC1planes.end());
    planes.insert(planes.end(), DC2planes.begin(), DC2planes.end());

    std::map<TString, double> pdc_xcenter, pdc_ycenter, pdc_zpos,
                              pdc_roll, pdc_pitch, pdc_yaw;

    std::map<TString, double> pdc_plane_xpos, pdc_plane_ypos, pdc_plane_zpos,
                              pdc_plane_alpha, pdc_plane_beta, pdc_plane_gamma,
                              pdc_central_wire, pdc_nrwire;

    // wire spacing in cm
    Double_t pitch = 1.0;

    // Chamber positions and orientations
    // Distances in cm, angles in degrees
    pdc_xcenter["dc1"] = -.036;
    pdc_xcenter["dc2"] = 0.008;
    pdc_ycenter["dc1"] = 0.019-0.429871;
    pdc_ycenter["dc2"] = 0.013-0.429871;
    pdc_zpos["dc1"]    = -40 - 0.656;
    pdc_zpos["dc2"]    =  40 - 0.668;
    pdc_roll["dc1"]    = 180.0;
    pdc_roll["dc2"]    = 180.0;
    pdc_pitch["dc1"]   = -0.006;
    pdc_pitch["dc2"]   =  0.001;
    pdc_yaw["dc1"]     = -0.006;
    pdc_yaw["dc2"]     = -0.035;

    // number of wires
    pdc_nrwire["1u1"] = 107;
    pdc_nrwire["1u2"] = 107;
    pdc_nrwire["1x1"] = 79;
    pdc_nrwire["1x2"] = 79;
    pdc_nrwire["1v1"] = 107;
    pdc_nrwire["1v2"] = 107;
    pdc_nrwire["2v2"] = 107;
    pdc_nrwire["2v1"] = 107;
    pdc_nrwire["2x2"] = 79;
    pdc_nrwire["2x1"] = 79;
    pdc_nrwire["2u2"] = 107;
    pdc_nrwire["2u1"] = 107;

    // planes' central posititons in cm
    pdc_plane_xpos["1u1"] = pdc_xcenter["dc1"]-0.006;
    pdc_plane_xpos["1u2"] = pdc_xcenter["dc1"]-0.018;
    pdc_plane_xpos["1x1"] = pdc_xcenter["dc1"];
    pdc_plane_xpos["1x2"] = pdc_xcenter["dc1"];
    pdc_plane_xpos["1v1"] = pdc_xcenter["dc1"];
    pdc_plane_xpos["1v2"] = pdc_xcenter["dc1"]-0.015;
    pdc_plane_xpos["2v2"] = pdc_xcenter["dc2"]-0.012;
    pdc_plane_xpos["2v1"] = pdc_xcenter["dc2"]+0.014;
    pdc_plane_xpos["2x2"] = pdc_xcenter["dc2"];
    pdc_plane_xpos["2x1"] = pdc_xcenter["dc2"];
    pdc_plane_xpos["2u2"] = pdc_xcenter["dc2"]-0.01;
    pdc_plane_xpos["2u1"] = pdc_xcenter["dc2"]+0.001;

    pdc_plane_ypos["1u1"] = pdc_ycenter["dc1"]-0.009;
    pdc_plane_ypos["1u2"] = pdc_ycenter["dc1"]-0.017;
    pdc_plane_ypos["1x1"] = pdc_ycenter["dc1"];
    pdc_plane_ypos["1x2"] = pdc_ycenter["dc1"];
    pdc_plane_ypos["1v1"] = pdc_ycenter["dc1"];
    pdc_plane_ypos["1v2"] = pdc_ycenter["dc1"]+0.015;
    pdc_plane_ypos["2v2"] = pdc_ycenter["dc2"]-0.012;
    pdc_plane_ypos["2v1"] = pdc_ycenter["dc2"]+0.018;
    pdc_plane_ypos["2x2"] = pdc_ycenter["dc2"];
    pdc_plane_ypos["2x1"] = pdc_ycenter["dc2"];
    pdc_plane_ypos["2u2"] = pdc_ycenter["dc2"]-0.003;
    pdc_plane_ypos["2u1"] = pdc_ycenter["dc2"]-0.001;

    pdc_plane_zpos["1u1"] = pdc_zpos["dc1"] - 0.68701*cminch;
    pdc_plane_zpos["1u2"] = pdc_zpos["dc1"] - 0.43701*cminch;
    pdc_plane_zpos["1x1"] = pdc_zpos["dc1"] - 0.18701*cminch;
    pdc_plane_zpos["1x2"] = pdc_zpos["dc1"] + 0.18701*cminch;
    pdc_plane_zpos["1v1"] = pdc_zpos["dc1"] + 0.43701*cminch;
    pdc_plane_zpos["1v2"] = pdc_zpos["dc1"] + 0.68701*cminch;
    pdc_plane_zpos["2v2"] = pdc_zpos["dc2"] - 0.68701*cminch;
    pdc_plane_zpos["2v1"] = pdc_zpos["dc2"] - 0.43701*cminch;
    pdc_plane_zpos["2x2"] = pdc_zpos["dc2"] - 0.18701*cminch;
    pdc_plane_zpos["2x1"] = pdc_zpos["dc2"] + 0.18701*cminch;
    pdc_plane_zpos["2u2"] = pdc_zpos["dc2"] + 0.43701*cminch;
    pdc_plane_zpos["2u1"] = pdc_zpos["dc2"] + 0.68701*cminch;

    // Wire angles with respect to the x direction
    pdc_plane_alpha["1u1"] = ( 30.0 + pdc_roll["dc1"])*raddeg;
    pdc_plane_alpha["1u2"] = ( 30.0 + pdc_roll["dc1"])*raddeg;
    pdc_plane_alpha["1x1"] = ( 90.0 + pdc_roll["dc1"])*raddeg;
    pdc_plane_alpha["1x2"] = ( 90.0 + pdc_roll["dc1"])*raddeg;
    pdc_plane_alpha["1v1"] = (150.0 + pdc_roll["dc1"])*raddeg;
    pdc_plane_alpha["1v2"] = (150.0 + pdc_roll["dc1"])*raddeg;
    pdc_plane_alpha["2v2"] = ( 30.0 + pdc_roll["dc2"])*raddeg;
    pdc_plane_alpha["2v1"] = ( 30.0 + pdc_roll["dc2"])*raddeg;
    pdc_plane_alpha["2x2"] = ( 90.0 + pdc_roll["dc2"])*raddeg;
    pdc_plane_alpha["2x1"] = ( 90.0 + pdc_roll["dc2"])*raddeg;
    pdc_plane_alpha["2u2"] = (150.0 + pdc_roll["dc2"])*raddeg;
    pdc_plane_alpha["2u1"] = (150.0 + pdc_roll["dc2"])*raddeg;

    for (auto const &p : planes) {
        TString thisDC = Form("dc%c", p[0]);
        pdc_plane_beta[p]  = pdc_pitch[thisDC]*raddeg;
        pdc_plane_gamma[p] = pdc_yaw[thisDC]*raddeg;
    }

    // Central wires
    pdc_central_wire["1u1"] = 54.25;
    pdc_central_wire["1u2"] = 53.75;
    pdc_central_wire["1x1"] = 40.25;
    pdc_central_wire["1x2"] = 39.75;
    pdc_central_wire["1v1"] = 54.25;
    pdc_central_wire["1v2"] = 53.75;
    pdc_central_wire["2v2"] = 53.75;
    pdc_central_wire["2v1"] = 54.25;
    pdc_central_wire["2x2"] = 39.75;
    pdc_central_wire["2x1"] = 40.25;
    pdc_central_wire["2u2"] = 53.75;
    pdc_central_wire["2u1"] = 54.25;

    // ------------------------------------------------------------
    // Create planes in display
    std::map<TString, Plane*> planes3D;

    for (auto const& plane: planes) {
        planes3D[plane] = new Plane();

        planes3D[plane]->SetCenter(pdc_plane_xpos[plane],
                                   pdc_plane_ypos[plane],
                                   pdc_plane_zpos[plane]);

        planes3D[plane]->SetTilt(pdc_plane_alpha[plane],
                                 pdc_plane_beta[plane],
                                 pdc_plane_gamma[plane]);

        planes3D[plane]->SetSize(100, 100);

        planes3D[plane]->SetWires(pdc_nrwire[plane], pitch, pdc_central_wire[plane]);

        planes3D[plane]->Populate();

        planes3D[plane]->Draw();
    }
    c1->Update();
    c1->Draw();
    gPad->Modified(); gPad->Update(); gSystem->ProcessEvents();


    // // ------------------------------------------------------------
    // Set up TTreeReader
    TTreeReader reader(chain);

    std::map<TString, TTreeReaderArray<double>*> wireno;
    std::map<TString, TTreeReaderValue<int>*> wirenoN;
    std::map<TString, TTreeReaderValue<double>*> nhit;

    TTreeReaderValue<double> pHodBetanotrack(reader, "P.hod.betanotrack");
    TTreeReaderValue<double> pHodGoodscinhit(reader, "P.hod.goodscinhit");
    TTreeReaderValue<double> pNGCer(reader, "P.ngcer.npeSum");

    TTreeReaderValue<double> pPrE(reader, "P.cal.pr.eplane");
    TTreeReaderValue<double> pShE(reader, "P.cal.fly.earray");
    TTreeReaderValue<double> pEtot(reader, "P.cal.etot");
    TTreeReaderValue<double> pEtotnorm(reader, "P.cal.etotnorm");

    TTreeReaderValue<double> pNtrack(reader, "P.dc.ntrack");

    TTreeReaderValue<double> xfp(reader,  "P.dc.x_fp");
    TTreeReaderValue<double> xpfp(reader, "P.dc.xp_fp");
    TTreeReaderValue<double> yfp(reader,  "P.dc.y_fp");
    TTreeReaderValue<double> ypfp(reader, "P.dc.yp_fp");

    // Wire number
    for (auto const &p : planes) {
        wireno[p] = new TTreeReaderArray<double>(reader, Form("P.dc.%s.wirenum", p.Data()));
    }

    // Number of hits per plane
    for (auto const &p : planes) {
        wirenoN[p] = new TTreeReaderValue<int>(reader, Form("Ndata.P.dc.%s.wirenum", p.Data()));
    }

    // Number of hits per plane
    for (auto const &p : planes) {
        nhit[p] = new TTreeReaderValue<double>(reader, Form("P.dc.%s.nhit", p.Data()));
    }

    // ------------------------------------------------------------
    // Analysis proper

    // open PDF; "filename.pdf["
    // canvas->Print((pdfFilename+"[").Data());

    int currentEvent;
    std::vector<Int_t> displayedEvents;

    TPolyLine3D   *track = nullptr;
    TPolyMarker3D *pm3d1 = nullptr;

    double wire;
    Int_t hitsum1, hitsum2;
    while (reader.Next()) {

        // Turn off planes
        for (auto const &p : planes) {
            planes3D[p]->TurnOff();
        }


        hitsum1=0;
        for (auto const &p : DC1planes) {
            hitsum1 += **nhit[p];
        }
        hitsum2=0;
        for (auto const &p : DC2planes) {
            hitsum2 += **nhit[p];
        }

        // =====================================================
        // -----------------------------------------------------
        // =====================================================
        // Skip events we're not interested in
        if (!(   *pHodBetanotrack < 1.5 && *pHodBetanotrack>0.5
              && *pNGCer < 0.1
              && *pHodGoodscinhit==1
              && (hitsum1<21) && (hitsum2<21)
              // && ((hitsum1>0) || (hitsum2>0))
              // && *pNtrack==0
             )
           ) {
            continue;
        }
        // =====================================================
        // -----------------------------------------------------
        // =====================================================

        // Display info to help user understand event
        currentEvent = reader.GetCurrentEntry();

        std::cout << std::endl;
        std::cout << "--------------------" << std::endl;
        std::cout << "Event " << currentEvent << std::endl;
        std::cout << std::endl;

        std::cout << "DC2 hits = ";
        for (auto const &p : DC2planes) {
            std::cout << std::setw(2) << **nhit[p] << " +";
        }
        std::cout << " = " << hitsum2 << std::endl;

        std::cout << "DC1 hits = ";
        for (auto const &p : DC1planes) {
            std::cout << std::setw(2) << **nhit[p] << " +";
        }
        std::cout << " = " << hitsum1 << std::endl;

        std::cout << "NGCER NPE sum    = " << *pNGCer  << std::endl;
        std::cout << "Calorimeter E    = " << *pPrE << " + " << *pShE << " = " << *pEtot << std::endl;
        std::cout << "Calorimeter E/p0 = " << *pEtotnorm << std::endl;
        std::cout << "Beta notrack     = " << *pHodBetanotrack << std::endl;
        std::cout << std::endl;

        for (auto const &p : planes) {
            for (int n=0; n < **wirenoN[p]; n++) {
                wire = (*wireno[p])[n];
                planes3D[p]->TurnOnWire(wire);

                // std::cout << p << " wire #" << wire << std::endl;
            }
        }

        if (*pNtrack>0) {
            // Draw track
            track = new TPolyLine3D(2);
            pm3d1 = new TPolyMarker3D(3);

            Double_t zNear = -75;
            Double_t zFar  =  75;
            Double_t xNear = (*xfp) + (*xpfp)*zNear;
            Double_t xFar  = (*xfp) + (*xpfp)*zFar;
            Double_t yNear = (*yfp) + (*ypfp)*zNear;
            Double_t yFar  = (*yfp) + (*ypfp)*zFar;

            track->SetPoint(0, xNear, yNear, zNear);
            track->SetPoint(1, xFar, yFar, zFar);
            track->SetLineColor(kGreen+1);
            track->Draw();

            // Where does the track intersect the DCs and FP?
            Double_t zDC1 = pdc_zpos["dc1"];
            Double_t zDC2 = pdc_zpos["dc2"];
            Double_t zFP  = 0;
            Double_t xDC1 = (*xfp) + (*xpfp)*zDC1;
            Double_t xDC2 = (*xfp) + (*xpfp)*zDC2;
            Double_t xFP  = (*xfp) + (*xpfp)*zFP;
            Double_t yDC1 = (*yfp) + (*ypfp)*zDC1;
            Double_t yDC2 = (*yfp) + (*ypfp)*zDC2;
            Double_t yFP  = (*yfp) + (*ypfp)*zFP;
            pm3d1->SetPoint(0, xDC1, yDC1, zDC1);
            pm3d1->SetPoint(1, xDC2, yDC2, zDC2);
            pm3d1->SetPoint(2, xFP,  yFP,  zFP);
            pm3d1->SetMarkerSize(2);
            pm3d1->SetMarkerColor(4);
            pm3d1->SetMarkerStyle(3);
            pm3d1->Draw();
        }

        c1->Update();
        c1->Draw();
        gPad->Modified(); gPad->Update(); gSystem->ProcessEvents();
        // gSystem->Sleep(2000);

        TString response = waitForInput("Enter any text for next event, 'p' for previous event: ");
        if (response=='p') {
            displayedEvents.pop_back();
            reader.SetEntry(displayedEvents.back());
        } else {
            displayedEvents.push_back(currentEvent);
        }

        if (track) {
            delete track;
            delete pm3d1;
        }
    }

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "End of TChain." << std::endl;

    // canvas->Print((pdfFilename+"]").Data());

}

int main(int argc, char* argv[]) {
    TApplication app("app",&argc,argv);
    StandaloneApplication(app.Argc(), app.Argv());
    app.Run();
    return 0;
}
