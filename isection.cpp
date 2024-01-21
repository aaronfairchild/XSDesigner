/*********************************************
Program Planar Frame
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis

Derived class from CXSType base class
to store I-section data and properties
*********************************************/
#include <cmath>
#include <iostream>
#include "isection.h"

CISection::CISection (const CVector<float>& fV) : 
                      CXSType (m_numISDimensions)
// ---------------------------------------------------------------------------
// Function: overloaded ctor
// Input:    vector with I-section dimensions
// Output:   none
// ---------------------------------------------------------------------------
{
    for (int i=1; i <= m_numISDimensions; i++)
        m_fVDimensions(i) = fV(i);
    ComputeProperties ();
    m_Type = EPType::ISECTION;
}

CISection::~CISection ()
// ---------------------------------------------------------------------------
// Function: dtor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
}

void CISection::ComputeProperties ()
// ---------------------------------------------------------------------------
// Function: computes the I-section properties
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // web height
    float wH = m_fVDimensions(1); 
    // web thickness
    float wT = m_fVDimensions(2); 
    // flange width
    float fW = m_fVDimensions(3); 
    // flange thickness
    float fT = m_fVDimensions(4); 

    // cross-sectional area
    m_fArea = wH*wT + 2.0f*fW*fT;
    // MOI y-axis
    float fOI, fII;
    fOI = (pow(wH+2.0f*fT, 3.0f) * fW)/12.0f;
    fII = 2.0f*(pow(wH, 3.0f) * (0.5f*(fW-wT)))/12.0f;
    m_fIyy = fOI - fII;
    // MOI z-axis
    float fIzz = 2.0f*(pow(fW,3.0f)*fT)/12.0f + 
                 pow(wT,3.0f)*wH/12.0f;
    m_fIzz = fIzz;
    // Section Modulus z-axis
    m_fSzz = m_fIzz / (wH/2.0f + fT);
    // Section Modulus y-axis
    m_fSyy = m_fIyy / (fW/2.0f);
    // Shear Factor y-axis
    float fQy = ((fW * fW * fT) / 4.0f) + ((wH * wT * wT) / 4.0f);
    m_fSFyy = (fT * m_fIyy) / fQy;
    // Shear Factor z-axis
    float fQz = fW * fT * ((wH + fT) / 2.0f) + ((wH * wT) / 2.0f) * (wH / 4.0f);
    m_fSFzz = (wT * m_fIzz) / fQz;
}