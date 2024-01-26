/**********************************************
Concrete and Steel Cross-Section Design Program

Created by: Aaron Fairchild
Based on the work of: S.B. Rajan
Date: Spring 2024

TODO:

**********************************************/

#include <vector>
#include <iomanip>
#include <sstream>
#include "element.h"
#include "reinforcement.h"
#include "getinteractiveEXH.h"
#include "parserEXH.h"
#include "fileioEXH.h"
#include "elementloads.h"
#include "circsolid.h"
#include "tsection.h"
#include "printtableEXH.h"
#include "xstype.h"

void CElement::Banner (std::ostream& OF)
// ---------------------------------------------------------------------------
// Function: Prints program banner
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    OF << '\n';
    OF << "\t\t--------------------------------------------" << '\n';
    OF << "\t\t   2D XS Designer -- Reinforced Concrete    " << '\n';
    OF << "\t\t          CEE 593: Applied Project          " << '\n';
    OF << "\t\t            By: Aaron Fairchild             " << '\n';
    OF << "\t\t--------------------------------------------" << '\n';
}

void CElement::PrepareIO (int argc, char *argv[])
// ---------------------------------------------------------------------------
// Function: Obtains file names and opens input/output files
// Input:    Command line arguments
// Output:   None
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

void CElement::ReadProblemSize ()
// ---------------------------------------------------------------------------
// Function: Reads the number of reinforcement groups (in and out of plane)
// Input:    None
// Output:   None
// TODO: Point to where the invalid input is, and what it is.
// ---------------------------------------------------------------------------
{
    bool bEOF = false;
    try
    {
        // Read the Problem Description
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        std::cout << m_strVTokens[0] << '\n';
        if (m_strVTokens[0] != "*heading")
            IOErrorHandler (ERRORCODE::INVALIDINPUT);

        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);

        // Unit Mode
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        if (m_strVTokens[0] != "*unit" && m_strVTokens[1] != "mode")
            IOErrorHandler (ERRORCODE::INVALIDINPUT);
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        // Member Type
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        if (m_strVTokens[0] != "*column" && m_strVTokens[1] != "or" && m_strVTokens[2] != "beam")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);
        
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);


        // Concrete Material Properties
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "*concrete" && m_strVTokens[1] != "material" && m_strVTokens[2] != "properties")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        // Reinforcement Material Properties
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "*reinforcement" && m_strVTokens[1] != "material" && m_strVTokens[2] != "properties")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        // Element Geometry
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "*geometry")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        // Cross-Sectional Reinforcement Data
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "*xs" && m_strVTokens[1] != "reinforcements")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

        for (;;)
        {
            m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                               m_nTokens, m_strDelimiters, m_strComment,
                               bEOF);
            if (m_strVTokens[0] == "*transverse" && m_strVTokens[1] == "reinforcements")
                break;
            ++m_nXSR;
        }

        // Out of Plane Reinforcement Data
        for (;;)
        {
            m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
                m_nTokens, m_strDelimiters, m_strComment,
                bEOF);
            if (m_strVTokens[0] == "*nodal" && m_strVTokens[1] == "forces")
                break;
            ++m_nTR;
        }

        // Nodal Forces and Moments
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "start")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "end")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

       // Element Loads
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] != "*element" && m_strVTokens[1] != "loads")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);

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
    // Trap all input file errors here
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
    if (m_nDebugLevel < 0 || m_nDebugLevel > 1) 
        IOErrorHandler (ERRORCODE::DEBUGCODE);
}

void CElement::ReadFrameModel ()
// ---------------------------------------------------------------------------
// Function: Reads the rest of the frame data from input file
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    CVector<float> fVC(3);   // Stores float input
    bool bEOF = false;       // End of file flag

    try
    {
        // Rewind the file to read the input file again
        m_FIO.Rewind (m_FileInput);
        m_nLineNumber = 0;

        // Header line
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);

        // Unit Mode
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);
        m_Parse.GetTokens (m_FileInput, m_nLineNumber, m_strVTokens,
                           m_nTokens, m_strDelimiters, m_strComment,
                           bEOF);

        // Member Type
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_strVTokens[0] == "column") {
            CElement::SetType(ElementType::COLUMN);
        }
        else if (m_strVTokens[0] == "beam") {
            CElement::SetType(ElementType::BEAM);
        }
        else {
            IOErrorHandler(ERRORCODE::INVALIDINPUT);
        }


        // Read concrete material data
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_nTokens != 3)
            IOErrorHandler(ERRORCODE::INVALIDINPUT);
        m_Parse.GetFloatValue(m_strVTokens[0], fVC(1));
        m_Parse.GetFloatValue(m_strVTokens[1], fVC(2));
        m_Parse.GetFloatValue(m_strVTokens[2], fVC(3));
        if (fVC(1) <= 0.0f || fVC(2) <= 0.0f || fVC(3) <= 0.0f)
            IOErrorHandler(ERRORCODE::MATPROPERTY);

        m_ConcMatData(1).SetCompStr(fVC(1));
        m_ConcMatData(1).SetDensity(fVC(2));
        m_ConcMatData(1).SetPoisson(fVC(3));

        // Read reinforcement material data
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_nTokens != 3)
            IOErrorHandler(ERRORCODE::INVALIDINPUT);
        m_Parse.GetFloatValue(m_strVTokens[0], fVC(1));
        m_Parse.GetFloatValue(m_strVTokens[1], fVC(2));
        m_Parse.GetFloatValue(m_strVTokens[2], fVC(3));
        if (fVC(1) <= 0.0f || fVC(2) <= 0.0f || fVC(3) <= 0.0f)
            IOErrorHandler(ERRORCODE::MATPROPERTY);

        m_ConcMatData(1).SetYieldStr(fVC(1));
        m_ConcMatData(1).SetYM(fVC(2));
        m_ConcMatData(1).SetDensity(fVC(3));

        // read cross-sectional geometry data
        std::string strTag;
        CVector<float> fVXSDims(MAXEPDIM);
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);

        if (m_strVTokens[0] == "rectangular")
        {
            if (m_nTokens != 3)
                IOErrorHandler(ERRORCODE::INVALIDINPUT);
            m_Parse.GetFloatValue(m_strVTokens[1], fVXSDims(1));
            m_Parse.GetFloatValue(m_strVTokens[2], fVXSDims(2));
            if (fVXSDims(1) <= 0.0f || fVXSDims(2) <= 0.0f)
                IOErrorHandler(ERRORCODE::XSDIMENSION);
            m_EPData(1) = new CRectSolid(fVXSDims);
        }
        else if (m_strVTokens[0] == "t")
        {
            if (m_nTokens != 5)
                IOErrorHandler(ERRORCODE::INVALIDINPUT);
            m_Parse.GetFloatValue(m_strVTokens[1], fVXSDims(1));
            m_Parse.GetFloatValue(m_strVTokens[2], fVXSDims(2));
            m_Parse.GetFloatValue(m_strVTokens[3], fVXSDims(3));
            m_Parse.GetFloatValue(m_strVTokens[4], fVXSDims(4));
            if (fVXSDims(1) <= 0.0f || fVXSDims(2) <= 0.0f ||
                fVXSDims(3) <= 0.0f || fVXSDims(4) <= 0.0f)
                IOErrorHandler(ERRORCODE::XSDIMENSION);
            m_EPData(1) = new CTSection(fVXSDims);
        }
        else if (m_strVTokens[0] == "circular")
        {
            if (m_nTokens != 2)
                IOErrorHandler(ERRORCODE::INVALIDINPUT);
            m_Parse.GetFloatValue(m_strVTokens[1], fVXSDims(1));
            if (fVXSDims(1) <= 0.0f)
                IOErrorHandler(ERRORCODE::XSDIMENSION);
            m_EPData(1) = new CCircSolid(fVXSDims);
        }
        else
            IOErrorHandler(ERRORCODE::XSTYPE);

        // Read XS Reinforcements data
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        /*
        if (m_strVTokens[0] != "*XS REINFORCEMENTS")
            IOErrorHandler(ERRORCODE::XSREINFO);
            */
        std::string strType, strSize, strSpacing;
        float fRow, fNumBars, fSpacing, fHEdge, fVEdge;
        for (int i = 1; i <= m_nXSR; i++) {
            m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
                m_nTokens, m_strDelimiters, m_strComment,
                bEOF);

            strType = m_strVTokens[0];
            m_Parse.GetFloatValue(m_strVTokens[1], fRow);
            m_Parse.GetFloatValue(m_strVTokens[2], fNumBars);
            strSize = m_strVTokens[3];
            strSpacing = m_strVTokens[4];
            m_Parse.GetFloatValue(m_strVTokens[5], fHEdge);
            m_Parse.GetFloatValue(m_strVTokens[6], fVEdge);
            if (strSpacing == "eql") {
                m_ReData(i).SetXSReData(strType, fRow, fNumBars, strSize, strSpacing, fHEdge, fVEdge);
            }
            else {
                m_Parse.GetFloatValue(m_strVTokens[4], fSpacing);
                m_ReData(i).SetXSReData(strType, fRow, fNumBars, strSize, fSpacing, fHEdge, fVEdge);
            }
        }
        

        // Read Transverse Reinforcements data (NOT COMPLETED)
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        strType = m_strVTokens[0];
        m_Parse.GetFloatValue(m_strVTokens[1], fRow);
        strSize = m_strVTokens[2];
        /* EXPETED INPUT
        for (int i = 1; i <= 3; i++) {
            m_Parse.GetFloatValue(m_strVTokens[i + 2], fVC(i));
            m_Parse.GetFloatValue(m_strVTokens[i + 1], fVC(i + 1));
        }
        */
        // Read nodal loads
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_nTokens != 4)
            IOErrorHandler(ERRORCODE::INVALIDINPUT);
        strTag = m_strVTokens[0];
        if (strTag != "start")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);
        m_Parse.GetFloatValue(m_strVTokens[1], fVC(1));
        m_Parse.GetFloatValue(m_strVTokens[2], fVC(2));
        m_Parse.GetFloatValue(m_strVTokens[3], fVC(3));
        m_NodalLoadData(1).SetValues(fVC);

        m_Parse.GetTokens(m_FileInput, m_nLineNumber, m_strVTokens,
            m_nTokens, m_strDelimiters, m_strComment,
            bEOF);
        if (m_nTokens != 4)
            IOErrorHandler(ERRORCODE::INVALIDINPUT);
        strTag = m_strVTokens[0];
        if (strTag != "end")
            IOErrorHandler(ERRORCODE::INVALIDINPUT);
        m_Parse.GetFloatValue(m_strVTokens[1], fVC(1));
        m_Parse.GetFloatValue(m_strVTokens[2], fVC(2));
        m_Parse.GetFloatValue(m_strVTokens[3], fVC(3));
        m_NodalLoadData(2).SetValues(fVC);

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
            if (m_nTokens != 3)
                IOErrorHandler (ERRORCODE::INVALIDINPUT);
            strEType = m_strVTokens[0];
            m_Parse.GetFloatValue(m_strVTokens[1], fVELoads(1));
            m_Parse.GetFloatValue(m_strVTokens[2], fVELoads(2));
            if (strEType != "dly'" && strEType != "ploadx'" &&
                strEType != "ploady'" && strEType != "cmoment")
                IOErrorHandler (ERRORCODE::ELOADTYPE);

            if (strEType == "dly'") {
                m_ElementLoadData(counter).SetValues(1, CElementLoads::ELType::DISTRIBUTEDY,
                    fVELoads(1), fVELoads(2));
                counter += 1;
            }
            else if (strEType == "ploadx'") {
                m_ElementLoadData(counter).SetValues(1, CElementLoads::ELType::CONCENTRATEDX,
                    fVELoads(1), fVELoads(2));
                counter += 1;
            }
            else if (strEType == "ploady'") {
                m_ElementLoadData(counter).SetValues(1, CElementLoads::ELType::CONCENTRATEDY,
                    fVELoads(1), fVELoads(2));
                counter += 1;
            }
            else if (strEType == "cmoment") {
                m_ElementLoadData(counter).SetValues(1, CElementLoads::ELType::CONCENTRATEDM,
                    fVELoads(1), fVELoads(2));
                counter += 1;
            }
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

void CElement::CreateOutput ()
// ---------------------------------------------------------------------------
// Function: creates the output file
// Input:    none
// Output:   none
// ---------------------------------------------------------------------------
{
    /*// print analysis date and time
    m_FileOutput << '\n';
    m_FileOutput << "Starting out at : " << m_strDateTime << '\n';

    // problem description

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
    m_FileOutput << "-----------" << '\n';
    m_FileOutput << "NODAL FORCES" << '\n';
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

    // Print elapsed clock time
    m_FileOutput << '\n';
    m_FileOutput << "      Ending at : " << m_strEndDateTime;
    m_FileOutput << "      Elapsed clock time : " << Timer.DiffTime() << " s\n";
    */
}

void CElement::IOErrorHandler (ERRORCODE ECode) const
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

void CElement::TerminateProgram ()
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
