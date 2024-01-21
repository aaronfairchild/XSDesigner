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
#include "rectsolid.h"

CRectSolid::CRectSolid (const CVector<float>& fV) 
                     : CXSType (m_numRectDimensions)
// ---------------------------------------------------------------------------
// Function: overloaded ctor
// Input:    vector with rectangular solid dimensions
// Output:   none
// ---------------------------------------------------------------------------
{
    for (int i=1; i <= m_numRectDimensions; i++)
        m_fVDimensions(i) = fV(i);
    ComputeProperties ();
    m_Type = EPType::RECTS;
}

CRectSolid::~CRectSolid ()
// ---------------------------------------------------------------------------
// Function: dtor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
}

void CRectSolid::ComputeProperties ()
// ---------------------------------------------------------------------------
// Function: computes the rectangular solid properties
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // height
    float fH = m_fVDimensions(1); 
    // width
    float fW = m_fVDimensions(2); 

    // cross-sectional area
    m_fArea = fH * fW;
    // MOI y-axis
    m_fIyy = pow(fH, 3.0f) * fW / 12.0f;
    // MOI z-axis
    m_fIzz = pow(fW, 3.0f) * fH / 12.0f;
    // Section Modulus z-axis
    m_fSzz = (fW*fH*fH)/6.0f;
    // Section Modulus y-axis
    m_fSyy = (fW * fW * fH) / 6.0f;
    // Shear Factor z-axis
    m_fSFzz = (2.0f / 3.0f) * fW * fH;
    // Shear Factor y-axis
    m_fSFyy = (2.0f / 3.0f) * fW * fH;
}