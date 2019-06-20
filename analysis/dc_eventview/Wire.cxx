#include "Wire.h"

// Constructor
Wire::Wire(Double_t x0, Double_t y0, Double_t z0,
           Double_t x1, Double_t y1, Double_t z1) {
    xBegin = x0;
    yBegin = y0;
    zBegin = z0;
    xEnd = x1;
    yEnd = y1;
    zEnd = z1;

    guiElement = new TPolyLine3D(2);
    guiElement->SetPoint(0, x0, y0, z0);
    guiElement->SetPoint(1, x1, y1, z1);

    TurnOff();
}

// Copy constructor
Wire::Wire(const Wire& wire) {
    Copy(wire);
}

// Assignment operator
Wire& Wire::operator=(const Wire& wire) {
    // Protect against self-assignment
    if (this != &wire) {
        Clear();      // prevents memory leak
        Copy(wire); // copy object
    }

    return *this; // for cases like a=b=c
}

// Destructor
Wire::~Wire() {
    Clear();
}

// Copy data from another object
void Wire::Copy(const Wire& wire) {
}

// Clear all data
void Wire::Clear() {
    delete guiElement;
}

void Wire::TurnOn() {
    isOn  = true;
    color = kRed;
    guiElement->SetLineColor(color);
}

void Wire::TurnOff() {
    isOn  = false;
    color = kBlack;
    guiElement->SetLineColorAlpha(color, 0.1);
}
