/*********************************************
Program Planar Frame
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis

Derived class from CXSType base class
to store circular hollow data and properties
*********************************************/
#include <cmath>
#include <iostream>
#include "circhollow.h"
#include "constants.h"

CCircHollow::CCircHollow(const CVector<float>& fV)
                     : CXSType (m_numCircDimensions)
// ---------------------------------------------------------------------------
// Function: overloaded ctor
// Input:    vector with circular hollow dimensions
// Output:   none
// ---------------------------------------------------------------------------
{
    for (int i=1; i <= m_numCircDimensions; i++)
        m_fVDimensions(i) = fV(i);
    ComputeProperties ();
    m_Type = EPType::CIRCH;
}

CCircHollow::~CCircHollow()
// ---------------------------------------------------------------------------
// Function: dtor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
}

void CCircHollow::ComputeProperties ()
// ---------------------------------------------------------------------------
// Function: computes the circular hollow properties
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // inner radius
    float fRI = m_fVDimensions(1);
    // wall thickness
    float fWT = m_fVDimensions(2);
    // outter radius
    float fRO = fRI + fWT;

    // cross-sectional area
    m_fArea = PI * (fRO * fRO - fRI * fRI);
    // MOI y-axis
    m_fIyy = (PI / 4.0f) * (pow(fRO, 4.0f) - pow(fRI, 4.0f));
    // MOI z-axis
    m_fIzz = (PI / 4.0f) * (pow(fRO, 4.0f) - pow(fRI, 4.0f));
    // Section Modulus z-axis
    m_fSzz = m_fIzz / fRO;
    // Section Modulus y-axis
    m_fSyy = m_fIyy / fRO;
    // Shear Factor z-axis
    float fQ = (2.0f / 3.0f) * (pow(fRO, 3) - pow(fRI, 3));
    m_fSFzz = (2.0f * (fRO - fRI) * m_fIyy) / (fQ);
    // Shear Factor y-axis
    m_fSFyy = (2.0f * (fRO - fRI) * m_fIzz) / (fQ);
}