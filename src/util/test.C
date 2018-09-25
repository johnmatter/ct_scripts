#include <CTData.h>
#include <CTCuts.h>

// This macro tests the CTData dn CTCuts classes.
// It requires the user to know what I wrote this to test.
// TODO: Automated tests
void test() {
    CTData* data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts* cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    std::cout << "Now testing the CTData and CTCuts classes" << std::endl;
    if (data->TestChains() == true) {
        // Test that we can access info in the chain
        TString kinematics = "LH2_Q2_8";

        Double_t Q2 = data->GetQ2(kinematics);
        TString  target  = data->GetTarget(kinematics);
        Double_t hmsMomentum = data->GetHMSMomentum(kinematics);
        Double_t shmsMomentum = data->GetSHMSMomentum(kinematics);
        Double_t hmsAngle = data->GetHMSAngle(kinematics);
        Double_t shmsAngle = data->GetSHMSAngle(kinematics);

        // Display information in the terminal
        std::cout << std::right
            << std::setw(30) << "name"
            << std::setw(5) << "Q2"
            << std::setw(10) << "target"
            << std::setw(15) << "hmsMomentum"
            << std::setw(15) << "shmsMomentum"
            << std::setw(15) << "hmsAngle"
            << std::setw(15) << "shmsAngle"
            << std::endl;
        std::cout << std::string(110, '-') << std::endl;

        std::cout << std::right
            << std::setw(30) << kinematics
            << std::setw(5) << Q2
            << std::setw(10) << target
            << std::setw(15) << hmsMomentum
            << std::setw(15) << shmsMomentum
            << std::setw(15) << hmsAngle
            << std::setw(15) << shmsAngle
            << std::endl;

        std::vector<Int_t> runs = data->GetRuns(kinematics);
        std::cout << "Runs:";
        for (auto run : runs) {
            std::cout << " " << run;
        }
        std::cout << std::endl;

        // Try to access some info in the chain and plot it
        TChain* chain = data->GetChain(kinematics);
        std::cout << "Chain has nentries:" << chain->GetEntries() << std::endl;

        TString drawStr = "H.gtr.dp>>h(100,-20,20)";
        TCut cut = cuts->Get("coinCutsLH2");

        std::cout << "Test draw" << std::endl;
        std::cout << drawStr << std::endl;
        cut.Print();

        chain->Draw(drawStr,cut);
    } else {
        std::cout << "Failure." << std::endl;
    }
}
