/*********************************************
Program Planar Frame
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis

Implementation of the CNodalResponse class.

*********************************************/
#include "NodalResponse.h"

CNodalResponse::CNodalResponse ()
// ---------------------------------------------------------------------------
// Function: default constructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    m_fVDisp.SetSize (DOFPN);
}

CNodalResponse::~CNodalResponse ()
// ---------------------------------------------------------------------------
// Function: destructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
}

void CNodalResponse::GetValues (CVector<float>& fV) const
// ---------------------------------------------------------------------------
// Function: gets the nodal displacements at the current node
// Input:    vector to hold the nodal displacement values
// Output:   nodal displacement values
// ---------------------------------------------------------------------------
{
    for (int i=1; i <= DOFPN; i++)
        fV(i) = m_fVDisp(i);
}

void CNodalResponse::SetValues (const CVector<float>& fV)
// ---------------------------------------------------------------------------
// Function: sets the nodal displacements at the current node
// Input:    vector holding the nodal displacement values
// Output:   none
// ---------------------------------------------------------------------------
{
    for (int i=1; i <= DOFPN; i++)
        m_fVDisp(i) = fV(i);
}

void CNodalResponse::GetReactions(float& fRX, float& fRY, float& fRZ) const
// ---------------------------------------------------------------------------
// Function: gets x and y  element reactions for 3D
// Input:    variables to hold x-displacement, y-displacement values
// Output:   x-displacement, y-displacement values
// ---------------------------------------------------------------------------
{
    fRX = m_fRX;
    fRY = m_fRY;
    fRZ = m_fRZ;
}

void CNodalResponse::SetReactions(const float fRX, const float fRY, const float fRZ)
// ---------------------------------------------------------------------------
// Function: sets x and y element reactions for 3D
// Input:    variables holding x-displacement, y-displacement values
// Output:   modified x-displacement, y-displacement values
// ---------------------------------------------------------------------------
{
    m_fRX = fRX;
    m_fRY = fRY;
    m_fRZ = fRZ;
}