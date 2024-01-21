/*********************************************
Program Planar Frame
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis

Derived class from CXSType base class
to store rectangular solid data and properties
*********************************************/
#include <cmath>
#include <iostream>
#include "circsolid.h"
#include "constants.h"

CCircSolid::CCircSolid (const CVector<float>& fV) 
                     : CXSType (m_numCircDimensions)
// ---------------------------------------------------------------------------
// Function: overloaded ctor
// Input:    vector with circular solid dimensions
// Output:   none
// ---------------------------------------------------------------------------
{
    for (int i=1; i <= m_numCircDimensions; i++)
        m_fVDimensions(i) = fV(i);
    ComputeProperties ();
    m_Type = EPType::CIRCS;
}

CCircSolid::~CCircSolid ()
// ---------------------------------------------------------------------------
// Function: dtor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
}

void CCircSolid::ComputeProperties ()
// ---------------------------------------------------------------------------
// Function: computes the circular solid properties
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // radius
    float fR = m_fVDimensions(1);

    // cross-sectional area
    m_fArea = PI * fR * fR;
    // MOI y-axis
    m_fIyy = pow(fR, 4.0f) * PI / 4.0f;
    // MOI z-axis
    m_fIzz = pow(fR, 4.0f) * PI / 4.0f;
    // Section Modulus z-axis
    m_fSzz = m_fIzz / fR;
    // Section Modulus y-axis
    m_fSyy = m_fIyy / fR;
    // Shear Factor z-axis
    float fQ = (2.0f / 3.0f) * (pow(fR, 3));
    m_fSFzz = (2.0f * fR * m_fIyy) / (fQ);
    // Shear Factor y-axis
    m_fSFyy = (2.0f * fR * m_fIzz) / (fQ);
}