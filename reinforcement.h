/**********************************************
Concrete and Steel Cross-Section Design Program

Created by: Aaron Fairchild
Based on the work of: S.B. Rajan
Date: Spring 2024

TODO:

**********************************************/
#pragma once
#include "..\libraryEXH\arraycontainersEXH.h"
#include <string>

class CReinforcement
{

public:
    CReinforcement();   // ctor
    ~CReinforcement();  // dtor

    // Parse and set XS reinforcement data
    void SetXSReData(const std::string& strType, const float& fRow,
        const float& fNumBars, const std::string& strSize,
        const std::string& strSpacing, const float& fHEdge,
        const float& fVEdge);

    void SetXSReData(const std::string& strType, const float& fRow,
        const float& fNumBars, const std::string& strSize,
        const float& fSpacing, const float& fHEdge,
        const float& fVEdge);

    // Parse and set Transverse reinforcement data

    // Accessor functions


private:
    std::string m_strType, m_strSize, m_strSpacing;
    float m_fRow, m_fNumBars, m_fSpacing, m_fHEdge, m_fVEdge;
};
