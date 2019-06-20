#include "Plane.h"

// Constructor
Plane::Plane() {
}

// Copy constructor
Plane::Plane(const Plane& plane) {
    Copy(plane);
}

// Assignment operator
Plane& Plane::operator=(const Plane& plane) {
    // Protect against self-assignment
    if (this != &plane) {
        Clear();      // prevents memory leak
        Copy(plane); // copy object
    }

    return *this; // for cases like a=b=c
}

// Destructor
Plane::~Plane() {
    Clear();
}

// Copy data from another object
void Plane::Copy(const Plane& plane) {
}

// Clear all data
void Plane::Clear() {
    for (auto &wire: wires) {
        delete wire;
    }
}

// Calculate positions of wires and construct them
void Plane::Populate() {
    Clear();

    Double_t wirelength=100;
    for (int n=0; n<nWires; n++) {
        // chi is along wires, psi is perpendicular to wires
        Double_t chi0 = -wirelength/2;
        Double_t psi0 = (n-centerWire)*pitch;
        std::vector<Double_t> x0 = WireCoordToTransportCoord(chi0, psi0);

        Double_t chi1 = wirelength/2;
        Double_t psi1 = (n-centerWire)*pitch;
        std::vector<Double_t> x1 = WireCoordToTransportCoord(chi1, psi1);

        Wire *wire = new Wire(x0[0],x0[1],x0[2],
                              x1[0],x1[1],x1[2]);
        wires.push_back(wire);
    }
}

// See https://hallcweb.jlab.org/DocDB/0008/000845/001/DC_orientation.pdf
// for details of the coordinate transformation
std::vector<Double_t> Plane::WireCoordToTransportCoord(Double_t chi, Double_t psi) {
    // Rotate
    Double_t z =  psi*(TMath::Sin(alpha)*TMath::Sin(beta)+TMath::Cos(alpha)*TMath::Cos(beta)*TMath::Sin(gamma))
                 -chi*(TMath::Cos(alpha)*TMath::Sin(beta)-TMath::Sin(alpha)*TMath::Cos(beta)*TMath::Sin(gamma));
    Double_t x =  psi*(TMath::Sin(alpha)*TMath::Cos(beta)-TMath::Cos(alpha)*TMath::Sin(beta)*TMath::Sin(gamma))
                 -chi*(TMath::Cos(alpha)*TMath::Cos(beta)+TMath::Sin(alpha)*TMath::Sin(beta)*TMath::Sin(gamma));
    Double_t y =  psi*(TMath::Cos(alpha)*TMath::Cos(gamma))+chi*(TMath::Sin(alpha)*TMath::Cos(gamma));

    // Add offsets
    x += x0;
    y += y0;
    z += z0;

    return {x,y,z};
}

void Plane::TurnOnWire(Int_t n) {
    wires[n]->TurnOn();
}

void Plane::TurnOffWire(Int_t n) {
    wires[n]->TurnOff();
}

void Plane::TurnOff() {
    for (auto &wire : wires) {
        wire->TurnOff();
    }
}

void Plane::Draw() {
    for (auto &wire : wires) {
        wire->Draw();
    }
}
