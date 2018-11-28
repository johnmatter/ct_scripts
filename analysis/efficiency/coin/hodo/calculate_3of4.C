#include <algorithm>
#include <utility>
#include <fstream>
#include <vector>
#include <tuple>
#include <map>

#include <TEfficiency.h>
#include <TCanvas.h>
#include <TH1.h>

#include <CTData.h>
#include <CTCuts.h>

void calculate_3of4() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    // Where are we saving the output
    TString csvPerPlaneFilename = "hodoPlaneEfficiency.csv";
    TString csv3of4Filename = "hodo3of4Efficiency.csv";

    // Which kinematics
    std::vector<TString> kinematics = {"LH2_Q2_8","LH2_Q2_10","LH2_Q2_12","LH2_Q2_14",
                                       "C12_Q2_8","C12_Q2_10","C12_Q2_12","C12_Q2_14"};

    // Which hodoscope planes
    std::vector<TString> spectrometers = {"P", "H"};
    std::vector<TString> planes = {"1x", "1y", "2x", "2y"};

    // Key is <kinematics,spectrometer,plane>
    std::map<std::tuple<TString,TString,TString>, TEfficiency*> efficiencyPerPlane;

    // Key is <kinematics,spectrometer>
    std::map<std::tuple<TString,TString>, Double_t> efficiency3of4;
    std::map<std::tuple<TString,TString>, Double_t> efficiency3of4Error;

    // ------------------------------------------------------------------------
    // Calculate per-plane efficiencies
    // --------------------------------
    // Loop over kinematics
    for (auto const &k : kinematics) {
        std::cout << "Calculating per-plane efficiency for " << k << std::endl;
        // Loop over spectrometers
        for (auto const &s : spectrometers) {
            // Calculate per-plane efficiency
            for (auto const &p : planes) {

                // Construct "should" cut based on PID and the other planes
                TCut shouldFire;
                if (s.EqualTo("H")) {
                        shouldFire = cuts->Get("hPIDCut");
                        shouldFire = shouldFire + "H.hod.goodscinhit==1";
                }
                if (s.EqualTo("P")) {
                        shouldFire = cuts->Get("pPIDCut");
                        shouldFire = shouldFire + "P.hod.goodscinhit==1";
                }
                for (auto const &q : planes) {
                    if (p != q) {
                        shouldFire += Form("%s.hod.%s.nhits>0", s.Data(), q.Data());
                    }
                }
                TCut didFire = shouldFire + Form("%s.hod.%s.nhits>0", s.Data(), p.Data());

                auto key = std::make_tuple(k,s,p);

                // Set up efficiency object
                // I'm using scattering angle as a dummy variable here.
                // There might be a more careful way.
                TString didHistoName    = Form("%s_%s.%s_pass", k.Data(), s.Data(), p.Data());
                TString shouldHistoName = Form("%s_%s.%s_should", k.Data(), s.Data(), p.Data());
                TString didDraw    = Form("H.kin.primary.scat_ang_deg>>%s(1,0,180)", didHistoName.Data());
                TString shouldDraw = Form("H.kin.primary.scat_ang_deg>>%s(1,0,180)", shouldHistoName.Data());
                data->GetChain(k)->Draw(didDraw.Data(),    didFire);
                data->GetChain(k)->Draw(shouldDraw.Data(), shouldFire);
                TH1 *passed = (TH1*) gDirectory->Get(didHistoName.Data());
                TH1 *total  = (TH1*) gDirectory->Get(shouldHistoName.Data());
                efficiencyPerPlane[key] = new TEfficiency(*passed, *total);
            }
        }
    }

    // Print per-plane efficiency
    std::cout << "Writing per-plane efficiency" << std::endl;
    std::ofstream ofsPerPlane;
    ofsPerPlane.open(csvPerPlaneFilename.Data());
    ofsPerPlane << "target,Q2,spectrometer,plane,efficiency,efficiencyErrorUp,efficiencyErrorLow,did,should" << std::endl;
    for (auto const &k : kinematics) {
        for (auto const &s : spectrometers) {
            for (auto const &p : planes) {
                auto key = std::make_tuple(k,s,p);
                TEfficiency* e = efficiencyPerPlane[key];
                TString printme = Form("%s,%f,%s,%s,%f,%f,%f,%d,%d",
                                    data->GetTarget(k).Data(), data->GetQ2(k), s.Data(), p.Data(),
                                    e->GetEfficiency(1), e->GetEfficiencyErrorUp(1), e->GetEfficiencyErrorLow(1),
                                    e->GetCopyPassedHisto()->GetBinContent(1),
                                    e->GetCopyTotalHisto()->GetBinContent(1));
                ofsPerPlane << printme << std::endl;
            }
        }
    }
    ofsPerPlane.close();

    // ------------------------------------------------------------------------
    // Calculate 3/4 efficiency
    // Tvaskis' definition: e = prod_i(e_i) + sum_i((1-e_i)*prod_{j!=i}(e_j))
    // --------------------------------
    // Loop over kinematics
    for (auto const &k : kinematics) {
        std::cout << "Calculating 3/4 efficiency for " << k << std::endl;
        // Loop over spectrometers
        for (auto const &s : spectrometers) {
            // Calculate efficiency
            Double_t efficiency = 1;
            for (auto const &p : planes) {
                efficiency *= efficiencyPerPlane[std::make_tuple(k,s,p)]->GetEfficiency(1);
            }

            for (auto const &p : planes) {
                Double_t thisTerm = (1 - efficiencyPerPlane[std::make_tuple(k,s,p)]->GetEfficiency(1));
                for (auto const &q : planes) {
                    if (p != q) {
                        thisTerm *= efficiencyPerPlane[std::make_tuple(k,s,p)]->GetEfficiency(1);
                    }
                }
                efficiency += thisTerm;
            }
            efficiency3of4[std::make_tuple(k,s)] = efficiency;

            // Calculate efficiency error
            // sqrt( sum_i(1-[prod_{j!=i}(e_j)])*sigma_i)^2
            Double_t error = 0;
            for (auto const &p : planes) {
                Double_t thisTerm = -1;
                for (auto const &q : planes) {
                    if (p != q) {
                        Double_t eUp = efficiencyPerPlane[std::make_tuple(k,s,q)]->GetEfficiencyErrorUp(1);
                        Double_t eLo = efficiencyPerPlane[std::make_tuple(k,s,q)]->GetEfficiencyErrorLow(1);
                        thisTerm *= max(eUp,eLo);
                    }
                }
                thisTerm += 1;

                Double_t eUp = efficiencyPerPlane[std::make_tuple(k,s,p)]->GetEfficiencyErrorUp(1);
                Double_t eLo = efficiencyPerPlane[std::make_tuple(k,s,p)]->GetEfficiencyErrorLow(1);
                thisTerm *= max(eUp,eLo);

                thisTerm *= thisTerm;

                error += thisTerm;
            }
            error = sqrt(error);
            efficiency3of4Error[std::make_tuple(k,s)] = error;

        } // loop over spectrometers
    }// loop over kinematics

    // Print 3/4 efficiency
    std::cout << "Writing 3/4 efficiency" << std::endl;
    std::ofstream ofs3of4;
    ofs3of4.open(csv3of4Filename.Data());
    ofs3of4 << "target,Q2,spectrometer,efficiency,efficiencyError" << std::endl;
    for (auto const &k : kinematics) {
        for (auto const &s : spectrometers) {
                TString printme = Form("%s,%f,%s,%f,%f",
                                    data->GetTarget(k).Data(), data->GetQ2(k), s.Data(),
                                    efficiency3of4[std::make_tuple(k,s)],
                                    efficiency3of4Error[std::make_tuple(k,s)]);
                ofs3of4 << printme << std::endl;
        }
    }
    ofs3of4.close();
}
