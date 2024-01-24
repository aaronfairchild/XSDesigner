/*********************************************
Planar Frame Analysis Program
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis
*********************************************/
#include <vector>
#include <iomanip>
#include <sstream>
#include "frame.h"
#include "getinteractiveEXH.h"
#include "parserEXH.h"
#include "fileioEXH.h"
#include "elementloads.h"
#include "circsolid.h"
#include "circhollow.h"
#include "printtableEXH.h"
#include "xstype.h"
#include "node.h"

void CFrame::Banner (std::ostream& OF)
// ---------------------------------------------------------------------------
// Function: Prints program banner
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    OF << '\n';
    OF << "\t\t--------------------------------------------" << '\n';
    OF << "\t\t     XS Designer -- Reinforced Concrete     " << '\n';
    OF << "\t\t          CEE 593: Applied Project          " << '\n';
    OF << "\t\t            By: Aaron Fairchild            " << '\n';
    OF << "\t\t--------------------------------------------" << '\n';
}

void CFrame::PrepareIO (int argc, char *argv[])
// ---------------------------------------------------------------------------
// Function: Obtains file names and opens input/output files
// Input:    command line arguments
// Output:   none
// ---------------------------------------------------------------------------
{
    if (argc == 1)
    {
        // open the input file
        m_FIO.OpenInputFileByName ("Complete input file name: ", m_FileInput,
                                   std::ios::in);

        // open the output file
        m_FIO.OpenOutputFileByName ("Complete output file name: ", m_FileOutput,
                                    std::ios::out);
    }
    else if (argc == 3) // XSDesigner input_file output_file
    {
        m_FileInput.open (argv[1], std::ios::in);
        if (!m_FileInput)
            ErrorHandler (CGlobalErrorHandler::ERRORCODE::CANNOTOPENIFILE);
        m_FileOutput.open (argv[2], std::ios::out);
        if (!m_FileOutput)
            ErrorHandler (CGlobalErrorHandler::ERRORCODE::CANNOTOPENOFILE);
        std::cout << "\n";
        std::cout << argv[1] << " opened as input file.\n";
        std::cout << argv[2] << " opened as output file.\n";
    }
	else
    {
        ErrorHandler (CLocalErrorHandler::ERRORCODE::INVALIDCOMMANDLINE);
    }

    // print banner
    Banner (m_FileOutput);
}

void CFrame::ReadProblemSize ()
// ---------------------------------------------------------------------------
// Function: reads the number of reinforcement groups (in and out of plane)
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    bool bEOF = false;
    try
    {
        // read the problem description
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        if (m_strVTokens[0] != "*HEADING")
            IOErrorHandler (ERRORCODE::INVALIDINPUT);

        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);

        // Unit Mode
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        if (m_strVTokens[0] != "*UNIT" && m_strVTokens[1] != "MODE")
            IOErrorHandler (ERRORCODE::INVALIDINPUT);
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        // Member Type
        for (;;)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_strVTokens[0] == "*nodal" && m_strVTokens[1] == "loads")
                break;
        }

        // Concrete Material Properties
        for (;;)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_strVTokens[0] == "*material" && m_strVTokens[1] == "data")
                break;
        }

        // Reinforcement Material Properties
        for (;;)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_strVTokens[0] == "*cross-sectional" && m_strVTokens[1] == "data")
                break;
            ++m_nMatGroups;
        }

        // Element Geometry
        for (;;)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_strVTokens[0] == "*element" && m_strVTokens[1] == "data")
                break;
            ++m_nEPGroups;
        }

        // Cross-Sectional Reinforcement Data
        for (;;)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_strVTokens[0] == "*element" && m_strVTokens[1] == "loads")
                break;
            ++m_nElements;
        }

        // Out of Plane Reinforcement Data
        for (;;)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_strVTokens[0] == "*end")
                break;
            ++m_nElementLoads;
        }

        // Forces and Moments
        for (;;)
        {
            m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
                m_nTokens, m_strDelimiters, m_strComment,
                bEOF);
            if (m_strVTokens[0] == "*end")
                break;
            ++m_nElementLoads;
        }
    }
    // trap all input file errors here
    catch (CLocalErrorHandler::ERRORCODE &err)
    {
        m_LEH.ErrorHandler (err);
    }

    catch (CGlobalErrorHandler::ERRORCODE &err)
    {
        ErrorHandler (CLocalErrorHandler::ERRORCODE::INVALIDINPUT);
    }

    catch (...)
    {
        std::cout << "Sorry, could not catch the error whatever it is.\n";
    }

    // check data for validity
    if (m_nNodes <= 1) 
        IOErrorHandler (ERRORCODE::NUMNODES);
    if (m_nElements <= 0) 
        IOErrorHandler (ERRORCODE::NUMELEMENTS);
    if (m_nMatGroups <= 0) 
        IOErrorHandler (ERRORCODE::NUMMATGROUPS); 
    if (m_nEPGroups <= 0) 
        IOErrorHandler (ERRORCODE::NUMEPROPGROUPS); 
    if (m_nDebugLevel < 0 || m_nDebugLevel > 1) 
        IOErrorHandler (ERRORCODE::DEBUGCODE);
}

void CFrame::ReadFrameModel ()
// ---------------------------------------------------------------------------
// Function: reads the rest of the frame data from input file
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    int nTag;                // stores tag
    CVector<float> fVC(3);   // stores float input
    bool bEOF = false;       // end of file flag

    try
    {
        // rewind the file to read the input file again
        m_FIO.Rewind (m_FileInput);
        m_nLineNumber = 0;

        // header line
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);

        // read nodal coordinates
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment, 
                           bEOF);
        for (int i = 1; i <= m_nNodes; i++)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_nTokens != 3)
                IOErrorHandler (ERRORCODE::INVALIDINPUT);
            if (!m_Parse.GetIntValue(m_strVTokens[0], nTag) ||
                !m_Parse.GetFloatValue(m_strVTokens[1], fVC(1)) ||
                !m_Parse.GetFloatValue(m_strVTokens[2], fVC(2)))
                IOErrorHandler (ERRORCODE::INVALIDINPUT);
            if (nTag <= 0 || nTag > m_nNodes)
                IOErrorHandler (ERRORCODE::NODENUMBER);
            m_NodalData(nTag).SetCoords (fVC);
        }

        // read nodal fixity conditions
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        CVector<CNode::Fixity> VFC(DOFPN);
        CVector<float> fVDisp(DOFPN);
        for (;;)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_strVTokens[0] == "*nodal" && m_strVTokens[1] == "loads")
                break;
            if (m_nTokens != 7)
                IOErrorHandler (ERRORCODE::INVALIDINPUT);
            if (!m_Parse.GetIntValue(m_strVTokens[0], nTag) ||
                !m_Parse.GetFloatValue(m_strVTokens[4], fVC(1)) ||
                !m_Parse.GetFloatValue(m_strVTokens[5], fVC(2)) ||
                !m_Parse.GetFloatValue(m_strVTokens[6], fVC(3)))
                IOErrorHandler (ERRORCODE::INVALIDINPUT);

            if (nTag <= 0 || nTag > m_nNodes)
                IOErrorHandler (ERRORCODE::NODENUMBER);
            if (m_strVTokens[1] != "free" && m_strVTokens[1] != "specified")
                IOErrorHandler (ERRORCODE::NODALFIXITY);
            if (m_strVTokens[2] != "free" && m_strVTokens[2] != "specified")
                IOErrorHandler (ERRORCODE::NODALFIXITY);
            if (m_strVTokens[3] != "free" && m_strVTokens[3] != "specified")
                IOErrorHandler (ERRORCODE::NODALFIXITY);

            VFC(1) = (m_strVTokens[1] == "specified" ? CNode::Fixity::SPECIFIED : CNode::Fixity::FREE);
            VFC(2) = (m_strVTokens[2] == "specified" ? CNode::Fixity::SPECIFIED : CNode::Fixity::FREE);
            VFC(3) = (m_strVTokens[3] == "specified" ? CNode::Fixity::SPECIFIED : CNode::Fixity::FREE);
            m_NodalData(nTag).SetFixity (VFC, fVC);
        }

        // read nodal loads
        for (;;)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_strVTokens[0] == "*material" && m_strVTokens[1] == "data")
                break;
            if (m_nTokens != 5)
                IOErrorHandler (ERRORCODE::INVALIDINPUT);
            m_Parse.GetIntValue(m_strVTokens[0], nTag);
            m_Parse.GetFloatValue(m_strVTokens[1], fVC(1));
            m_Parse.GetFloatValue(m_strVTokens[2], fVC(2));
            m_Parse.GetFloatValue(m_strVTokens[3], fVC(3));
            if (nTag <= 0 || nTag > m_nNodes)
                IOErrorHandler (ERRORCODE::NODENUMBER);
            int nSDOF = (nTag - 1)*DOFPN;
            m_NodalLoadData(nTag).SetValues(fVC);
        }

        // read material data
        for (int i = 1; i <= m_nMatGroups; i++)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_nTokens != 4)
                IOErrorHandler (ERRORCODE::INVALIDINPUT);
            m_Parse.GetIntValue(m_strVTokens[0], nTag);
            m_Parse.GetFloatValue(m_strVTokens[1], fVC(1));
            m_Parse.GetFloatValue(m_strVTokens[2], fVC(2));
            m_Parse.GetFloatValue(m_strVTokens[3], fVC(3));
            if (fVC(1) <= 0.0f)
                IOErrorHandler (ERRORCODE::MATPROPERTY);
            if (nTag <= 0 || nTag > m_nMatGroups)
                IOErrorHandler (ERRORCODE::MATGROUPNUMBER);
            m_MaterialData(nTag).SetYM (fVC(1));
        }

        // read cross-sectional data
        std::string strTag;
        CVector<float> fVXSDims(MAXEPDIM);
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        for (int i = 1; i <= m_nEPGroups; i++)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            m_Parse.GetIntValue(m_strVTokens[0], nTag);
            if (nTag < 0 || nTag > m_nEPGroups)
                IOErrorHandler (ERRORCODE::INVALIDINPUT);
            if (m_strVTokens[1] == "rects")
            {
                if (m_nTokens != 4)
                    IOErrorHandler (ERRORCODE::INVALIDINPUT);
                m_Parse.GetFloatValue(m_strVTokens[2], fVXSDims(1));
                m_Parse.GetFloatValue(m_strVTokens[3], fVXSDims(2));
                if (fVXSDims(1) <= 0.0f || fVXSDims(2) <= 0.0f)
                    IOErrorHandler (ERRORCODE::XSDIMENSION);
                m_EPData(nTag) = new CRectSolid (fVXSDims);
            }
            else if (m_strVTokens[1] == "isection")
            {
                if (m_nTokens != 6)
                    IOErrorHandler (ERRORCODE::INVALIDINPUT);
                m_Parse.GetFloatValue(m_strVTokens[2], fVXSDims(1));
                m_Parse.GetFloatValue(m_strVTokens[3], fVXSDims(2));
                m_Parse.GetFloatValue(m_strVTokens[4], fVXSDims(3));
                m_Parse.GetFloatValue(m_strVTokens[5], fVXSDims(4));
                if (fVXSDims(1) <= 0.0f || fVXSDims(2) <= 0.0f ||
                    fVXSDims(3) <= 0.0f || fVXSDims(4) <= 0.0f)
                    IOErrorHandler (ERRORCODE::XSDIMENSION);
                m_EPData(nTag) = new CISection (fVXSDims);
            }
            else if (m_strVTokens[1] == "circs")
            {
                if (m_nTokens != 3)
                    IOErrorHandler(ERRORCODE::INVALIDINPUT);
                m_Parse.GetFloatValue(m_strVTokens[2], fVXSDims(1));
                if (fVXSDims(1) <= 0.0f)
                    IOErrorHandler(ERRORCODE::XSDIMENSION);
                m_EPData(nTag) = new CCircSolid (fVXSDims);
            }
            else if (m_strVTokens[1] == "tube")
            {
                if (m_nTokens != 4)
                    IOErrorHandler(ERRORCODE::INVALIDINPUT);
                m_Parse.GetFloatValue(m_strVTokens[2], fVXSDims(1));
                m_Parse.GetFloatValue(m_strVTokens[3], fVXSDims(2));
                if (fVXSDims(1) <= 0.0f || fVXSDims(2) <= 0.0f)
                    IOErrorHandler(ERRORCODE::XSDIMENSION);
                m_EPData(nTag) = new CCircHollow (fVXSDims);
            }
            else
                IOErrorHandler (ERRORCODE::XSTYPE);
        }

        // read element data
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        int nSN, nEN, nMatGrp, nEPGrp;
        for (int i = 1; i <= m_nElements; i++)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_nTokens != 5)
                IOErrorHandler (ERRORCODE::INVALIDINPUT);
            m_Parse.GetIntValue(m_strVTokens[0], nTag);
            if (nTag <= 0 || nTag > m_nElements)
                IOErrorHandler (ERRORCODE::ELEMENTNUMBER);
            m_Parse.GetIntValue(m_strVTokens[1], nSN);
            m_Parse.GetIntValue(m_strVTokens[2], nEN);
            m_Parse.GetIntValue(m_strVTokens[3], nMatGrp);
            m_Parse.GetIntValue(m_strVTokens[4], nEPGrp);
            if (nMatGrp <= 0 || nMatGrp > m_nMatGroups)
                IOErrorHandler (ERRORCODE::MATGROUPNUMBER);
            if (nEPGrp <= 0 || nEPGrp > m_nEPGroups)
                IOErrorHandler (ERRORCODE::EPROPNUMBER);
            if (nSN <= 0 || nSN > m_nNodes)
                IOErrorHandler (ERRORCODE::NODENUMBER);
            if (nEN <= 0 || nEN > m_nNodes)
                IOErrorHandler (ERRORCODE::NODENUMBER);
            m_ElementData(nTag).SetMatPropertyGroup (nMatGrp);
            m_ElementData(nTag).SetEPropertyGroup (m_EPData(nEPGrp));
            m_ElementData(nTag).SetENodes (nSN, nEN);

            // calc and store element length
            CVector<float> fVC1(NDIM), fVC2(NDIM);
            m_NodalData(nSN).GetCoords(fVC1);
            m_NodalData(nEN).GetCoords(fVC2);
            float x1 = fVC1(1); float y1 = fVC1(2);
            float x2 = fVC2(1); float y2 = fVC2(2);
            float fLength = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
            m_ElementData(nTag).SetLength(fLength);
        }

        // read element loads
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        std::string strEType;
        CVector<float> fVELoads(2);
        int counter = 1;
        for (;;)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_strVTokens[0] == "*end")
                break;
            if (m_nTokens != 4)
                IOErrorHandler (ERRORCODE::INVALIDINPUT);
            m_Parse.GetIntValue(m_strVTokens[0], nTag);
            if (nTag <= 0 || nTag > m_nElements)
                IOErrorHandler (ERRORCODE::ELEMENTNUMBER);
            strEType = m_strVTokens[1];
            m_Parse.GetFloatValue(m_strVTokens[2], fVELoads(1));
            m_Parse.GetFloatValue(m_strVTokens[3], fVELoads(2));
            if (strEType != "dly'" && strEType != "ploadx'" &&
                strEType != "ploady'" && strEType != "cmoment")
                IOErrorHandler (ERRORCODE::ELOADTYPE);

            if (strEType == "dly'") {
                m_ElementLoadData(counter).SetValues(nTag, CElementLoads::ELType::DISTRIBUTEDY,
                    fVELoads(1), fVELoads(2));
                counter += 1;
            }
            else if (strEType == "ploadx'") {
                m_ElementLoadData(counter).SetValues(nTag, CElementLoads::ELType::CONCENTRATEDX,
                    fVELoads(1), fVELoads(2));
                counter += 1;
            }
            else if (strEType == "ploady'") {
                m_ElementLoadData(counter).SetValues(nTag, CElementLoads::ELType::CONCENTRATEDY,
                    fVELoads(1), fVELoads(2));
                counter += 1;
            }
            else if (strEType == "cmoment") {
                m_ElementLoadData(counter).SetValues(nTag, CElementLoads::ELType::CONCENTRATEDM,
                    fVELoads(1), fVELoads(2));
                counter += 1;
            }
            

            // Construct equivalent nodal loads and update the matrices
            float fLength;
            CVector<float> fNL1(DOFPN), fNL2(DOFPN);    // global Equiv. nodal load vector on left and right
            CVector<float> fEL1(DOFPN), fEL2(DOFPN);    // equiv nodal load from element
            m_ElementData(nTag).GetLength(fLength);
            m_ElementData(nTag).GetENodes(nSN, nEN);
            /*
            m_NodalLoadData(nSN).GetValues(fNL1);
            m_NodalLoadData(nEN).GetValues(fNL2);
            */

            m_ElementLoadData(counter - 1).GetENF(fEL1, fEL2, fLength);

            CVector<float> fVC1(NDIM), fVC2(NDIM);
            m_NodalData(nSN).GetCoords(fVC1);
            m_NodalData(nEN).GetCoords(fVC2);
            double dLength = static_cast<double>(fLength);
            double x1 = static_cast<double>(fVC1(1));
            double y1 = static_cast<double>(fVC1(2));
            double x2 = static_cast<double>(fVC2(1));
            double y2 = static_cast<double>(fVC2(2));
            double l = (x2 - x1) / fLength;
            double m = (y2 - y1) / fLength;

            CVector<double> dQL(6);          // local load vector
            CVector<double> dQ(6);           // global load vector


            dQL(1) = static_cast<double>(fEL1(1));
            dQL(2) = static_cast<double>(fEL1(2));
            dQL(3) = static_cast<double>(fEL1(3));
            dQL(4) = static_cast<double>(fEL2(1));
            dQL(5) = static_cast<double>(fEL2(2));
            dQL(6) = static_cast<double>(fEL2(3));

                m_ELL(1, nTag) += static_cast<double>(fEL1(1));
                m_ELL(2, nTag) += static_cast<double>(fEL1(2));
                m_ELL(3, nTag) += static_cast<double>(fEL1(3));
                m_ELL(4, nTag) += static_cast<double>(fEL2(1));
                m_ELL(5, nTag) += static_cast<double>(fEL2(2));
                m_ELL(6, nTag) += static_cast<double>(fEL2(3));


            dQ(1) = l * dQL(1) - m * dQL(2);
            dQ(2) = m * dQL(1) + l * dQL(2);
            dQ(3) = dQL(3);
            dQ(4) = l * dQL(4) - m * dQL(5);
            dQ(5) = m * dQL(4) + l * dQL(5);
            dQ(6) = dQL(6);

            if (m_nDebugLevel == 1)
            {
                m_MTBDP.PrintVector(dQL, "QL:",
                    m_FileOutput);
                m_MTBDP.PrintVector(dQ, "Q:",
                    m_FileOutput);
            }
            m_ELG(1, nTag) += static_cast<double>(dQ(1));
            m_ELG(2, nTag) += static_cast<double>(dQ(2));
            m_ELG(3, nTag) += static_cast<double>(dQ(3));
            m_ELG(4, nTag) += static_cast<double>(dQ(4));
            m_ELG(5, nTag) += static_cast<double>(dQ(5));
            m_ELG(6, nTag) += static_cast<double>(dQ(6));
        }
    }
    // trap all input file errors here
    catch (CLocalErrorHandler::ERRORCODE &err)
    {
        m_LEH.ErrorHandler (err);
    }

    catch (CGlobalErrorHandler::ERRORCODE &err)
    {
        ErrorHandler (CLocalErrorHandler::ERRORCODE::INVALIDINPUT);
    }

    catch (...)
    {
        std::cout << "Sorry, could not catch the error whatever it is.\n";
    }
}

void CFrame::CreateOutput ()
// ---------------------------------------------------------------------------
// Function: creates the output file
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // print analysis date and time
    m_FileOutput << '\n';
    m_FileOutput << "Starting out at : " << m_strDateTime << '\n';

    // problem description
    m_FileOutput << "------------" << '\n';
    m_FileOutput << "PROBLEM SIZE" << '\n';
    m_FileOutput << "------------" << '\n';
    m_FileOutput << "                  Number of nodes : " << m_nNodes << '\n';
    m_FileOutput << "               Number of elements : " << m_nElements << '\n';
    m_FileOutput << "        Number of material groups : " << m_nMatGroups << '\n';
    m_FileOutput << "Number of element property groups : " << m_nEPGroups << '\n';
    m_FileOutput << "                    Number of DOF : " << m_nDOF << "\n\n";
    m_FileOutput << "-------------------" << '\n';
    m_FileOutput << "MATERIAL PROPERTIES" << '\n';
    m_FileOutput << "-------------------" << '\n';
    m_FileOutput << "Group    Young's Modulus                CTE" << '\n';
    m_FileOutput << "-----    ---------------    ---------------" << '\n';
    for (int nGroup = 1; nGroup <= m_nMatGroups; nGroup++)
    {
        float fYM = m_MaterialData(nGroup).GetYM();
        float fCTE = m_MaterialData(nGroup).GetCTE();
        m_FileOutput << std::setw(5) << nGroup << "    ";
        m_FileOutput << std::setw(15) << fYM << "    ";
        m_FileOutput << std::setw(15) << fCTE << '\n';
    }
    m_FileOutput << '\n';
    m_FileOutput << "--------------------------" << '\n';
    m_FileOutput << "CROSS-SECTIONAL DIMENSIONS" << '\n';
    m_FileOutput << "--------------------------" << '\n';
    m_FileOutput << "Group               Type         Dimensions" << '\n';
    m_FileOutput << "-----    ---------------    ---------------" << '\n';
    CVector<float> fDims(MAXEPDIM);

    CXSType* pXSGrp;
    CXSType::EPType myType;
    CVector<float> fV(MAXEPDIM);
    for (int nGroup = 1; nGroup <= m_nEPGroups; nGroup++)
    {
        pXSGrp = m_EPData(nGroup);
        pXSGrp->GetType(myType);
        pXSGrp->GetDimensions(fV);
        if (myType == CXSType::EPType::CIRCH) {
            m_FileOutput << std::setw(5) << nGroup << "       ";
            m_FileOutput << std::setw(15) << "Circ. Hollow" << " ";
            m_FileOutput << std::setw(15) << "Inner Radius = " << fV(1) << '\n';
            m_FileOutput << std::setw(42) << "                         Wall Thickness = " << fV(2) << '\n';
        }
        if (myType == CXSType::EPType::CIRCS) {
            m_FileOutput << std::setw(5) << nGroup << "        ";
            m_FileOutput << std::setw(15) << "Circ. Solid" << "    ";
            m_FileOutput << std::setw(15) << "Radius = " << fV(1) << '\n';
        }
        if (myType == CXSType::EPType::ISECTION) {
            m_FileOutput << std::setw(5) << nGroup << "    ";
            m_FileOutput << std::setw(15) << "I-Section" << "   ";
            m_FileOutput << std::setw(15) << "Web Height = " << fV(1) << '\n';
            m_FileOutput << std::setw(42) << "Web Thickness = " << fV(2) << '\n';
            m_FileOutput << std::setw(42) << "Flange Width = " << fV(3) << '\n';
            m_FileOutput << std::setw(42) << "Flange Thickness = " << fV(4) << '\n';
        }
        if (myType == CXSType::EPType::RECTS) {
            m_FileOutput << std::setw(5) << nGroup << "    ";
            m_FileOutput << std::setw(15) << "Rect. Solid" << " ";
            m_FileOutput << std::setw(15) << "Height = " << fV(1) << '\n';
            m_FileOutput << std::setw(39) << "Width = " << fV(2) << '\n';
        }
    }

    float fArea, fIyy, fIzz, fSyy, fSzz, fSFyy, fSFzz;
    m_FileOutput << '\n';
    m_FileOutput << "--------------------------" << '\n';
    m_FileOutput << "CROSS-SECTIONAL PROPERTIES" << '\n';
    m_FileOutput << "--------------------------" << '\n';
    m_FileOutput << "Group                Area     Moment of Inertia         Shear Factor      Section Modulus" << '\n';
    m_FileOutput << "-----    -----------------    -----------------    -----------------    -----------------" << '\n';
    for (int i = 1; i <= m_nEPGroups; i++) {
        m_EPData(i)->GetProperties(fArea, fIyy, fIzz, fSyy, fSzz, fSFyy, fSFzz);
        float fI = (fIyy > fIzz ? fIyy : fIzz);
        float fS = (fIyy > fIzz ? fSyy : fSzz);
        float fSF = (fIyy > fIzz ? fSFzz : fSFyy);
        m_FileOutput << std::setw(5) << i << "    ";
        m_FileOutput << std::setw(17) << fArea << "    ";
        m_FileOutput << std::setw(17) << fI << "    ";
        m_FileOutput << std::setw(17) << fSF << "    ";
        m_FileOutput << std::setw(17) << fS << "    \n";
    }

    m_FileOutput << '\n';
    m_FileOutput << "-----------------" << '\n';
    m_FileOutput << "NODAL COORDINATES" << '\n';
    m_FileOutput << "-----------------" << '\n';
    m_FileOutput << "Node    X-Coordinate    Y-Coordinate" << '\n';
    m_FileOutput << "----    ------------    ------------" << '\n';
    CVector<float> fVC(2);
    for (int i = 1; i <= m_nNodes; i++) {
        m_NodalData(i).GetCoords(fVC);
        m_FileOutput << std::setw(4) << i << "    ";
        m_FileOutput << std::setw(12) << fVC(1) << "    ";
        m_FileOutput << std::setw(12) << fVC(2) << "\n";
    }

    m_FileOutput << '\n';
    m_FileOutput << "---------------" << '\n';
    m_FileOutput << "NODAL FIXITIESS (default is FREE)" << '\n';
    m_FileOutput << "------------ --" << '\n';
    m_FileOutput << "Node    X-Fixity       X-Disp     Y-Fixity       Y-Disp     Z-Fixity       Z-Disp" << '\n';
    m_FileOutput << "---    ---------    ---------    ---------    ---------    ---------    ---------" << '\n';
    CVector<CNode::Fixity> VFC1(DOFPN);
    CVector<float> fVC2(DOFPN);      // Stores the specified displacements
    for (int i = 1; i <= m_nNodes; i++) {
        m_NodalData(i).GetFixity(VFC1, fVC2);
        m_FileOutput << std::setw(4) << i << "    ";
        if (VFC1(1) == CNode::Fixity::SPECIFIED) {
            m_FileOutput << std::setw(9) << "Specified" << "    ";
            m_FileOutput << std::setw(9) << fVC2(1) << "    ";
        }
        else {
            m_FileOutput << std::setw(9) << "Free" << "    ";
            m_FileOutput << "             ";
        }
        if (VFC1(2) == CNode::Fixity::SPECIFIED) {
            m_FileOutput << std::setw(9) << "Specified" << "    ";
            m_FileOutput << std::setw(9) << fVC2(2) << "    ";
        }
        else {
            m_FileOutput << std::setw(9) << "Free" << "    ";
            m_FileOutput << "             ";
        }
        if (VFC1(3) == CNode::Fixity::SPECIFIED) {
            m_FileOutput << std::setw(9) << "Specified" << "    ";
            m_FileOutput << std::setw(9) << fVC2(3) << "    \n";
        }
        else {
            m_FileOutput << std::setw(9) << "Free" << "    ";
            m_FileOutput << "             \n";
        }
    }

    m_FileOutput << '\n';
    m_FileOutput << "-----------" << '\n';
    m_FileOutput << "NODAL FORCES (default is no force)" << '\n';
    m_FileOutput << "-----------" << '\n';
    m_FileOutput << "Node    X-Force    Y-Force    Z-Force    Delta-T" << '\n';
    m_FileOutput << "----    -------    -------    -------    -------" << '\n';
    CVector<float> fNL(DOFPN);
    for (int i = 1; i <= m_nNodes; i++) {
        m_NodalLoadData(i).GetValues(fNL);
        if (fNL(1) == 0.0f && fNL(2) == 0.0f && fNL(3) == 0.0f) {
            continue;
        }
        m_FileOutput << std::setw(4) << i << "    ";
        m_FileOutput << std::setw(7) << fNL(1) << "    ";
        m_FileOutput << std::setw(7) << fNL(2) << "    ";
        m_FileOutput << std::setw(7) << fNL(3) << "    \n";
    }
    
    m_FileOutput << '\n';
    m_FileOutput << "------------" << '\n';
    m_FileOutput << "ELEMENT DATA" << '\n';
    m_FileOutput << "------------" << '\n';
    m_FileOutput << "Element     Start Node       End Node     Prop Group      Mat Group" << '\n';
    m_FileOutput << "-------    -----------    -----------    -----------    -----------" << '\n';
    int nSN, nEN, nMatGrp, nPropGroup;
    for (int i = 1; i <= m_nElements; i++){
        m_ElementData(i).GetENodes(nSN, nEN);
        nMatGrp = m_ElementData(i).GetMatPropertyGroup();
        m_FileOutput << std::setw(7) << i << "    ";
        m_FileOutput << std::setw(11) << nSN << "    ";
        m_FileOutput << std::setw(11) << nEN << "    ";
        m_FileOutput  << "               ";
        m_FileOutput << std::setw(11) << nMatGrp << "    \n";
    }

    m_FileOutput << '\n';
    m_FileOutput << "------------------------------" << '\n';
    m_FileOutput << "ELEMENT CONCENTRATED LOAD DATA" << '\n';
    m_FileOutput << "------------------------------" << '\n';
    m_FileOutput << "Element                    Type    Dist from Start Node          Load Intensity" << '\n';
    m_FileOutput << "-------    --------------------    --------------------    --------------------" << '\n';
    float f1; float f2; CElementLoads::ELType Type;
    int nTag;
    for (int i = 1; i <= m_nElementLoads; i++) {
        m_ElementLoadData(i).GetValues(nTag, Type, f1, f2);
        if (Type == CElementLoads::ELType::CONCENTRATEDM) {
            m_FileOutput << std::setw(7) << nTag << "    ";
            m_FileOutput << std::setw(20) << "Conc. Moment Load" << "    ";
            m_FileOutput << std::setw(20) << f1 << "    ";
            m_FileOutput << std::setw(20) << f2 << "    \n";
        }
        if (Type == CElementLoads::ELType::CONCENTRATEDX) {
            m_FileOutput << std::setw(7) << nTag << "    ";
            m_FileOutput << std::setw(20) << "Conc. X Load" << "    ";
            m_FileOutput << std::setw(20) << f1 << "    ";
            m_FileOutput << std::setw(20) << f2 << "    \n";
        }
        if (Type == CElementLoads::ELType::CONCENTRATEDY) {
            m_FileOutput << std::setw(7) << nTag << "    ";
            m_FileOutput << std::setw(20) << "Conc. Y Load" << "    ";
            m_FileOutput << std::setw(20) << f1 << "    ";
            m_FileOutput << std::setw(20) << f2 << "    \n";
        }
    }

    m_FileOutput << '\n';
    m_FileOutput << "-----------------------------" << '\n';
    m_FileOutput << "ELEMENT DISTRIBUTED LOAD DATA" << '\n';
    m_FileOutput << "-----------------------------" << '\n';
    m_FileOutput << "Element                       Type    Intensity at Start Node             Load Intensity" << '\n';
    m_FileOutput << "-------    -----------------------    -----------------------    -----------------------" << '\n';
    for (int i = 1; i <= m_nElementLoads; i++) {
        m_ElementLoadData(i).GetValues(nTag, Type, f1, f2);
        if (Type == CElementLoads::ELType::DISTRIBUTEDY) {
            m_FileOutput << std::setw(7) << nTag << "    ";
            m_FileOutput << std::setw(23) << "Local y dist. load" << "    ";
            m_FileOutput << std::setw(23) << f1 << "    ";
            m_FileOutput << std::setw(23) << f2 << "    \n";
        }
    }

    m_FileOutput << '\n';
    m_FileOutput << "==============================================================================" << '\n';
    m_FileOutput << "== FEA RESULTS ===============================================================" << '\n';
    m_FileOutput << "==============================================================================" << '\n';
    m_FileOutput << '\n';
    m_FileOutput << "-------------------" << '\n';
    m_FileOutput << "NODAL DISPLACEMENTS" << '\n';
    m_FileOutput << "-------------------" << '\n';
    m_FileOutput << "Node    X-Displacement    Y-Displacement        Z-Rotation" << '\n';
    m_FileOutput << "----    --------------    --------------    --------------" << '\n';
    CVector<float> dDisps(DOFPN);
    for (int i = 1; i <= m_nNodes; i++) {
        m_NodalResponseData(i).GetValues(dDisps);
        if (dDisps(1) != 0.0 || dDisps(2) != 0.0 || dDisps(3) != 0.0) {
            m_FileOutput << std::setw(4) << i << "    ";
            m_FileOutput << std::setw(14) << dDisps(1) << "    ";
            m_FileOutput << std::setw(14) << dDisps(2) << "    ";
            m_FileOutput << std::setw(14) << dDisps(3) << "    \n";
        }
    }

    m_FileOutput << '\n';
    m_FileOutput << "--------------------" << '\n';
    m_FileOutput << "ELEMENT NODAL FORCES" << '\n';
    m_FileOutput << "--------------------" << '\n';
    m_FileOutput << "Element            Axial            Shear           Moment" << '\n';
    m_FileOutput << "-------    -------------    -------------    -------------" << '\n';
    CVector<float>fVFSN(DOFPN); CVector<float>fVFEN(DOFPN);
    for (int i = 1; i <= m_nElements; i++) {
        m_ElementData(i).GetENodes(nSN, nEN);
        m_ElementResponseData(i).GetValues(fVFSN, fVFEN);
        m_FileOutput << std::setw(7) << i << "    ";
        m_FileOutput << std::setw(13) << fVFSN(1) << "    ";
        m_FileOutput << std::setw(13) << fVFSN(2) << "    ";
        m_FileOutput << std::setw(13) << fVFSN(3) << "    \n";
        m_FileOutput << "           ";
        m_FileOutput << std::setw(13) << fVFEN(1) << "    ";
        m_FileOutput << std::setw(13) << fVFEN(2) << "    ";
        m_FileOutput << std::setw(13) << fVFEN(3) << "    \n";
    }

    m_FileOutput << '\n';
    m_FileOutput << "----------------------------" << '\n';
    m_FileOutput << "MAX. ELEMENT INTERNAL FORCES" << '\n';
    m_FileOutput << "----------------------------" << '\n';
    m_FileOutput << "Element            Axial            Shear           Moment" << '\n';
    m_FileOutput << "-------    -------------    -------------    -------------" << '\n';
    CVector<float>fVFMax(DOFPN);
    for (int i = 1; i <= m_nElements; i++) {
        m_ElementData(i).GetENodes(nSN, nEN);
        m_ElementResponseData(i).GetValues(fVFSN, fVFEN);
        fVFMax(1) = (abs(fVFSN(1)) > abs(fVFEN(1)) ? fVFSN(1) : fVFEN(1));
        fVFMax(2) = (abs(fVFSN(2)) > abs(fVFEN(2)) ? fVFSN(2) : fVFEN(2));
        fVFMax(3) = (abs(fVFSN(3)) > abs(fVFEN(3)) ? fVFSN(3) : fVFEN(3));
        m_FileOutput << std::setw(7) << i << "    ";
        m_FileOutput << std::setw(13) << fVFMax(1) << "    ";
        m_FileOutput << std::setw(13) << fVFMax(2) << "    ";
        m_FileOutput << std::setw(13) << fVFMax(3) << "    \n";
    }

    m_FileOutput << '\n';
    m_FileOutput << "--------------------" << '\n';
    m_FileOutput << "ELEMENT MAX STRESSES" << '\n';
    m_FileOutput << "--------------------" << '\n';
    m_FileOutput << "Element      Compressive          Tensile            Shear" << '\n';
    m_FileOutput << "-------    -------------    -------------    -------------" << '\n';
    float fCompStress, fTensStress, fShearStress;
    for (int i = 1; i <= m_nElements; i++) {
        m_ElementResponseData(i).GetStresses(fCompStress, fTensStress, fShearStress);
        m_FileOutput << std::setw(7) << i << "    ";
        m_FileOutput << std::setw(13) << fCompStress << "    ";
        m_FileOutput << std::setw(13) << fTensStress << "    ";
        m_FileOutput << std::setw(13) << fShearStress << "    \n";
    }

    m_FileOutput << '\n';
    m_FileOutput << "---------------" << '\n';
    m_FileOutput << "NODAL REACTIONS" << '\n';
    m_FileOutput << "---------------" << '\n';
    m_FileOutput << "Node       X-Reaction       Y-Reaction       Z-Reaction" << '\n';
    m_FileOutput << "----    -------------    -------------    -------------" << '\n';
    float fRX, fRY, fRZ;
    for (int i = 1; i <= m_nNodes; i++) {
        m_NodalResponseData(i).GetReactions(fRX, fRY, fRZ);
        
        if (fRX == 0.0f && fRY == 0.0f && fRZ == 0.0f) {
            continue;
        }
        m_FileOutput << std::setw(4) << i << "    ";
        if (abs(fRX) > TOL) {
            m_FileOutput << std::setw(13) << fRX << "    ";
        }
        else {
            m_FileOutput << "                 ";
        }
        if (abs(fRY) > TOL) {
            m_FileOutput << std::setw(13) << fRY << "    ";
        }
        else {
            m_FileOutput << "                 ";
        }
        if (abs(fRZ) > TOL) {
            m_FileOutput << std::setw(13) << fRZ << "    \n";
        }
        else {
            m_FileOutput << "                 \n";
        }
    }
    // Print error stats
    m_FileOutput << '\n';
    m_FileOutput << "Absolute Error Norm : " << m_dAbsErr << '\n';
    m_FileOutput << "Relative Error Norm : " << m_dRelAbsErr << '\n';

    // Print elapsed clock time
    m_FileOutput << '\n';
    m_FileOutput << "      Ending at : " << m_strEndDateTime;
    m_FileOutput << "      Elapsed clock time : " << Timer.DiffTime() << " s\n";
}

void CFrame::IOErrorHandler (ERRORCODE ECode) const
// ---------------------------------------------------------------------------
// Function: displays error messages related to input data
// Input:    error code
// Output:   none
// ---------------------------------------------------------------------------
{
    std::cerr << '\n';

    if (ECode == ERRORCODE::NUMNODES) // invalid number of nodes
        std::cerr << "Number of nodes must be >= 2.";
    else if (ECode == ERRORCODE::NUMELEMENTS) // invalid number of elements
        std::cerr << "Number of elements must be >= 1.";
    else if (ECode == ERRORCODE::DEBUGCODE) // invalid debug level
        std::cerr << "Debug level must be 0 or 1.";
    else if (ECode == ERRORCODE::NODENUMBER) // invalid node number
        std::cerr << "Invalid node number.";
    else if (ECode == ERRORCODE::ELEMENTNUMBER) // invalid element number
        std::cerr << "Invalid element number.";
    else if (ECode == ERRORCODE::XSAREA) // invalid x/s area
        std::cerr << "Area must be positive.";
    else if (ECode == ERRORCODE::YOUNGSMODULUS) // invalid E
        std::cerr << "Modulus of elasticity must be positive.";
    else if (ECode == ERRORCODE::INVALIDINPUT) // invalid input
        std::cerr << "Invalid input.";
    else if (ECode == ERRORCODE::INVALIDLASTLINE) // invalid input
        std::cerr << "Input file needs *end as last line.";
    else if (ECode == ERRORCODE::NODALFIXITY) // invalid fixity code
        std::cerr << "Nodal fixity code must be 'free' or 'specified'.";
    else if (ECode == ERRORCODE::NUMMATGROUPS) // invalid number of material groups
        std::cerr << "Number of material groups must be >= 1.";
    else if (ECode == ERRORCODE::NUMEPROPGROUPS) // invalid number of property groups
        std::cerr << "Number of element property groups must be >= 1.";
    else if (ECode == ERRORCODE::EPROPNUMBER) // invalid element property group
        std::cerr << "Invalid element property group number.";
    else if (ECode == ERRORCODE::XSDIMENSION) // invalid x/s dimension
        std::cerr << "Invalid cross-section dimension.";
    else if (ECode == ERRORCODE::MATGROUPNUMBER) // invalid material group
        std::cerr << "Invalid material property group number.";
    else if (ECode == ERRORCODE::MATPROPERTY) // invalid material property
        std::cerr << "Invalid material property.";
    else if (ECode == ERRORCODE::ELOADTYPE) // invalid element load type
        std::cerr << "Invalid element load type.";
    else if (ECode == ERRORCODE::XSTYPE) // invalid x/s type
        std::cerr << "Invalid cross-section type.";
    else
        std::cerr << "Unknown error ...?";

    std::cerr << '\n' << "Error in input file line : " << m_nLineNumber;
    std::cerr << std::endl;

    exit (1);
}

void CFrame::TerminateProgram ()
// ---------------------------------------------------------------------------
// Function: terminates the program steps by closing the input/output files
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    // close the input and output files
    m_FileInput.close ();
    m_FileOutput.close ();

    std::cout << "\nExecution completed successfully." << std::endl;
}
