#ifndef PLANE_H
#define PLANE_H

#include <vector>
#include <iostream>

#include <TMath.h>

#include "Wire.h"

class Plane {
    public:
        Plane();
        Plane(const Plane& plane);
        Plane& operator=(const Plane& plane);
        ~Plane();

        void Copy(const Plane& plane);
        void Clear();

        // These ALL need to be called after constructing a plane
        void SetCenter(Double_t x, Double_t y, Double_t z) {x0=x; y0=y; z0=z;};
        void SetTilt(Double_t a, Double_t b, Double_t g) {alpha=a; beta=b; gamma=g;};
        void SetSize(Double_t h, Double_t w) {height=h; width=w;};
        void SetWires(Int_t n, Double_t p, Double_t center) {nWires=n; pitch=p; centerWire = center;};

        // This should be called after all the SetX()s
        void Populate();
        std::vector<Double_t> WireCoordToTransportCoord(Double_t chi, Double_t psi);

        void TurnOnWire(Int_t n);
        void TurnOffWire(Int_t n);
        void TurnOff();

        void Draw();

    private:

        Double_t height, width;      // size [cm]
        Double_t pitch;              // wire spacing [cm]
        Double_t x0, y0, z0;         // center [cm]
        Double_t alpha, beta, gamma; // tilt [rad]
        Int_t nWires;
        Double_t centerWire;         // [number]

        std::vector<Wire*> wires;
};

#endif
