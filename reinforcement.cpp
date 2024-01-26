/**********************************************
Concrete and Steel Cross-Section Design Program

Created by: Aaron Fairchild
Based on the work of: S.B. Rajan
Date: Spring 2024

TODO:

**********************************************/
#include "reinforcement.h"
#include <sstream>

CReinforcement::CReinforcement ()
// ---------------------------------------------------------------------------
// Function: Default constructor
// Input:    None
// Output:   None
// ---------------------------------------------------------------------------
{
    m_fRow = m_fNumBars = m_fSpacing = m_fHEdge = m_fVEdge = 0.0f;
}

CReinforcement::~CReinforcement ()
// ---------------------------------------------------------------------------
// Function: Destructor
// Input:    None
// Output:   None
// ---------------------------------------------------------------------------
{
}

void CReinforcement::SetXSReData(const std::string& strType, const float& fRow,
                                 const float& fNumBars, const std::string& strSize,
                                 const std::string& strSpacing, const float& fHEdge,
                                 const float& fVEdge)
// ---------------------------------------------------------------------------
// Function: Sets the cross-sectional reinforcement data
// Input:    cross-sectional reinforcement data
// Output:   None
// ---------------------------------------------------------------------------
{
    m_strType = strType;
    m_fRow = fRow;
    m_fNumBars = fNumBars;
    m_strSize = strSize;
    m_strSpacing = strSpacing;
    m_fHEdge = fHEdge;
    m_fVEdge = fVEdge;
}

void CReinforcement::SetXSReData(const std::string& strType, const float& fRow,
    const float& fNumBars, const std::string& strSize,
    const float& fSpacing, const float& fHEdge,
    const float& fVEdge)
    // ---------------------------------------------------------------------------
    // Function: Sets the cross-sectional reinforcement data
    // Input:    cross-sectional reinforcement data
    // Output:   None
    // ---------------------------------------------------------------------------
{
    m_strType = strType;
    m_fRow = fRow;
    m_fNumBars = fNumBars;
    m_strSize = strSize;
    m_fSpacing = fSpacing;
    m_fHEdge = fHEdge;
    m_fVEdge = fVEdge;
}