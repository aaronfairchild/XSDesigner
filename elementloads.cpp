/*********************************************
Program Planar Frame
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis

Implementation of the CElementLoads class.

*********************************************/
#include "ElementLoads.h"

CElementLoads::CElementLoads ()
// ---------------------------------------------------------------------------
// Function: default constructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    m_nElement = 0;
    m_Type = ELType::CONCENTRATEDY;
    m_fValue1 = m_fValue2 = 0.0f;
}

CElementLoads::CElementLoads (const CElementLoads& ELO)
// ---------------------------------------------------------------------------
// Function: copy constructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    m_nElement = ELO.m_nElement;
    m_Type = ELO.m_Type;
    m_fValue1 = ELO.m_fValue1;
    m_fValue2 = ELO.m_fValue2;
}

CElementLoads::~CElementLoads ()
// ---------------------------------------------------------------------------
// Function: destructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
}

void CElementLoads::GetValues (int& nE, ELType& Type,
                               float& f1, float& f2) const
// ---------------------------------------------------------------------------
// Function: gets the element load information
// Input:    element #, load type, 2 load associated values
// Output:   values for all these variables
// ---------------------------------------------------------------------------
{
    nE = m_nElement;
    Type = m_Type;
    f1 = m_fValue1;
    f2 = m_fValue2;
}

void CElementLoads::SetValues (const int nE, const ELType Type,
                               const float f1, const float f2)
// ---------------------------------------------------------------------------
// Function: sets the element load information
// Input:    element #, load type, 2 load associated values
// Output:   none
// ---------------------------------------------------------------------------
{
    m_nElement = nE;
    m_Type = Type;
    m_fValue1 = f1;
    m_fValue2 = f2;
}

void CElementLoads::GetENF (CVector<float>& fV1, CVector<float>& fV2,
                            const float fLength) const
// ---------------------------------------------------------------------------
// Function: Gets the equivalent nodal forces
// Input:    Vectors to store ENF at start node and end node, element length
// Output:   The two vectors suitably populated
// ---------------------------------------------------------------------------
{
    fV1.Set(0.0f); fV2.Set(0.0f);
    if (m_Type == ELType::DISTRIBUTEDY) {
        float fwl = m_fValue1;
        float fwr = m_fValue2;

        // y'-component
        fV1(2) = (fLength / 20.0f) * (7.0f * fwl + 3.0f * fwr);
        fV2(2) = (fLength / 20.0f) * (3.0f * fwl + 7.0f * fwr);
        // z' moment-component
        fV1(3) = (fLength * fLength / 60.0f) * (3.0f * fwl + 2.0f * fwr);
        fV2(3) = -1.0f * (fLength * fLength / 60.0f) * (2.0f * fwl + 3.0f * fwr);
    }
    else if (m_Type == ELType::CONCENTRATEDY) {
        float fa = m_fValue1;
        float fb = fLength - fa;
        float fP = m_fValue2;

        // y'-component
        fV1(2) = (fP * fb * fb * (fLength + 2.0f * fa)) / (pow(fLength, 3.0f));
        fV2(2) = (fP * fa * fa * (fLength + 2.0f * fb)) / (pow(fLength, 3.0f));
        // z' moment-component
        fV1(3) = (fP * fa * fb * fb) / (fLength * fLength);
        fV2(3) = -1.0f * (fP * fa * fa * fb) / (fLength * fLength);
    }
    else if (m_Type == ELType::CONCENTRATEDX) {
        float fa = m_fValue1;
        float fb = fLength - fa;
        float fP = m_fValue2;

        // x'-component
        fV1(1) = (fP * fb) / (fLength);
        fV2(1) = (fP * fa) / (fLength);
    }
    else if (m_Type == ELType::CONCENTRATEDM) {
        float fa = m_fValue1;
        float fb = fLength - fa;
        float fM = m_fValue2;

        // y'-component
        fV1(2) = (-6.0f * fM * fa * fb) / (pow(fLength, 3));
        fV2(2) = (6.0f * fM * fa * fb) / (pow(fLength, 3));
        // z' moment-component
        fV1(3) = -1.0f * (fM * fb * (2.0f * fa - fb)) / (fLength * fLength);
        fV2(3) = (fM * fa * (fa - 2.0f * fb)) / (fLength * fLength);
    }
}

