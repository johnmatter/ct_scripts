#ifndef WIRE_H
#define WIRE_H

#include <vector>

#include <TPolyLine3D.h>

class Wire {
    public:
        Wire(Double_t x0, Double_t y0, Double_t z0, Double_t x1, Double_t y1, Double_t z2);
        Wire(const Wire& wire);
        Wire& operator=(const Wire& wire);
        ~Wire();

        void Copy(const Wire& wire);
        void Clear();

        void TurnOn();
        void TurnOff();

        void Draw() { guiElement->Draw(); };

    private:
        Bool_t isHit = false;
        Int_t color = kBlack;
        Double_t xBegin;
        Double_t xEnd;
        Double_t yBegin;
        Double_t yEnd;
        Double_t zBegin;
        Double_t zEnd;

        TPolyLine3D* guiElement;
};

#endif
