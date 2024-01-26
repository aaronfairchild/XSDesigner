/**********************************************
Concrete and Steel Cross-Section Design Program

Created by: Aaron Fairchild
Based on the work of: S.B. Rajan
Date: Spring 2024

TODO:

**********************************************/
#include "material.h"

CMaterial::CMaterial ()
// ---------------------------------------------------------------------------
// Function: Default constructor
// Input:    None
// Output:   None
// ---------------------------------------------------------------------------
{
    m_fYM = 0.0f;
    m_fCompStr = 0.0f;
    m_fYieldStr = 0.0f;
    m_fDensity = 0.0f;
    m_fPoisson = 0.0f;
}

CMaterial::~CMaterial ()
// ---------------------------------------------------------------------------
// Function: Destructor
// Input:    None
// Output:   None
// ---------------------------------------------------------------------------
{
}

float CMaterial::GetYM () const
// ---------------------------------------------------------------------------
// Function: Gets the young's modulus value
// Input:    None
// Output:   Returns the YM value
// ---------------------------------------------------------------------------
{
    return m_fYM;
}

float CMaterial::GetCompStr () const
// ---------------------------------------------------------------------------
// Function: Gets the compressive strength value
// Input:    None
// Output:   Returns the compressive strength
// ---------------------------------------------------------------------------
{
    return m_fCompStr;
}

float CMaterial::GetYieldStr() const
// ---------------------------------------------------------------------------
// Function: Gets the yield strength value
// Input:    None
// Output:   Returns the yield strength
// ---------------------------------------------------------------------------
{
    return m_fYieldStr;
}

float CMaterial::GetDensity() const
// ---------------------------------------------------------------------------
// Function: Gets the density value
// Input:    None
// Output:   Returns the density value
// ---------------------------------------------------------------------------
{
    return m_fDensity;
}

float CMaterial::GetPoisson() const
// ---------------------------------------------------------------------------
// Function: Gets Poisson's ratio value
// Input:    None
// Output:   Returns the poissons ratio value
// ---------------------------------------------------------------------------
{
    return m_fPoisson;
}

void CMaterial::SetYM (const float fYM)
// ---------------------------------------------------------------------------
// Function: Sets the young's modulus value
// Input:    YM value
// Output:   None
// ---------------------------------------------------------------------------
{
    m_fYM = fYM;
}

void CMaterial::SetCompStr (const float fCompStr)
// ---------------------------------------------------------------------------
// Function: Sets the compressive strength value
// Input:    Compressive strength value
// Output:   None
// ---------------------------------------------------------------------------
{
    m_fCompStr = fCompStr;
}

void CMaterial::SetYieldStr(const float fYieldStr)
// ---------------------------------------------------------------------------
// Function: Sets the yield strength value
// Input:    Yield strength value
// Output:   None
// ---------------------------------------------------------------------------
{
    m_fYieldStr = fYieldStr;
}

void CMaterial::SetDensity(const float fDensity)
// ---------------------------------------------------------------------------
// Function: Sets the density value
// Input:    Density value
// Output:   None
// ---------------------------------------------------------------------------
{
    m_fDensity = fDensity;
}

void CMaterial::SetPoisson(const float fPoisson)
// ---------------------------------------------------------------------------
// Function: Sets the poisson's ratio value
// Input:    Poisson's ratio value
// Output:   None
// ---------------------------------------------------------------------------
{
    m_fPoisson = fPoisson;
}