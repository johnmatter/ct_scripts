{
    std::vector<Double_t> q2s = {9.5, 11.5};
    std::map<Double_t, TCanvas*> canvas;
    std::map<Double_t, TFile*> file;
    TString canvasName;

    // Get stuff from canvas root files
    for (auto q2: q2s) {
        file[q2] = TFile::Open(Form("q2_%.1f_canvas.root", q2));

        // Get canvas
        canvasName = Form("c_q2_%.1f", q2);
        file[q2]->GetObject(canvasName.Data(), canvas[q2]);

        canvas[q2]->Draw();
    }
}

