#include <utility>
#include <vector>
#include <map>

#include <TF1.h>
#include <TCut.h>
#include <TH1F.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <CTData.h>
#include <CTCuts.h>
// This will loop over kinematics, histogram a branch, and fit the data to a Gaussian.
// The histograms are saved to a PDF, and the results of the fit are summarized in CSV.
//
// Example command line usage to histogram P.gtr.beta between 0 and 1.5, and then fit between 0.9 and 1.1:
// $ root -l "ct_gaussian_fit.C+(\"/home/jmatter/ct_scripts/ct_coin_data_pass3.json\", \"P.gtr.beta\", 300, 0, 1.5, \"pScinDid\", 0.9, 1.1)"
void ct_gaussian_fit(TString dataJson,
            TString branch, Int_t nbins, Double_t drawLo, Double_t drawHi, TString cutName,
            Double_t fitLo, Double_t fitHi) {

    // ------------------------------------------------------------
    // PDF and CSV to save
    //
    // Replace .'s with _'s for names.
    TString branchStr = branch;
    branchStr.ReplaceAll(".","_");

    TString pdfFilename = Form("/home/jmatter/ct_scripts/analysis/diagnostic/%s.pdf", branchStr.Data());
    TString csvFilename = Form("/home/jmatter/ct_scripts/analysis/diagnostic/%s.csv", branchStr.Data());

    std::ofstream csv;
    csv.open(csvFilename.Data());

    // Load cuts
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");
    TCut cut = cuts->Get(cutName);
    // ------------------------------------------------------------

    // Load data
    CTData *data = new CTData(dataJson);

    // Which kinematics?
    std::vector<TString> kinematics = data->GetNames();

    // Used in calls to Draw()
    TString histoName, drawMe;
    TH1F *histo;

    // Results of fit
    TF1* fit;
    Double_t mean, meanError, sigma, sigmaError;

    // ------------------------------------------------------------
    // Loop over kinematics, draw histogram, fit it, and then print to PDF

    TCanvas* canvas = new TCanvas("canvas", "Fit results", 640, 480);

    // open PDF; "filename.pdf["
    canvas->Print((pdfFilename+"[").Data());

    csv << "kinematics,Q2,target,mean,meanError,sigma,sigmaError" << std::endl;
    for (auto const &k: kinematics) {

        Double_t q = data->GetQ2(k);
        TString t = data->GetTarget(k);

        std::cout << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Fitting " << k << std::endl;

        // Form name and draw format
        histoName = Form("%s_%s", branch.Data(), k.Data());
        drawMe = Form("%s>>%s(%d,%f,%f)", branch.Data(), histoName.Data(), nbins, drawLo, drawHi);

        // Draw
        data->GetChain(k)->Draw(drawMe.Data(), cut, "");

        // Get pointer
        histo = (TH1F*) gDirectory->Get(histoName.Data());

        // Fit
        histo->Fit("gaus", "", "", fitLo, fitHi);

        fit = histo->GetFunction("gaus");
        mean  = fit->GetParameter(1);
        sigma = fit->GetParameter(2);
        meanError  = fit->GetParError(1);
        sigmaError = fit->GetParError(2);

        // Write to csv
        csv << k << "," << q <<  "," << t << "," << mean << "," << meanError << "," << sigma << "," << sigmaError << std::endl;
        std::cout << k << "," << q <<  "," << t << "," << mean << "," << meanError << "," << sigma << "," << sigmaError << std::endl;

        // Print
        canvas->Print(pdfFilename.Data()); // write page to PDF
    }
    canvas->Print((pdfFilename+"]").Data());
    csv.close();
}
