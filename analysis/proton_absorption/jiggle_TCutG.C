#include <TFile.h>
#include <TROOT.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCut.h>
#include <TMath.h>

// Note that the area calculated by TCutG::Area() is directed!
// Reversing the order of the points will flip the sign.
// Negative area is into the screen, positive is out.
void jiggle_TCutG() {

    Double_t deltaMagnitude = 0.01; // how much to wiggle each point per iteration?
    Double_t reductionFactor = 0.9; // how much smaller do we want the area to be?
    Double_t epsilon = 0.0001; // how close do we need to be to the reduction factor?

    TCutG *cut = new TCutG("cut", 11);
    cut->SetPoint(0,-0.0266854,-0.0496894);
    cut->SetPoint(1,-0.0290262,0.0139752);
    cut->SetPoint(2,-0.0154494,0.0690994);
    cut->SetPoint(3,0.0135768,0.0737578);
    cut->SetPoint(4,0.0346442,0.0434783);
    cut->SetPoint(5,0.0318352,-0.0263975);
    cut->SetPoint(6,0.0224719,-0.0659938);
    cut->SetPoint(7,-0.0145131,-0.0636646);
    cut->SetPoint(8,-0.0262172,-0.0504658);
    cut->SetPoint(9,-0.0262172,-0.0504658);
    cut->SetPoint(10,-0.0266854,-0.0496894);

    // // This area should be 4. And indeed after trying it, it is!
    // TCutG *cut = new TCutG("asdf", 5);
    // cut->SetPoint(0, 0, 0);
    // cut->SetPoint(1, 0, 10);
    // cut->SetPoint(2,10, 10);
    // cut->SetPoint(3,10, 0);
    // cut->SetPoint(4, 0, 0);

    // // This area should be 100. And indeed after trying it, it is!
    // TCutG *cut = new TCutG("asdf", 5);
    // cut->SetPoint(0, 5, 15);
    // cut->SetPoint(1, 5,  5);
    // cut->SetPoint(2,15,  5);
    // cut->SetPoint(3,15,15);
    // cut->SetPoint(4, 5,15);


    Double_t x, y;
    Double_t xCenter, yCenter;
    Double_t xWiggle, yWiggle;

    // Initialize the wiggled cut
    TCutG *cutWiggle = new TCutG("cutWiggle", cut->GetN());
    cutWiggle->SetLineColor(kRed);
    for (int i=0; i<cut->GetN(); i++) {
        cut->GetPoint(i, x, y);
        cutWiggle->SetPoint(i, x, y);
    }
    cut->Center(xCenter, yCenter);

    Double_t areaRatio = 1.0;

    // Wiggle!
    Double_t delta;
    while (abs(areaRatio - reductionFactor) > epsilon) {
        if (areaRatio<reductionFactor)
            delta = -deltaMagnitude;
        else
            delta = +deltaMagnitude;

        for (int i=0; i<cutWiggle->GetN(); i++) {
            cutWiggle->GetPoint(i, x, y);

            xWiggle = xCenter + (1-delta) * (x-xCenter);
            yWiggle = yCenter + (1-delta) * (y-yCenter);

            cutWiggle->SetPoint(i, xWiggle, yWiggle);

        }

        areaRatio = (cutWiggle->Area()/cut->Area());
        std::cout << areaRatio << std::endl;
    }

    // Visualize
    if (reductionFactor<1) {
        cut->Draw();
        cutWiggle->Draw("SAME");
    } else {
        cutWiggle->Draw();
        cut->Draw("SAME");
    }

    // Summarize cuts
    cut->Center(xCenter, yCenter);
    std::cout << "Initial area = " << cut->Area() << std::endl;
    std::cout << Form("Old center = (%f, %f)\n", xCenter, yCenter);

    cutWiggle->Center(xCenter, yCenter);
    std::cout << "New area = " << cutWiggle->Area() << std::endl;
    std::cout << Form("New center = (%f, %f)\n", xCenter, yCenter);

    // This should give us the desired ratio
    std::cout << "ratio = " << areaRatio << std::endl;

    std::cout << "\n\n\n";
    for (int i=0; i<cutWiggle->GetN(); i++) {
        cutWiggle->GetPoint(i, x, y);
        std::cout << Form("cut->SetPoint(%d, %f, %f);\n", i, x, y);
    }
    std::cout << "\n\n\n";

}
