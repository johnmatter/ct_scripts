#include <utility>
#include <vector>
#include <map>

#include <TF1.h>
#include <TCut.h>
#include <TH1F.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TTreeReaderValue.h>

#include <CTData.h>
#include <CTCuts.h>
// GOAL: Plot hits in DC along with track found by hcana
void dc_eventview() {

    int maxEvents = 1;

    // ------------------------------------------------------------
    TString pdfFilename = "/home/jmatter/ct_scripts/analysis/diagnostic/pdc_trackview.pdf";

    // Load cuts and data
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data_pass3.json");

    // Which kinematics?
    TString kinematics = "LH2_Q2_8";
    TChain* chain = data->GetChain(kinematics);

    gStyle->SetOptStat(0);
    TCanvas* canvas = new TCanvas("canvas", "Fit results", 1024, 480);

    // ------------------------------------------------------------
    // Geometry from hallc_replay/PARAM/SHMS/DC/pdc_geom.param

    Double_t cminch = 2.54;
    Double_t raddeg = 3.1415926535/180.0;
    // This is the correct order that particles pass through
    std::vector<TString> planes = {"1u1", "1u2", "1x1", "1x2", "1v1", "1v2",
                                   "2v2", "2v1", "2x2", "2x1", "2u2", "2u1"};

    std::map<TString, double> pdc_xcenter, pdc_ycenter, pdc_zpos,
                              pdc_roll, pdc_pitch, pdc_yaw;

    std::map<TString, double> pdc_plane_xpos, pdc_plane_ypos, pdc_plane_zpos,
                              pdc_plane_alpha, pdc_plane_beta, pdc_plane_gamma,
                              pdc_central_wire;

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

    // wire spacing in cm
    pitch = 1.0;

    // plane posititons in cm
    pdc_plane_xpos["1u1"] = pdc_xcenter["dc1"]-0.006
    pdc_plane_xpos["1u2"] = pdc_xcenter["dc1"]-0.018
    pdc_plane_xpos["1x1"] = pdc_xcenter["dc1"]
    pdc_plane_xpos["1x2"] = pdc_xcenter["dc1"]
    pdc_plane_xpos["1v1"] = pdc_xcenter["dc1"]
    pdc_plane_xpos["1v2"] = pdc_xcenter["dc1"]-0.015
    pdc_plane_xpos["2v2"] = pdc_xcenter["dc2"]-0.012
    pdc_plane_xpos["2v1"] = pdc_xcenter["dc2"]+0.014
    pdc_plane_xpos["2x2"] = pdc_xcenter["dc2"]
    pdc_plane_xpos["2x1"] = pdc_xcenter["dc2"]
    pdc_plane_xpos["2u2"] = pdc_xcenter["dc2"]-0.01
    pdc_plane_xpos["2u1"] = pdc_xcenter["dc2"]+0.001

    pdc_plane_ypos["1u1"] = pdc_ycenter["dc1"]-0.009
    pdc_plane_ypos["1u2"] = pdc_ycenter["dc1"]-0.017
    pdc_plane_ypos["1x1"] = pdc_ycenter["dc1"]
    pdc_plane_ypos["1x2"] = pdc_ycenter["dc1"]
    pdc_plane_ypos["1v1"] = pdc_ycenter["dc1"]
    pdc_plane_ypos["1v2"] = pdc_ycenter["dc1"]+0.015
    pdc_plane_ypos["2v2"] = pdc_ycenter["dc2"]-0.012
    pdc_plane_ypos["2v1"] = pdc_ycenter["dc2"]+0.018
    pdc_plane_ypos["2x2"] = pdc_ycenter["dc2"]
    pdc_plane_ypos["2x1"] = pdc_ycenter["dc2"]
    pdc_plane_ypos["2u2"] = pdc_ycenter["dc2"]-0.003
    pdc_plane_ypos["2u1"] = pdc_ycenter["dc2"]-0.001

    pdc_plane_zpos["1u1"] = pdc_zpos["dc1"] - 0.68701*cminch
    pdc_plane_zpos["1u2"] = pdc_zpos["dc1"] - 0.43701*cminch
    pdc_plane_zpos["1x1"] = pdc_zpos["dc1"] - 0.18701*cminch
    pdc_plane_zpos["1x2"] = pdc_zpos["dc1"] + 0.18701*cminch
    pdc_plane_zpos["1v1"] = pdc_zpos["dc1"] + 0.43701*cminch
    pdc_plane_zpos["1v2"] = pdc_zpos["dc1"] + 0.68701*cminch
    pdc_plane_zpos["2v2"] = pdc_zpos["dc2"] - 0.68701*cminch
    pdc_plane_zpos["2v1"] = pdc_zpos["dc2"] - 0.43701*cminch
    pdc_plane_zpos["2x2"] = pdc_zpos["dc2"] - 0.18701*cminch
    pdc_plane_zpos["2x1"] = pdc_zpos["dc2"] + 0.18701*cminch
    pdc_plane_zpos["2u2"] = pdc_zpos["dc2"] + 0.43701*cminch
    pdc_plane_zpos["2u1"] = pdc_zpos["dc2"] + 0.68701*cminch

    // Wire angles with respect to the x direction
    pdc_plane_alpha["1u1"] = ( 30.0 + pdc_roll["dc1"])*raddeg
    pdc_plane_alpha["1u2"] = ( 30.0 + pdc_roll["dc1"])*raddeg
    pdc_plane_alpha["1x1"] = ( 90.0 + pdc_roll["dc1"])*raddeg
    pdc_plane_alpha["1x2"] = ( 90.0 + pdc_roll["dc1"])*raddeg
    pdc_plane_alpha["1v1"] = (150.0 + pdc_roll["dc1"])*raddeg
    pdc_plane_alpha["1v2"] = (150.0 + pdc_roll["dc1"])*raddeg
    pdc_plane_alpha["2v2"] = ( 30.0 + pdc_roll["dc2"])*raddeg
    pdc_plane_alpha["2v1"] = ( 30.0 + pdc_roll["dc2"])*raddeg
    pdc_plane_alpha["2x2"] = ( 90.0 + pdc_roll["dc2"])*raddeg
    pdc_plane_alpha["2x1"] = ( 90.0 + pdc_roll["dc2"])*raddeg
    pdc_plane_alpha["2u2"] = (150.0 + pdc_roll["dc2"])*raddeg
    pdc_plane_alpha["2u1"] = (150.0 + pdc_roll["dc2"])*raddeg

    // Central wires
    pdc_central_wire["1u1"] = 54.25
    pdc_central_wire["1u2"] = 53.75
    pdc_central_wire["1x1"] = 40.25
    pdc_central_wire["1x2"] = 39.75
    pdc_central_wire["1v1"] = 54.25
    pdc_central_wire["1v2"] = 53.75
    pdc_central_wire["2v2"] = 53.75
    pdc_central_wire["2v1"] = 54.25
    pdc_central_wire["2x2"] = 39.75
    pdc_central_wire["2x1"] = 40.25
    pdc_central_wire["2u2"] = 53.75
    pdc_central_wire["2u1"] = 54.25

    for (auto const &p : planes) {
        TString thisDC = Form("dc%s", p[0]);
        pdc_plane_beta[p]  = pdc_pitch[thisDC]*raddeg;
        pdc_plane_gamma[p] = pdc_yaw[thisDC]*raddeg;
    }

    // ------------------------------------------------------------
    // Set up TTreeReader
    TTreeReader reader(chain);

    std::map<TString, TTreeReaderArray<double>*> wireno;
    std::map<TString, TTreeReaderValue<int>*> wirenoN;

    TTreeReaderValue<double> pHodBetanotrack(reader, "P.hod.betanotrack");

    // Wire number
    for (auto const &p : planes) {
        wireno[p] = new TTreeReaderArray<double>(reader, Form("P.dc.%s.wirenum", p.Data()));
    }

    // Number of hits per plane
    for (auto const &p : planes) {
        wirenoN[p] = new TTreeReaderValue<int>(reader, Form("Ndata.P.dc.%s.wirenum", p.Data()));
    }

    // ------------------------------------------------------------
    // Analysis proper

    // open PDF; "filename.pdf["
    canvas->Print((pdfFilename+"[").Data());

    double wire;
    int eventNum=0;
    while (reader.Next() && (eventNum<maxEvents)) {
        for (auto const &p : planes) {
            for (int n=0; n < **wirenoN[p]; n++) {
                wire = (*wireno[p])[n];
            }
        }
        eventNum++;
    }

    canvas->Print((pdfFilename+"]").Data());
}
