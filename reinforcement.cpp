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
    m_fVCoords.Set(0.0f);
    m_fVSizes.Set(0.0f);
    m_fVReData.Set(0.0f);
}

CReinforcement::~CReinforcement ()
// ---------------------------------------------------------------------------
// Function: Destructor
// Input:    None
// Output:   None
// ---------------------------------------------------------------------------
{
}

void CReinforcement::SetSize(float fSize)
// ---------------------------------------------------------------------------
// Function: Sets the input data vector size
// Input:    needed size
// Output:   None
// ---------------------------------------------------------------------------
{
    m_fVReData.SetSize(fSize);
}

void CReinforcement::SetXSReData(CVector<float> fVReData)
// ---------------------------------------------------------------------------
// Function: Sets the cross-sectional reinforcement data
// Input:    cross-sectional reinforcement data
// Output:   None
// ---------------------------------------------------------------------------
{
    m_fVReData = fVReData;
}

float CReinforcement::GetArea()
// ---------------------------------------------------------------------------
// Function: Gets the total area of the reinforcing material
// Input:    None
// Output:   Total area of reinforcement
// ---------------------------------------------------------------------------
{
    m_fArea = 0.0f;
    return m_fArea;
}
