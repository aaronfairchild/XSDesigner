/**********************************************
Concrete and Steel Cross-Section Design Program

Created by: Aaron Fairchild
Based on the work of: S.B. Rajan
Date: Spring 2024

TODO:

**********************************************/
#include <cmath>
#include <iostream>
#include "tsection.h"

CTSection::CTSection (const CVector<float>& fV) : 
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
    m_Type = EPType::TSECTION;
}

CTSection::~CTSection ()
// ---------------------------------------------------------------------------
// Function: dtor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
}

void CTSection::ComputeProperties ()
// ---------------------------------------------------------------------------
// Function: Computes the I-section properties
// Input:    None
// Output:   None
// ---------------------------------------------------------------------------
{
    // total depth (H)
    float H = m_fVDimensions(1);
    // flange width (b)
    float b = m_fVDimensions(2);
    // web thickness (bw)
    float bw = m_fVDimensions(3);
    // flange thickness (tf)
    float tf = m_fVDimensions(4);

    // cross-sectional area
    m_fArea = b * tf + bw * (H - tf);

    // Distance from bottom to the centroid
    float yBar = (b * tf * (H - tf / 2) + bw * (H - tf) * ((H - tf) / 2)) / m_fArea;

    // Moment of Inertia (MOI) about y-axis
    float Iyy_flange = (b * pow(tf, 3)) / 12.0;
    float Iyy_web = (bw * pow(H - tf, 3)) / 12.0;
    // Applying parallel axis theorem for flange
    Iyy_flange += b * tf * pow((H - yBar - tf / 2), 2);
    m_fIyy = Iyy_flange + Iyy_web;

    // MOI about z-axis
    float Izz_flange = (tf * pow(b, 3)) / 12.0;
    float Izz_web = (H - tf) * pow(bw, 3) / 12.0;
    m_fIzz = Izz_flange + Izz_web;

    // Section Modulus z-axis
    m_fSzz = m_fIzz / (H - yBar);
    // Section Modulus y-axis
    m_fSyy = m_fIyy / (b / 2.0);

    // Shear Factor y-axis
    m_fSFyy = (5.0f / 6.0f) * bw;

    // Shear Factor z-axis
    m_fSFzz = (5.0f / 6.0f) * tf;
}