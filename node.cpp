/*********************************************
Program Planar Frame
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis

Implementation of the CNode class.

*********************************************/
#include "Node.h"

CNode::CNode ()
// ---------------------------------------------------------------------------
// Function: default constructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    m_fVCoor.SetSize (NDIM);   m_fVCoor.Set (0.0f);
    m_VFC.SetSize (DOFPN);     m_VFC.Set (Fixity::FREE);
    m_fVDisp.SetSize (DOFPN);  m_fVDisp.Set (0.0f);
}

CNode::CNode (const CNode& NO)
// ---------------------------------------------------------------------------
// Function: copy constructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    m_fVCoor = NO.m_fVCoor;
    m_VFC = NO.m_VFC;   
    m_fVDisp = NO.m_fVDisp;
}

CNode::~CNode ()
// ---------------------------------------------------------------------------
// Function: destructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
}

void CNode::GetCoords (CVector<float>& fV) const
// ---------------------------------------------------------------------------
// Function: gets the nodal coordinates at the current node
// Input:    vector to hold the nodal coordinates
// Output:   nodal coordinates
// ---------------------------------------------------------------------------
{
    for (int i=1; i <= NDIM; i++)
        fV(i) = m_fVCoor(i);
}

void CNode::SetCoords (const CVector<float>& fV)
// ---------------------------------------------------------------------------
// Function: sets the nodal coordinates at the current node
// Input:    vector holding the nodal coordinates
// Output:   none
// ---------------------------------------------------------------------------
{
    for (int i=1; i <= NDIM; i++)
        m_fVCoor(i) = fV(i);
}

void CNode::GetFixity (CVector<Fixity>& VFC, CVector<float>& fVDisp) const
// ---------------------------------------------------------------------------
// Function: gets the nodal fixity conditions at the current node
// Input:    vector to hold the nodal fixities
// Output:   nodal fixities
// ---------------------------------------------------------------------------
{
    for (int i = 1; i <= DOFPN; i++) {
        VFC(i) = m_VFC(i);
        fVDisp(i) = m_fVDisp(i);
    }
}

void CNode::SetFixity (const CVector<Fixity>& VFC, CVector<float>& fVDisp)
// ---------------------------------------------------------------------------
// Function: sets the nodal fixity conditions at the current node
// Input:    vector holding the nodal fixities
// Output:   none
// ---------------------------------------------------------------------------
{
    for (int i = 1; i <= DOFPN; i++) {
        m_VFC(i) = VFC(i);
        m_fVDisp(i) = fVDisp(i);
    }
}

std::ostream &operator<< (std::ostream& ofs, const CNode::Fixity& FC)
// ---------------------------------------------------------------------------
// Function: non-class function to display the nodal fixity as a text string
// Input:    output stream, fixity condition
// Output:   none
// ---------------------------------------------------------------------------
{
    if (FC == CNode::Fixity::FREE)
        ofs << "Free";
    else
        ofs << "Specified";

    return ofs;
}
