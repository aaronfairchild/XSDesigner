/*********************************************
Program Planar Frame
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis

Implementation of the CElementResponse class.

*********************************************/
#include "ElementResponse.h"

CElementResponse::CElementResponse ()
// ---------------------------------------------------------------------------
// Function: default constructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    m_fVFStartNode.SetSize (DOFPN);  m_fVFStartNode.Set(0.0f);
    m_fVFEndNode.SetSize (DOFPN);    m_fVFEndNode.Set(0.0f);
}

CElementResponse::~CElementResponse ()
// ---------------------------------------------------------------------------
// Function: destructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
}

void CElementResponse::GetValues (CVector<float>& fVFSN,
                                  CVector<float>& fVFEN) const
// ---------------------------------------------------------------------------
// Function: gets the element force response
// Input:    vectors to hold element forces at start and end nodes
// Output:   the element force values
// ---------------------------------------------------------------------------
{
    for (int i=1; i <= DOFPN; i++)
    {
        fVFSN(i) = m_fVFStartNode(i);
        fVFEN(i) = m_fVFEndNode(i);
    }
}

void CElementResponse::SetValues (const CVector<float>& fVFSN,
                                  const CVector<float>& fVFEN)
// ---------------------------------------------------------------------------
// Function: gets the element force response
// Input:    vectors to hold element forces at start and end nodes
// Output:   none
// ---------------------------------------------------------------------------
{
    for (int i=1; i <= DOFPN; i++)
    {
        m_fVFStartNode(i) = fVFSN(i);
        m_fVFEndNode (i) = fVFEN(i);
    }
}

void CElementResponse::GetStresses(float& fCompStress, float& fTensStress, 
                                   float& fShearStress) const
    // ---------------------------------------------------------------------------
    // Function: gets the element max stress response
    // Input:    variables to hold stress values
    // Output:   the element stress values
    // ---------------------------------------------------------------------------
{
    fCompStress = m_fCompStress;
    fTensStress = m_fTensStress;
    fShearStress = m_fShearStress;
}

void CElementResponse::SetStresses(float& fCompStress, float& fTensStress,
                                   float& fShearStress)
    // ---------------------------------------------------------------------------
    // Function: sets the element stress response
    // Input:    variables to hold element stresses
    // Output:   none
    // ---------------------------------------------------------------------------
{
    m_fCompStress = fCompStress;
    m_fTensStress = fTensStress;
    m_fShearStress = fShearStress;
}