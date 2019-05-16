#include <utility>
#include <vector>
#include <map>

#include <TCut.h>
#include <TH2F.h>
#include <TCanvas.h>

#include <CTData.h>
#include <CTCuts.h>
void plot() {
    CTData *data = new CTData("/home/jmatter/ct_scripts/ct_coin_data.json");
    CTCuts *cuts = new CTCuts("/home/jmatter/ct_scripts/cuts.json");

    TString pdfFilename = "/home/jmatter/ct_scripts/analysis/efficiency/coin/hcal/prshwr_vs_shw.pdf";

    TCut cutShould = cuts->Get("hBetaCut");

    //-------------------------------------------------------------------------
    TCanvas *c1 = new TCanvas("c1", "HMS Cal Shower vs Preshower Energy", 600, 600);
    c1->SetLogz();

    c1->Print((pdfFilename+"[").Data()); // open PDF; "filename.pdf["
    for (auto const &k: data->GetNames()) {
        // Draw
        TString histoName = Form("prshVsShw_%s", k.Data());
        TString drawMe = Form("(H.cal.etrack-H.cal.eprtrack):H.cal.eprtrack>>%s(150,0,3,300,0,6)", histoName.Data());
        data->GetChain(k)->Draw(drawMe.Data(), hBetaCut, "goff");

        // Print to pdf
        TH1 *histoPointer = (TH1*) gDirectory->Get(histoName.Data());
        histoPointer->Draw("colz");
        c1->Print(pdfFilename.Data()); // write page to PDF
    }
    c1->Print((pdfFilename+"]").Data()); // close PDF; "filename.pdf]"
}
