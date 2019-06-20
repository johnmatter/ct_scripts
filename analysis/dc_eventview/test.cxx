#include <iostream>

#include <TApplication.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TView.h>
#include <TPad.h>

#include "Plane.h"

// Test progrm for my Plane and Wire display classes
int main(int argc, char* argv[]) {
    Double_t raddeg = 3.1415926535/180.0;

    TApplication app("app",&argc,argv);

    TCanvas *c1 = new TCanvas("c1","c1",500,500);
    TView *view = TView::CreateView(1);
    view->SetRange(0,0,0,200,200,200);

    Plane *p = new Plane();

    p->SetCenter(-0.036, -0.41, 40);
    p->SetTilt(210.0*raddeg, 0.001*raddeg, -0.035*raddeg);
    p->SetSize(100, 100);
    p->SetWires(100, 1, 54.25);
    p->Populate();

    p->Draw();

    c1->Update();
    c1->Draw();
    gPad->Modified(); gPad->Update(); gSystem->ProcessEvents();

    app.Run();

    return 0;
}
