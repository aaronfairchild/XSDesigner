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
#include "MatToolBox.h"
#include "constants.h"
#include "clockEXH.h"

CElement::CElement ()
// ---------------------------------------------------------------------------
// Function: default constructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    m_nDOF = m_nLineNumber = m_nElementLoads = 0;
    m_nDebugLevel = 0;
    m_strDelimiters = "\t, "; // tab space and comma delimited file
    m_strComment = "**";
    m_fLength = 0.0f;
    m_Type = CElement::ElementType::COLUMN;
}

CElement::~CElement ()
// ---------------------------------------------------------------------------
// Function: dtor 
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
}

void CElement::Analyze()
// ---------------------------------------------------------------------------
// Function: Reads the input data and analyzes the frame
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // Start Timer
    Timer.MarkTime();
    Timer.GetDateTime(m_strDateTime);

    // read the problem size
    ReadProblemSize();

    // set problem size
    SetSize();

    // read nodal and element data
    ReadFrameModel();

    // create output file
    Timer.GetDateTime(m_strEndDateTime);
    CreateOutput();
}

void CElement::SetSize()
// ---------------------------------------------------------------------------
// Function: memory allocation for all major arrays in the program
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // allocate space for nodal loads data
    m_ElementLoadData.SetSize(m_nElementLoads);
    m_NodalLoadData.SetSize(2);
    m_ConcMatData.SetSize(1);
    m_ReData.SetSize(m_nXSR);
    m_ReMatData.SetSize(1);
    m_EPData.SetSize(1);
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

CElement::ElementType CElement::GetType() const
// ---------------------------------------------------------------------------
// Function: Gets the element type
// Input:    none
// Output:   the start and end node numbers
// ---------------------------------------------------------------------------
{
    return m_Type;
}

void CElement::SetType(CElement::ElementType Type)
// ---------------------------------------------------------------------------
// Function: sets the element start and end node numbers
// Input:    the start and end node numbers
// Output:   none
// ---------------------------------------------------------------------------
{
    m_Type = Type;
}

void CElement::ErrorHandler(CLocalErrorHandler::ERRORCODE ErrorCode)
{
    throw ErrorCode;
}

void CElement::ErrorHandler(CGlobalErrorHandler::ERRORCODE ErrorCode) const
{
    throw ErrorCode;
}

void CElement::DisplayErrorMessage(CLocalErrorHandler::ERRORCODE err)
{
    m_LEH.ErrorHandler(err, m_nLineNumber);
}