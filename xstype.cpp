/*********************************************
Program Planar Frame
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis

Base class for element cross-sectional properties
*********************************************/
#include "xstype.h"

CXSType::CXSType ()
// ---------------------------------------------------------------------------
// Function: default ctor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    Initialize ();
}

CXSType::CXSType (int numDimensions)
// ---------------------------------------------------------------------------
// Function: overloaded ctor
// Input:    # of cross-sectional dimensions
// Output:   none
// ---------------------------------------------------------------------------
{
    Initialize ();
    m_numDimensions = numDimensions;
    m_fVDimensions.SetSize (m_numDimensions);
}

void CXSType::Initialize ()
// ---------------------------------------------------------------------------
// Function: initializes all the member variables with default values
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    m_numDimensions = 0;
    m_fArea = 0.0f;
    m_fIyy = 0.0f;
    m_fIzz = 0.0f;
    m_fSzz = 0.0f;
    m_fSyy = 0.0f;
    m_fSFzz = 0.0f;
    m_fSFyy = 0.0f;
}

CXSType::~CXSType ()
// ---------------------------------------------------------------------------
// Function: dtor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
}

void CXSType::GetProperties(float& fArea, float& fIyy, float& fIzz, float& fSzz, float& fSyy, float& fSFzz, float& fSFyy)
// ---------------------------------------------------------------------------
// Function: gets the computed cross-sectional properties
// Input:    variables to hold area, Iyy, Izz, Syy, Szz, SFyy, SFzz
// Output:   area, Iyy, Izz, Syy, Szz, SFyy, SFzz values
// ---------------------------------------------- -----------------------------
{
    ComputeProperties();
    fArea = m_fArea;
    fIyy = m_fIyy;
    fIzz = m_fIzz;
    fSzz = m_fSzz;
    fSyy = m_fSyy;
    fSFzz = m_fSFzz;
    fSFyy = m_fSFyy;
}

void CXSType::GetType(EPType& Type) const
// ---------------------------------------------------------------------------
// Function: gets the type of xs
// Input:    string to hold the type
// Output:   type of xs string
// ---------------------------------------------------------------------------
{
    Type = m_Type;
}

void CXSType::GetDimensions (CVector<float>& fV) const
// ---------------------------------------------------------------------------
// Function: gets the cross-sectional dimensions
// Input:    vector to hold x/s dimensions
// Output:   x/s dimensions
// ---------------------------------------------------------------------------
{
    for (int i=1; i <= m_numDimensions; i++)
        fV(i) = m_fVDimensions(i);
}
