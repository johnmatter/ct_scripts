void save_bcm_plots() {
    TFile *f = new TFile("bcm_average.root");
    TList *keys = f->GetListOfKeys();
    TCanvas *c;
    TString pdfFilename;
    char *name;
    int numCanvases = keys->GetSize();

    for (int n=0; n<numCanvases; n++) {
        std::cout << Form("%d/%d\n", n+1, numCanvases) << std::endl;;
        name = (char*) keys->At(n)->GetName();
        c = (TCanvas*) f->Get(name);

        pdfFilename = Form("%s.pdf", name);

        c->Print(pdfFilename.Data());
        delete c;
    }
}
