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
    void SetXSReData(CVector<float> fVReData);
    void SetSize(float fSize);
    float GetArea();

    // Parse and set Transverse reinforcement data

    // Accessor functions


private:
    CVector<float> m_fVReData, m_fVSizes, m_fVCoords;
    float m_fArea;
};
