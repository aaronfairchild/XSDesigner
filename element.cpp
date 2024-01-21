/*********************************************
Program Planar Frame
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis

Implementation of the CElement class.

*********************************************/
#include "element.h"

CElement::CElement ()
// ---------------------------------------------------------------------------
// Function: default constructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    m_nSN = m_nEN = m_nMPGroup = 0;
    m_pEPGroup = NULL;
    m_fLength = 0.0f;
}

CElement::CElement (const CElement& EO)
// ---------------------------------------------------------------------------
// Function: copy constructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    m_nSN = EO.m_nSN;
    m_nEN = EO.m_nEN;
    m_nMPGroup = EO.m_nMPGroup;
    m_pEPGroup = EO.m_pEPGroup;
    m_fLength = EO.m_fLength;
}

CElement::~CElement ()
// ---------------------------------------------------------------------------
// Function: dtor 
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
}

void CElement::GetENodes (int& nSN, int& nEN) const
// ---------------------------------------------------------------------------
// Function: obtains the element start and end node numbers
// Input:    variables to hold the values
// Output:   the start and end node numbers
// ---------------------------------------------------------------------------
{
    nSN = m_nSN;
    nEN = m_nEN;
}

void CElement::SetENodes (const int nSN, const int nEN) 
// ---------------------------------------------------------------------------
// Function: sets the element start and end node numbers
// Input:    the start and end node numbers
// Output:   none
// ---------------------------------------------------------------------------
{
    m_nSN = nSN;
    m_nEN = nEN;
}

void CElement::SetEPropertyGroup (CXSType* pEPG)
// ---------------------------------------------------------------------------
// Function: sets the pointer to the element property group associated
//           with the element
// Input:    element property pointer 
// Output:   none
// ---------------------------------------------------------------------------
{
    m_pEPGroup = pEPG;
}

CXSType* CElement::GetEPropertyGroup () const
// ---------------------------------------------------------------------------
// Function: gets the pointer to the element property group associated
//           with the element
// Input:    none
// Output:   returns the element property pointer
// ---------------------------------------------------------------------------
{
    return m_pEPGroup;
}

void CElement::SetMatPropertyGroup (const int nMPG)
// ---------------------------------------------------------------------------
// Function: sets the material group group # associated with the element
// Input:    material group #
// Output:   none
// ---------------------------------------------------------------------------
{
    m_nMPGroup = nMPG;
}

int CElement::GetMatPropertyGroup () const
// ---------------------------------------------------------------------------
// Function: gets the material group group # associated with the element
// Input:    none
// Output:   returns the material group #
// ---------------------------------------------------------------------------
{
    return m_nMPGroup;
}

void CElement::GetLength(float& fLength) const
// ---------------------------------------------------------------------------
// Function: obtains the element start and end node numbers
// Input:    variables to hold the values
// Output:   the start and end node numbers
// ---------------------------------------------------------------------------
{
    fLength = m_fLength;
}

void CElement::SetLength(const float fLength)
// ---------------------------------------------------------------------------
// Function: sets the element start and end node numbers
// Input:    the start and end node numbers
// Output:   none
// ---------------------------------------------------------------------------
{
    m_fLength = fLength;
}
