/*********************************************
Planar Frame Analysis Program
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis
*********************************************/
#include "frame.h"
#include "MatToolBox.h"
#include "constants.h"
#include "clockEXH.h"


/* ==================================================================
   ======================= CFrame class =============================
   ================================================================== */

CFrame::CFrame ()
// ---------------------------------------------------------------------------
// Function: default constructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    m_nNodes = m_nElements = m_nEPGroups = m_nMatGroups
             = m_nDOF = m_nLineNumber
             = m_nElementLoads = 0;
    m_nDebugLevel = 0;
    m_strDelimiters = "\t, "; // tab space and comma delimited file
    m_strComment    = "**";
}

CFrame::~CFrame ()
// ---------------------------------------------------------------------------
// Function: destructor
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // deallocate ... deallocate ... deallocate
    for (int i=1; i <= m_nEPGroups; i++)
    {
        delete m_EPData(i); 
    }
}

void CFrame::Analyze ()
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
	ReadProblemSize ();

	// set problem size
	SetSize ();

	// read nodal and element data
	ReadFrameModel ();

	// construct system equations
	ConstructK ();
	ConstructF ();

    // Initialize Residual Vector
    CVector<int> nE(DOFPE);
    CVector<float> fNL(DOFPN);
    for (int i = 1; i <= m_nNodes; i++) {

        m_NodalLoadData(i).GetValues(fNL);
        nE(1) = DOFPN * i - 2; nE(2) = DOFPN * i - 1; nE(3) = DOFPN * i;
        for (int j = 1; j <= DOFPN; j++) {
            int nRow = nE(j);
            m_dR(nRow) += -1.0 * fNL(j);
        }
    }
    // debug?
    if (m_nDebugLevel == 1)
    {
        m_MTBDP.PrintVector(m_dR, "Initial Residual Vector",
            m_FileOutput);
    }

	// impose boundary conditions
	ImposeBC ();

	// solve for the nodal displacements
	Solve ();

	// compute secondary unknowns
	Response ();

    // Residual
    Residual();

	// create output file
    Timer.GetDateTime(m_strEndDateTime);
	CreateOutput ();
}

void CFrame::SetSize ()
// ---------------------------------------------------------------------------
// Function: memory allocation for all major arrays in the program
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // allocate space for nodal data
    m_NodalData.SetSize (m_nNodes);
    // allocate space for nodal loads data
    m_NodalLoadData.SetSize (m_nNodes);
    // allocate space for material data
    m_MaterialData.SetSize (m_nMatGroups);
    // allocate space for x/s data
    m_EPData.SetSize (m_nEPGroups);
    // allocate space for nodal response data
    m_NodalResponseData.SetSize (m_nNodes);
    // allocate space for element data
    m_ElementData.SetSize (m_nElements);
    // allocate space for element response data
    m_ElementResponseData.SetSize (m_nElements);
    // allocate space for element loads
    m_ElementLoadData.SetSize(m_nElementLoads);
    m_ELL.SetSize(DOFPE, m_nElements, 0.0f);
    m_ELG.SetSize(DOFPE, m_nElements, 0.0f);


    // allocate and initialize major matrices
    m_nDOF = DOFPN*m_nNodes;
    m_SSM.SetSize (m_nDOF, m_nDOF, 0.0);
    m_SND.SetSize (m_nDOF, 0.0);
    m_SNF.SetSize (m_nDOF, 0.0);
    m_dR.SetSize(m_nDOF, 1);
    m_dR.Set(0.0);
    for (int i = 1; i <= m_nNodes; i++)
        m_NodalResponseData(i).SetReactions(0.0f, 0.0f, 0.0f);
}

void CFrame::ConstructF ()
// ---------------------------------------------------------------------------
// Function: constructs the system load vector
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    CVector<int> nE(DOFPE);                      // the nodal degrees-of-freedom vector
    /*
    CVector<float> fNL(DOFPN);
    for (int i = 1; i <= m_nNodes; i++)
    {
        m_NodalLoadData(i).GetValues(fNL);
        m_SNF(DOFPN * i - 2,1) += static_cast<double>(fNL(1));
        m_SNF(DOFPN * i - 1,1) += static_cast<double>(fNL(2));
        m_SNF(DOFPN * i,1) += static_cast<double>(fNL(3));
    }
    */

    CVector<float> fNL(DOFPN);

    for (int i = 1; i <= m_nNodes; i++) {

        m_NodalLoadData(i).GetValues(fNL);
        nE(1) = DOFPN * i - 2; nE(2) = DOFPN * i - 1; nE(3) = DOFPN * i;
        for (int j = 1; j <= DOFPN; j++) {
            int nRow = nE(j);
            m_SNF(nRow) += fNL(j);
        }
    }

    int nSN, nEN;
    CVector<int> nE2(DOFPE);                      // the nodal degrees-of-freedom vector
    for (int i = 1; i <= m_nElements; i++) {
        m_ElementData(i).GetENodes(nSN, nEN);
        nE2(1) = DOFPN * nSN - 2; nE2(2) = DOFPN * nSN - 1; nE2(3) = DOFPN * nSN;
        nE2(4) = DOFPN * nEN - 2; nE2(5) = DOFPN * nEN - 1; nE2(6) = DOFPN * nEN;
        for (int j = 1; j <= DOFPE; j++) {
            int nRow = nE2(j);
            m_SNF(nRow) += m_ELG(j, i);
        }
    }
    // debug?
    if (m_nDebugLevel == 1)
    {
        m_MTBDP.PrintVector(m_SNF, "Force vector",
            m_FileOutput);
    }

}

void CFrame::ConstructK ()
// ---------------------------------------------------------------------------
// Function: constructs the system stiffness matrix
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    int nSN, nEN, nMatGrp;
    CXSType* pXSGrp;
    float fArea, fIyy, fIzz, fSyy, fSzz, fSFyy, fSFzz;
    CVector<float> fVC1(NDIM), fVC2(NDIM);
    CVector<int> nE(DOFPE);                      // the nodal degrees-of-freedom vector

    for (int i=1; i <= m_nElements; i++)
    {
        m_ElementData(i).GetENodes (nSN, nEN);
        m_NodalData(nSN).GetCoords (fVC1);
        m_NodalData(nEN).GetCoords (fVC2);
        pXSGrp = m_ElementData(i).GetEPropertyGroup ();
        nMatGrp = m_ElementData(i).GetMatPropertyGroup ();
        pXSGrp->GetProperties(fArea, fIyy, fIzz, fSyy, fSzz, fSFyy, fSFzz);
        float fYM = m_MaterialData(nMatGrp).GetYM ();
        float fCTE = m_MaterialData(nMatGrp).GetCTE ();
        float fLength;
        m_ElementData(i).GetLength(fLength);
        
        float fI = (fIyy > fIzz ? fIyy : fIzz);
        float fS = (fIyy > fIzz ? fSyy : fSzz);
        float fSF = (fIyy > fIzz ? fSFzz : fSFyy);

        double dArea = static_cast<double>(fArea);
        double dLength = static_cast<double>(fLength);
        double dI = static_cast<double>(fI);
        double dYM = static_cast<double>(fYM);
        double x1 = static_cast<double>(fVC1(1));
        double y1 = static_cast<double>(fVC1(2));
        double x2 = static_cast<double>(fVC2(1));
        double y2 = static_cast<double>(fVC2(2));
        double l = (x2 - x1) / fLength;
        double m = (y2 - y1) / fLength;

        // compute k(6x6)
        double a = (dArea * dYM) / dLength;
        double b = (12 * dYM * dI) / (pow(dLength, 3));
        double c = (6 * dYM * dI) / (dLength * dLength);
        double d = (2 * dYM * dI) / dLength;

        CMatrix<double> dkg(6, 6);
        dkg(1, 1) = dkg(4, 4) = a * l * l + b * m * m;
        dkg(2, 2) = dkg(5, 5) = a * m * m + b * l * l;
        dkg(3, 3) = dkg(6, 6) = 2 * d;
        dkg(1, 2) = dkg(2, 1) = dkg(5, 4) = dkg(4, 5) = (a - b) * l * m;
        dkg(1, 3) = dkg(3, 1) = dkg(1, 6) = dkg(6, 1) = -1 * c * m;
        dkg(2, 3) = dkg(3, 2) = dkg(2, 6) = dkg(6, 2) = c * l;
        dkg(1, 4) = dkg(4, 1) = -1 * (a * l * l + b * m * m);
        dkg(2, 4) = dkg(4, 2) = dkg(1, 5) = dkg(5, 1) = -1 * (a - b) * l * m;
        dkg(3, 4) = dkg(4, 3) = dkg(4, 6) = dkg(6, 4) = c * m;
        dkg(2, 5) = dkg(5, 2) = -1 * (a * m * m + b * l * l);
        dkg(3, 5) = dkg(5, 3) = dkg(5, 6) = dkg(6, 5) = -1 * c * l;
        dkg(3, 6) = dkg(6, 3) = d;
        if (m_nDebugLevel == 1)
        {
            m_MTBDP.PrintMatrixRowWise(dkg, "k(6x6):",
                m_FileOutput);
        }

        nE(1) = DOFPN * nSN - 2; nE(2) = DOFPN * nSN - 1; nE(3) = DOFPN * nSN;
        nE(4) = DOFPN * nEN - 2; nE(5) = DOFPN * nEN - 1; nE(6) = DOFPN * nEN;

        for (int j = 1; j <= DOFPE; j++)
        {
            int nRow = nE(j);
            for (int k = 1; k <= DOFPE; k++)
            {
                int nCol = nE(k);
                m_SSM(nRow, nCol) += dkg(j, k);
            }
        }
    }
    // debug?
    if (m_nDebugLevel == 1)
    {
        m_MTBDP.PrintMatrixRowWise(m_SSM, "Structural Stiffness (Before BCs)",
            m_FileOutput);
    }

}

void CFrame::ImposeBC ()
// ---------------------------------------------------------------------------
// Function: imposes the homogenous and non-homogenous EBC
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    CVector<int> fVGDOF(DOFPN * m_nNodes);
    CVector<CNode::Fixity> VFC1(DOFPN);
    CVector<float> fVC(DOFPN);      // Stores the specified displacements
    fVGDOF.Set(0); 
    int n = 1;
    for (int i = 1; i <= m_nNodes; i++) {

        m_NodalData(i).GetFixity(VFC1, fVC);

        if (VFC1(1) == CNode::Fixity::SPECIFIED) {
            n = DOFPN * i - 2;
            SuppressDOF(n, fVC(1));
        }
        if (VFC1(2) == CNode::Fixity::SPECIFIED) {
            n = DOFPN * i - 1;
            SuppressDOF(n, fVC(2));
        }
        if (VFC1(3) == CNode::Fixity::SPECIFIED) {
            n = DOFPN * i;
            SuppressDOF(n, fVC(3));
        }
    }
    if (m_nDebugLevel == 1)
    {
        m_MTBDP.PrintMatrixRowWise(m_SSM, "Structural Stiffness (After BCs)",
            m_FileOutput);
        m_MTBDP.PrintVector(m_SNF, "Force vector after BC:",
            m_FileOutput);
    }
}

void CFrame::SuppressDOF (const int nEqn, const float fc)
// ---------------------------------------------------------------------------
// Function: works in conjunction with ImposeBC
// Input:    global equation # to impose EBC
// Output:   none
// ---------------------------------------------------------------------------
{
    for (int j = 1; j <= m_nDOF; j++)
    {
        // modify RHS
        m_SNF(j) = m_SNF(j) - m_SSM(j, nEqn) * fc;
        // zero out the row
        m_SSM(nEqn, j) = 0.0;
        // zero out the column
        m_SSM(j, nEqn) = 0.0;

    }
    // set known displacement
    m_SNF(nEqn) = fc;
    // set diagonal to 1
    m_SSM(nEqn, nEqn) = 1.0f;
}

void CFrame::Solve ()
// ---------------------------------------------------------------------------
// Function: solves for the nodal displacements
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // implement Gauss Elimination Technique
    double TOL = 1.0e-6;
    CVector<float> dDisps(DOFPN);

    m_MTBDP.LUFactorization(m_SSM, TOL);
    m_MTBDP.LUSolve(m_SSM, m_SND, m_SNF);

    for (int i = 1; i <= m_nNodes; i++)
    {
        dDisps(1) = static_cast<float>(m_SND(DOFPN * i - 2));
        dDisps(2) = static_cast<float>(m_SND(DOFPN * i - 1));
        dDisps(3) = static_cast<float>(m_SND(DOFPN * i));
        m_NodalResponseData(i).SetValues(dDisps);
    }
    if (m_nDebugLevel == 1)
    {
        m_MTBDP.PrintVector(m_SND, "Displacements:",
            m_FileOutput);
    }
}

void CFrame::Response ()
// ---------------------------------------------------------------------------
// Function: computes the element nodal forces and support reactions
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    int nSN, nEN, nMatGrp;
    CXSType* pXSGrp;
    float fArea, fIyy, fIzz, fSyy, fSzz, fSFyy, fSFzz;
    CVector<float> fVC1(NDIM), fVC2(NDIM);
    CVector<int> nE(DOFPE);                      // the nodal degrees-of-freedom vector
    
    // loop thro' all elements
    for (int i = 1; i <= m_nElements; i++)
    {
        m_ElementData(i).GetENodes(nSN, nEN);
        m_NodalData(nSN).GetCoords(fVC1);
        m_NodalData(nEN).GetCoords(fVC2);
        pXSGrp = m_ElementData(i).GetEPropertyGroup();
        nMatGrp = m_ElementData(i).GetMatPropertyGroup();
        pXSGrp->GetProperties(fArea, fIyy, fIzz, fSyy, fSzz, fSFyy, fSFzz);
        float fYM = m_MaterialData(nMatGrp).GetYM();
        float fCTE = m_MaterialData(nMatGrp).GetCTE();
        float fLength;
        m_ElementData(i).GetLength(fLength);

        float fI = (fIyy > fIzz ? fIyy : fIzz);
        float fS = (fIyy > fIzz ? fSyy : fSzz);
        float fSF = (fIyy > fIzz ? fSFzz : fSFyy);

        double dArea = static_cast<double>(fArea);
        double dLength = static_cast<double>(fLength);
        double dI = static_cast<double>(fI);
        double dS = static_cast<double>(fS);
        double dSF = static_cast<double>(fSF);
        double dE = static_cast<double>(fYM);
        double x1 = static_cast<double>(fVC1(1));
        double y1 = static_cast<double>(fVC1(2));
        double x2 = static_cast<double>(fVC2(1));
        double y2 = static_cast<double>(fVC2(2));
        double l = (x2 - x1) / fLength;
        double m = (y2 - y1) / fLength;

        // Get and store displacements at each node
        CVector<double> d(DOFPE); d.Set(0.0);
        CVector<float> dDisp1(DOFPN); CVector<float> dDisp2(DOFPN);
        m_NodalResponseData(nSN).GetValues(dDisp1);
        m_NodalResponseData(nEN).GetValues(dDisp2);
        for (int j = 1; j <= 3; j++) {
            d(j) = static_cast<double>(dDisp1(j));
            d(j + DOFPN) = static_cast<double>(dDisp2(j));
        }

        // Get and store loads at each node
        CVector<double> ql(DOFPE); ql.Set(0.0);
        CVector<float> ql1(DOFPN); CVector<float> ql2(DOFPN);
        for (int j = 1; j <= 6; j++) {
            ql(j) = static_cast<double>(m_ELL(j, i));
        }

        CVector<double> dfL(DOFPE);          // local force vector
        dfL.Set(0.0);

        // compute f(6x1)
        double dA = (dArea * dE) / dLength;
        double dB = (12.0 * dE * dI) / (pow(dLength, 3.0));
        double dC = (6.0 * dE * dI) / (dLength * dLength);
        double dD = (2.0 * dE * dI) / dLength;

        dfL(1) = (dA * (l * (d(1) - d(4)) + m * (d(2) - d(5)))) - ql(1);
        dfL(2) = (dB * (l * (d(2) - d(5)) - m * (d(1) - d(4))) + dC * (d(3) + d(6))) - ql(2);
        dfL(3) = (dC * (l * (d(2) - d(5)) - m * (d(1) - d(4))) + dD * (2 * d(3) + d(6))) - ql(3);
        dfL(4) = (-1 * dA * (l * (d(1) - d(4)) + m * (d(2) - d(5)))) - ql(4);
        dfL(5) = (-1 * dB * (l * (d(2) - d(5)) - m * (d(1) - d(4))) - dC * (d(3) + d(6))) - ql(5);
        dfL(6) = (dC * (l * (d(2) - d(5)) - m * (d(1) - d(4))) + dD * (d(3) + 2 * d(6))) - ql(6);

        // Store in element response
        CVector<float>fVFSN(DOFPN); CVector<float>fVFEN(DOFPN);
        for (int j = 1; j <= DOFPN; j++) {
            fVFSN(j) = static_cast<float>(dfL(j));
            fVFEN(j) = static_cast<float>(dfL(j + DOFPN));
        }
        m_ElementResponseData(i).SetValues(fVFSN, fVFEN);

        if (m_nDebugLevel == 1)
        {
            m_MTBDP.PrintVector(d, "element disp:",
                m_FileOutput);
            m_MTBDP.PrintVector(ql, "element local load vec:",
                m_FileOutput);
            m_MTBDP.PrintVector(dfL, "Forces:",
                m_FileOutput);
        }

        // Find Max Compressive, Tensile Stress and Shear Stress
        float fCompStress, fTensStress, fShearStress;

        float ft1 = (dfL(1) / dArea) - (abs(dfL(3)) / dS);
        fCompStress = static_cast<float>((ft1 < 0.0f ? abs(ft1) : 0.0f));
        float ft2 = (dfL(1) / dArea) + (abs(dfL(3))/dS);
        fTensStress = static_cast<float>((ft2 > 0.0f ? abs(ft2) : 0.0f));
        fShearStress = static_cast<float>(dfL(2) / dSF);

        m_ElementResponseData(i).SetStresses(fCompStress, fTensStress, fShearStress);
    }
    
}

void CFrame::Residual()
// ---------------------------------------------------------------------------
// Function: Computes the element response
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    CVector<int> nE(DOFPE);             // the nodal degrees-of-freedom vector
    CMatrix<double> dT(DOFPE,DOFPE),
        dTT(DOFPE, DOFPE);           // T and T(t)
    CVector<double> dLF(2), dF(DOFPE);      // local f, global f

    int nSN, nEN, nMatGrp;
    CXSType* pXSGrp;
    float fArea, fIyy, fIzz, fSyy, fSzz, fSFyy, fSFzz;
    CVector<float> fVC1(NDIM), fVC2(NDIM);

    // initialize
    dT.Set(0.0);

    // loop thro' all elements
    for (int i = 1; i <= m_nElements; i++)
    {
        m_ElementData(i).GetENodes(nSN, nEN);
        m_NodalData(nSN).GetCoords(fVC1);
        m_NodalData(nEN).GetCoords(fVC2);
        pXSGrp = m_ElementData(i).GetEPropertyGroup();
        nMatGrp = m_ElementData(i).GetMatPropertyGroup();
        pXSGrp->GetProperties(fArea, fIyy, fIzz, fSyy, fSzz, fSFyy, fSFzz);
        float fYM = m_MaterialData(nMatGrp).GetYM();
        float fCTE = m_MaterialData(nMatGrp).GetCTE();
        float fLength;
        m_ElementData(i).GetLength(fLength);

        double dArea = static_cast<double>(fArea);
        double dLength = static_cast<double>(fLength);
        double dYM = static_cast<double>(fYM);
        double x1 = static_cast<double>(fVC1(1));
        double y1 = static_cast<double>(fVC1(2));
        double x2 = static_cast<double>(fVC2(1));
        double y2 = static_cast<double>(fVC2(2));
        double l = (x2 - x1) / fLength;
        double m = (y2 - y1) / fLength;

        // Construct T
            dT(1, 1) = l; dT(1, 2) = m;
            dT(2, 1) = -1.0*m; dT(2, 2) = l;
            dT(3, 3) = 1.0;
            dT(4, 4) = l; dT(4, 5) = m;
            dT(5, 4) = -1.0 * m; dT(5, 5) = l;
            dT(6, 6) = 1.0;

        // Tranpose T
        m_MTBDP.Transpose(dT, dTT);

        CVector<float>fVFSN(DOFPN); CVector<float>fVFEN(DOFPN);
        m_ElementResponseData(i).GetValues(fVFSN, fVFEN);
        CVector<double> dLF(DOFPE);          // local force vector
        dLF.Set(0.0);
        for (int j = 1; j <= 3; j++) {
            dLF(j) = fVFSN(j);
            dLF(DOFPN + j) = fVFEN(j);
        }
        

        // debug?
        if (m_nDebugLevel == 1)
        {
            m_MTBDP.PrintVector(dLF, "Local Force Vector",
                m_FileOutput);
        }

        // form d'=T*d
        m_MTBDP.MatMultVec(dTT, dLF, dF);

        if (m_nDebugLevel == 1)
        {
            m_MTBDP.PrintVector(dF, "Global Force Vector",
                m_FileOutput);
        }

        // update residual
            nE(1) = DOFPN * nSN - 2; nE(2) = DOFPN * nSN - 1; nE(3) = DOFPN * nSN;
            nE(4) = DOFPN * nEN - 2; nE(5) = DOFPN * nEN - 1; nE(6) = DOFPN * nEN;


        for (int j = 1; j <= DOFPE; j++)
        {
            m_dR(nE(j)) = m_dR(nE(j)) + dF(j);
        }

        if (m_nDebugLevel == 1)
        {
            m_MTBDP.PrintVector(m_dR, "Updated Residual",
                m_FileOutput);
        }
    }
    // Extract support reactions
    // loop thro' all nodes
    for (int i = 1; i <= m_nNodes; i++)
    {
        float fRX = 0.0f; float fRY = 0.0f; float fRZ = 0.0f;
        int nXFC = 0; int nYFC = 0; int nZFC = 0;
        CVector<float> fVC(DOFPN);      // Stores the specified displacements
        CVector<CNode::Fixity> VFC(DOFPN);
        m_NodalData(i).GetFixity(VFC, fVC);
        if (VFC(1) == CNode::Fixity::SPECIFIED) {
            int nGDOF = DOFPN * i - 2;
            fRX = m_dR(nGDOF);
            m_dR(nGDOF) = 0.0f;
            nXFC = 1;
        }
        if (VFC(2) == CNode::Fixity::SPECIFIED) {
            int nGDOF = DOFPN * i - 1;
            fRY = m_dR(nGDOF);
            m_dR(nGDOF) = 0.0f;
            nYFC = 1;
        }
        if (VFC(3) == CNode::Fixity::SPECIFIED) {
            int nGDOF = DOFPN * i;
            fRZ = m_dR(nGDOF);
            m_dR(nGDOF) = 0.0f;
            nZFC = 1;
        }
        if (nXFC == 1 || nYFC == 1 || nZFC == 1) {
            m_NodalResponseData(i).SetReactions(fRX, fRY, fRZ);
        }
        if (m_nDebugLevel == 1)
        {
            m_MTBDP.PrintVector(m_dR, "Updated Residual",
                m_FileOutput);
        }

    }
    // compute errors
    m_dAbsErr = m_MTBDP.TwoNorm(m_dR);
    m_dRelAbsErr = m_dAbsErr / m_MTBDP.TwoNorm(dF);
}

void CFrame::ErrorHandler (CLocalErrorHandler::ERRORCODE ErrorCode)
{
    throw ErrorCode;
}

void CFrame::ErrorHandler (CGlobalErrorHandler::ERRORCODE ErrorCode) const
{
    throw ErrorCode;
}

void CFrame::DisplayErrorMessage (CLocalErrorHandler::ERRORCODE err)
{
    m_LEH.ErrorHandler (err, m_nLineNumber);
}
