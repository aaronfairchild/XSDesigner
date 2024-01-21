/*********************************************
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Intermediate Structural Analysis and Design and
Object-Oriented Numerical Analysis via C++

Contains the matrix tolbox class
Routes all errors through the CLocalErrorHandler class

List of improvements made:

*********************************************/
#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include "..\LibraryEXH\arraycontainersEXH.h"
#include "LocalErrorHandler.h"

const int NUM_ELEMENTS_PER_LINE = 5;  // # of vector/matrix elements per line
const int FW = 16;                    // field width

template <class T>
class CMatToolBox
{
public:
    enum class Error
    {
        VECERR_ADD, VECERR_SUBTRACT, VECERR_DOTPRODUCT, VECERR_CROSSPRODUCT,
        VECERR_NORMALIZE,
        MATERR_ADD, MATERR_SUBTRACT, MATERR_MULTIPLY, MATERR_DETERMINANT,
        MATERR_TRANSPOSE, MATERR_MATMULTVEC, MATERR_LUFACTORIZATION_DIM,
        MATERR_LUFACTORIZATION_DEPENDENT, MATERR_LUSOLVE_DIM,
        MATERR_LUSOLVE_DEPENDENT, MATERR_LDLTFACTORIZATION_SQUARE,
        MATERR_LDLTFACTORIZATION_SYM, MATERR_LDLTFACTORIZATION_POSDEF, MATERR_LDLTSOLVE,
        MATERR_GAUSSELIMINATION, MATERR_SINGULARMATRIX, MATERR_NOTPOSDEFMATRIX,
        MATERR_RESIDUALVECTOR, UNSUPPORTEDOPERATION
    };
    CMatToolBox();
    ~CMatToolBox();

    // vector-related functions
    void Display(const std::string& strMessage,
        const CVector<T>& A) const;
    void Add(const CVector<T>& A, const CVector<T>& B,
        CVector<T>& C);
    void Subtract(const CVector<T>& A, const CVector<T>& B,
        CVector<T>& C);
    T DotProduct(const CVector<T>& A,
        const CVector<T>& B, T& product);
    void Normalize(CVector<T>& A);
    void Scale(CVector<T>& A, const T factor);
    T    MaxValue(const CVector<T>& A) const;
    T    MinValue(const CVector<T>& A) const;
    T    TwoNorm(const CVector<T>& A);
    T    MaxNorm(const CVector<T>& A) const;
    void CrossProduct(const CVector<T>& A,
        const CVector<T>& B, CVector<T>& C);

    // matrix-related functions
    void Display(const std::string& strMessage,
        const CMatrix<T>& A) const;
    void Add(const CMatrix<T>& A, const CMatrix<T>& B,
        CMatrix<T>& C);
    void Subtract(const CMatrix<T>& A,
        const CMatrix<T>& B, CMatrix<T>& C);
    void Multiply(const CMatrix<T>& A,
        const CMatrix<T>& B, CMatrix<T>& C);
    void Determinant(const CMatrix<T>& A, T& det);
    void Scale(CMatrix<T>& A, const T scale);
    T    MaxNorm(const CMatrix<T>& A) const;
    void Transpose(const CMatrix<T>& A,
        CMatrix<T>& B);
    void Inverse(const CMatrix<T>& A,
        CMatrix<T>& B, const T TOL);
    void MatMultVec(const CMatrix<T>& A,
        const CVector<T>& x,
        CVector<T>& b);
    void LUFactorization(CMatrix<T>& A, const T TOL);
    void LUSolve(const CMatrix<T>& A,
        CVector<T>& x, const CVector<T>& b);
    void GaussElimination(CMatrix<T>& A,
        CVector<T>& x,
        CVector<T>& b,
        const T TOL);
    void LDLTFactorization(CMatrix<T>& A, const T TOL);
    void LDLTSolve(const CMatrix<T>& A, CVector<T>& x,
        const CVector<T>& b);

    // helper functions
    void ResidualVector(const CMatrix<T>& A, const CVector<T>& x,
        const CVector<T>& b, CVector<T>& R,
        T& AbsError, T& RelError);
    bool IsEqual(const T d1, const T d2, T TOL = TOLDEF) const;
    bool IsEqual(const CMatrix<T>& dMA,
        const CMatrix<T>& dMB, T TOL = TOLDEF) const;
    bool IsEqual(const CVector<T>& dVA,
        const CVector<T>& dVB, T TOL = TOLDEF) const;
    void GetFLOPStats(double& dAS, double& dM, double& dD) const;
    void PrintVector(const CVector<T>& A, const std::string& strMessage,
        std::ostream& Out) const;
    void PrintMatrixRowWise(CMatrix<T>& A, const std::string& heading,
        std::ostream& Out) const;
    void PrintMatrixColumn(CMatrix<T>& A, const std::string& heading, int i,
        std::ostream& Out) const;
    void PrintMatrixColumnWise(CMatrix<T>& A, const std::string& heading,
        std::ostream& Out) const;

private:
    // these are declared double to avoid integer overflow
    double m_dASOP; // # of floating point additions and subtractions
    double m_dMOP;  // # of floating point multiplications
    double m_dDOP;  // # of floating point divisions
    T m_c;          // output scalar
    void ErrorHandler(CMatToolBox<T>::Error err) const;

protected:
};

// ctor
template <class T>
CMatToolBox<T>::CMatToolBox()
// ==================================================================
// Function: default constructor
//    Input: none
//   Output: none
// ==================================================================
{
    m_dASOP = m_dMOP = m_dDOP = 0.0;
    m_c = 0.0;
}

// dtor
template <class T>
CMatToolBox<T>::~CMatToolBox()
// ==================================================================
// Function: destructor
//    Input: none
//   Output: none
// ==================================================================
{
}

// ---------------------------------------------------------------
// ------------------------ vector functions ---------------------
// ---------------------------------------------------------------
template <class T>
void CMatToolBox<T>::Display(const std::string& strMessage,
    const CVector<T>& A) const
    // ==================================================================
    // Function: displays a message and the elements of a vector
    //    Input: message string and the vector 
    //   Output: None
    // ==================================================================
{
    std::cout << '\n' << strMessage << '\n';
    std::cout.setf(std::ios::left);
    for (int i = 1; i <= A.GetSize(); i++)
    {
        std::cout << "(" << i << ") "
            << std::setw(FW) << A(i) << " ";
        if ((i % NUM_ELEMENTS_PER_LINE) == 0)
            std::cout << '\n';
    }
}

template <class T>
void CMatToolBox<T>::Add(const CVector<T>& A, const CVector<T>& B,
    CVector<T>& C)
    // ==================================================================
    // Function: adds two vectors and stores the result in the
    //           third vector C = A + B
    //    Input: vectors A and B 
    //   Output: vector C
    // ==================================================================
{
    // check for incompatible vectors
    int n = A.GetSize();
    if (n != B.GetSize() || n != C.GetSize())
        ErrorHandler(CLocalErrorHandler::ERRORCODE::MTB_VECTORADDERROR);

    // add
    for (int i = 1; i <= n; i++)
        C(i) = A(i) + B(i);
    m_dASOP += static_cast<double>(n);
}

template <class T>
void CMatToolBox<T>::Subtract(const CVector<T>& A,
    const CVector<T>& B, CVector<T>& C)
    // ==================================================================
    // Function: subtracts one vector from another and stores the result
    //           in the third vector C = A - B
    //    Input: vectors A and B 
    //   Output: vector C
    // ==================================================================
{
    // check for incompatible vectors
    int n = A.GetSize();
    if (n != B.GetSize() || n != C.GetSize())
        ErrorHandler(Error::VECERR_ADD);

    // add
    for (int i = 1; i <= n; i++)
        C(i) = A(i) - B(i);
    m_dASOP += static_cast<double>(n);
}

template <class T>
T CMatToolBox<T>::DotProduct(const CVector<T>& A,
    const CVector<T>& B, T& product)
    // ==================================================================
    // Function: computes the dot product of two vectors such that
    //           product = A dot B
    //    Input: vectors A and B 
    //   Output: product 
    // ==================================================================
{
    // check for incompatible vectors
    int n = A.GetSize();
    if (n != B.GetSize())
        ErrorHandler(Error::VECERR_ADD);
    m_c = 0;
    for (int i = 1; i <= n; i++) {
        m_c = m_c + A(i) * B(i);
    }
    m_dMOP += static_cast<double>(n);
    m_dASOP += static_cast<double>(n);

    return m_c;
}

template <class T>
void CMatToolBox<T>::Normalize(CVector<T>& A)
// ==================================================================
// Function: normalizes a vector
//    Input: vector A 
//   Output: normalized vector A 
// ==================================================================
{
    int n = A.GetSize();
    m_c = TwoNorm(A);    // Length of the vector
    // check if the length is close to zero
    if (abs(m_c) < std::numeric_limits<T>::epsilon()) {
        ErrorHandler(Error::VECERR_NORMALIZE);
    }
    for (int i = 1; i <= n; i++) {      // dividing each element by length
        A(i) = A(i) / m_c;
    }

    m_dDOP += static_cast<double>(n);
}

template <class T>
void CMatToolBox<T>::Scale(CVector<T>& A, T c)
// ==================================================================
// Function: scales a vector by a constant c such that A = c A
//    Input: vector A and constant c 
//   Output: scaled vector A
// ==================================================================
{
    int n = A.GetSize();
    for (int i = 1; i <= n; i++) {
        A(i) = A(i) * c;
    }

    m_dMOP += static_cast<double>(n);
}

template <class T>
T CMatToolBox<T>::MaxValue(const CVector<T>& A) const
// ==================================================================
// Function: finds the largest value among all the elements in A
//    Input: vector A 
//   Output: return value is the largest element in A
// ==================================================================
{
    int n = A.GetSize();
    T local_Max = A(1);        // used as max value
    for (int i = 2; i <= n; i++) {
        if (A(i) > local_Max) {
            local_Max = A(i);
        }
    }

    return local_Max;
}

template <class T>
T CMatToolBox<T>::MinValue(const CVector<T>& A) const
// ==================================================================
// Function: finds the smallest value among all the elements in A
//    Input: vector A 
//   Output: return value is the smallest element in A
// ==================================================================
{
    int n = A.GetSize();
    T local_Min = A(1);        // used as min value
    for (int i = 2; i <= n; i++) {
        if (A(i) < local_Min) {
            local_Min = A(i);
        }
    }
    return local_Min;
}

template <class T>
T CMatToolBox<T>::TwoNorm(const CVector<T>& A)
// ==================================================================
// Function: computes the two norm of vector A
//    Input: vector A 
//   Output: return value is the two-norm
// ==================================================================
{
    int n = A.GetSize();
    m_c = 0;    // Used to sum up the squares of each element
    for (int i = 1; i <= n; i++) {
        m_c = m_c + (A(i) * A(i));
    }
    m_dMOP += static_cast<double>(n);
    m_dASOP += static_cast<double>(n);
    m_c = sqrt(m_c);    // Length of the vector
    return m_c;
}

template <class T>
T CMatToolBox<T>::MaxNorm(const CVector<T>& A) const
// ==================================================================
// Function: computes the max norm of vector A
//    Input: vector A 
//   Output: return value is the max norm
// ==================================================================
{
    int n = A.GetSize();
    T local_max = abs(A(1));
    for (int i = 2; i <= n; i++) {
        if (abs(A(i)) > local_max) {
            local_max = abs(A(i));
        }
    }
    return local_max;
}

template <class T>
void CMatToolBox<T>::CrossProduct(const CVector<T>& A,
    const CVector<T>& B,
    CVector<T>& C)
    // ==================================================================
    // Function: computes the cross-product of two vectors and stores the
    //           result in the third vector such that C = A x B
    //           (3-dimensional space)
    //    Input: vectors A, B and C
    //   Output: vector C
    // ==================================================================
{
    int n = A.GetSize();
    int dim = 3;
    // check for three-dimensional vectors
    if (n != dim || B.GetSize() != dim || C.GetSize() != dim) {
        ErrorHandler(Error::VECERR_CROSSPRODUCT);
    }

    C(1) = (A(2) * B(3)) - (A(3) * B(2));
    C(2) = -1 * ((A(1) * B(3)) - (A(3) * B(1)));
    C(3) = (A(1) * B(2)) - (A(2) * B(1));

    m_dMOP += static_cast<double>(7.0);
    m_dASOP += static_cast<double>(3.0);
}

// ---------------------------------------------------------------
// ------------------------ matrix functions ---------------------
// ---------------------------------------------------------------
template <class T>
void CMatToolBox<T>::Display(const std::string& strMessage,
    const CMatrix<T>& A) const
    // ==================================================================
    // Function: displays a message and the elements of a matrix
    //           rowwise
    //    Input: message string and the matrix
    //   Output: None
    // ==================================================================
{
    std::cout << '\n' << strMessage << '\n';
    std::cout.setf(std::ios::left);
    for (int i = 1; i <= A.GetRows(); i++)
    {
        int nC = 0;
        for (int j = 1; j <= A.GetColumns(); j++)
        {
            ++nC;
            std::cout << "(" << i << "," << j << ") "
                << std::setw(FW) << A(i, j) << " ";
            if ((nC % NUM_ELEMENTS_PER_LINE) == 0)
                std::cout << '\n';
        }
        std::cout << '\n';
    }
}

template <class T>
void CMatToolBox<T>::Add(const CMatrix<T>& A, const CMatrix<T>& B,
    CMatrix<T>& C)
    // ==================================================================
    // Function: adds two matrices and stores the result in the
    //           third matrix C = A + B
    //    Input: matrices A and B 
    //   Output: matrix C
    // ==================================================================
{
    int nRows = A.GetRows();
    int nCols = A.GetColumns();
    // Check all matrix dimensions match
    if (B.GetRows() != nRows || B.GetColumns() != nCols || C.GetRows() != nRows || C.GetColumns() != nCols) {
        ErrorHandler(Error::MATERR_ADD);
    }

    for (int i = 1; i <= nRows; i++) {  // loop through rows
        for (int j = 1; j <= nCols; j++) {  // loop through columns
            C(i, j) = A(i, j) + B(i, j);    // add each element
        }
    }

    m_dASOP += static_cast<double>(nRows * nCols);
}

template <class T>
void CMatToolBox<T>::Subtract(const CMatrix<T>& A,
    const CMatrix<T>& B, CMatrix<T>& C)
    // ==================================================================
    // Function: subtracts one matrix from another and stores the result
    //           in the third matrix C = A + B
    //    Input: matrices A and B 
    //   Output: matrix C
    // ==================================================================
{
    int nRows = A.GetRows();
    int nCols = A.GetColumns();
    // Check all matrix dimensions match
    if (B.GetRows() != nRows || B.GetColumns() != nCols || C.GetRows() != nRows || C.GetColumns() != nCols) {
        ErrorHandler(Error::MATERR_SUBTRACT);
    }

    for (int i = 1; i <= nRows; i++) {  // loop through rows
        for (int j = 1; j <= nCols; j++) {  // loop through columns
            C(i, j) = A(i, j) - B(i, j);    // subtract each element
        }
    }

    m_dASOP += static_cast<double>(nRows * nCols);
}

template <class T>
void CMatToolBox<T>::Multiply(const CMatrix<T>& A,
    const CMatrix<T>& B, CMatrix<T>& C)
    // ==================================================================
    // Function: multiplies two matrices and stores the result
    //           in the third matrix C = A * B
    //    Input: matrices A and B 
    //   Output: matrix C
    // ==================================================================
{
    int m = A.GetRows();
    int n = A.GetColumns();
    int p = B.GetColumns();
    // Check all matrix dimensions match
    if (B.GetRows() != n || C.GetRows() != m || C.GetColumns() != p) {
        ErrorHandler(Error::MATERR_MULTIPLY);
    }

    C.Set(0);   // Make sure C is all zeros to start

    for (int i = 1; i <= C.GetRows(); i++)
    {
        for (int j = 1; j <= C.GetColumns(); j++)
        {
            T sum = 0.0;
            for (int k = 1; k <= A.GetColumns(); k++)
            {
                sum += A(i, k) * B(k, j);
            }
            C(i, j) = sum;
        }
    }
}

template <class T>
void CMatToolBox<T>::Determinant(const CMatrix<T>& A, T& det)
// ==================================================================
// Function: computes the determinant of matrix A
//    Input: matrix A and variable to hold the determinant
//   Output: determinant
// ==================================================================
{
    ErrorHandler(Error::UNSUPPORTEDOPERATION);
}

template <class T>
void CMatToolBox<T>::Scale(CMatrix<T>& A, T c)
// ==================================================================
// Function: scales all the elements of a matrix by a constant c
//           such that A = c A
//    Input: matrix A and constant c
//   Output: scaled matrix A
// ==================================================================
{
    int nRows = A.GetRows();
    int nCols = A.GetColumns();

    for (int i = 1; i <= nRows; i++) {  // loop through rows
        for (int j = 1; j <= nCols; j++) {  // loop through columns
            A(i, j) = A(i, j) * c;    // multiply by the scalar
        }
    }

    m_dMOP += static_cast<double>(nRows * nCols);
}

template <class T>
T CMatToolBox<T>::MaxNorm(const CMatrix<T>& A) const
// ==================================================================
// Function: computes the max norm of matrix A
//    Input: matrix A 
//   Output: return value is the max norm
// ==================================================================
{
    int nRows = A.GetRows();
    int nCols = A.GetColumns();

    T local_max = std::numeric_limits<T>::min();

    for (int i = 1; i <= nRows; i++) {
        for (int j = 1; j <= nCols; j++) {
            if (abs(A(i, j)) > local_max) {
                local_max = abs(A(i, j));
            }
        }
    }
    return local_max;
}

template <class T>
void CMatToolBox<T>::Transpose(const CMatrix<T>& A,
    CMatrix<T>& B)
    // ==================================================================
    // Function: computes the transpose of a matrix and stores the result
    //           in another matrix B = A(T)
    //    Input: matrices A and B
    //   Output: matrix B
    // ==================================================================
{
    int m = A.GetRows();
    int n = A.GetColumns();

    // Check matrix dimensions match
    if (B.GetRows() != n || B.GetColumns() != m) {
        ErrorHandler(Error::MATERR_TRANSPOSE);
    }

    for (int i = 1; i <= A.GetRows(); i++)
    {
        for (int j = 1; j <= A.GetColumns(); j++)
        {
            B(j, i) = A(i, j);
        }
    }
}

template <class T>
void CMatToolBox<T>::MatMultVec(const CMatrix<T>& A,
    const CVector<T>& x,
    CVector<T>& b)
    // ==================================================================
    // Function: multiplies a matrix and a vector and stores the result
    //           in a vector b = A * x
    //    Input: vectors A and x 
    //   Output: vector b
    // ==================================================================
{
    int m = A.GetRows();
    int n = A.GetColumns();
    int p = x.GetSize();
    // Check all matrix dimensions match
    if (n != p || b.GetSize() != m) {
        ErrorHandler(Error::MATERR_MATMULTVEC);
    }

    b.Set(0);   // Make sure b is all zeros to start


    for (int i = 1; i <= A.GetRows(); i++)
    {
        T sum = 0.0;
        for (int j = 1; j <= A.GetColumns(); j++)
        {
            sum += A(i, j) * x(j);
        }
        b(i) = sum;
    }
}

template <class T>
void CMatToolBox<T>::LUFactorization(CMatrix<T>& A, T TOL)
// ==================================================================
// Function: carries out LU factorization of matrix A
//           A is replaced with L and U
//    Input: matrix A and tolerance value to detect singular A
//   Output: matrix A 
// ==================================================================
{
    int m = A.GetRows();
    int n = A.GetColumns();
    // Check matrix is square
    if (m != n) {
        ErrorHandler(Error::MATERR_LUFACTORIZATION_DIM);
    }

    T c;     // for storing sums

    for (int j = 1; j <= m; j++) {      // loop throu rows
        if (abs(A(j, j)) <= TOL)        // check for dependence
            ErrorHandler(Error::MATERR_LUFACTORIZATION_DEPENDENT);

        for (int i = 1; i <= j; i++) {  // loop for U
            c = 0.0;
            for (int k = 1; k <= (i - 1); k++) {
                c = c + (A(i, k) * A(k, j));
                m_dASOP += static_cast<double>(1);
                m_dMOP += static_cast<double>(1);
            }
            A(i, j) = A(i, j) - c;
            m_dASOP += static_cast<double>(1);
        }



        if (abs(A(j, j)) <= TOL)        // check for dependence in U
            ErrorHandler(Error::MATERR_LUFACTORIZATION_DEPENDENT);

        for (int i = (j + 1); i <= n; i++) {  // loop for L
            c = 0.0;
            for (int k = 1; k <= (j - 1); k++) {
                c = c + (A(i, k) * A(k, j));
                m_dASOP += static_cast<double>(1);
                m_dMOP += static_cast<double>(1);
            }
            A(i, j) = ((A(i, j) - c) / A(j, j));
            m_dASOP += static_cast<double>(1);
            m_dDOP += static_cast<double>(1);
        }
    }
}

template <class T>
void CMatToolBox<T>::LUSolve(const CMatrix<T>& A,
    CVector<T>& x,
    const CVector<T>& b)
    // ==================================================================
    // Function: carries out forward and backward substitution so as to
    //           solve A x = b. A contains L and U terms.
    //    Input: matrix A, vectors x and b
    //   Output: vector x 
    // ==================================================================
{
    int m = A.GetRows();
    int n = A.GetColumns();

    // Check matrix is square, and dims of b and x
    if (m != n || x.GetSize() != n || b.GetSize() != n) {
        ErrorHandler(Error::MATERR_LUSOLVE_DIM);
    }

    CVector<T> y(n);
    T c;
    y(1) = b(1);

    // Forward Sub
    for (int i = 2; i <= n; i++) {
        c = 0.0;
        for (int j = 1; j <= (i - 1); j++) {
            c = c + A(i, j) * y(j);
            m_dASOP += static_cast<double>(1);
            m_dMOP += static_cast<double>(1);
        }
        y(i) = b(i) - c;
        m_dASOP += static_cast<double>(1);
        m_dDOP += static_cast<double>(1);
    }

    // Backward Sub
    x(n) = (y(n) / A(n, n));
    for (int i = (n - 1); i >= 1; --i) {
        c = 0.0;
        for (int j = (i + 1); j <= n; j++) {
            c = c + A(i, j) * x(j);
            m_dASOP += static_cast<double>(1);
            m_dMOP += static_cast<double>(1);
        }
        x(i) = (y(i) - c) / A(i, i);
        m_dASOP += static_cast<double>(1);
        m_dDOP += static_cast<double>(1);
    }
}

template <class T>
void CMatToolBox<T>::GaussElimination(CMatrix<T>& A,
    CVector<T>& x,
    CVector<T>& b,
    T TOL)
    // ==================================================================
    // Function: solves A x = b using Gaussian Elimination Technique
    //           (this version only for one rhs vector)
    //    Input: Matrices A and b
    //   Output: Matrix x
    // ==================================================================
{
    int i, j, k;   // loop indices
    T c;           // multiplier (Step 4)

    // number of equations to solve
    int n = A.GetRows();

    // x initially contains b
    x = b;

    // forward elimination
    for (k = 1; k <= n - 1; k++)              // Step 1
    {
        if (fabs(A(k, k)) <= TOL)          // Step 2
            ErrorHandler(Error::MATERR_GAUSSELIMINATION);
        for (i = k + 1; i <= n; i++)          // Step 3
        {
            c = A(i, k) / A(k, k);            // Step 4
            for (j = k + 1; j <= n; j++)      // Step 5
                A(i, j) -= c * A(k, j);     // Step 6
            x(i) -= c * x(k);             // Step 8
        }                                 // Step 9
        int nC = n - k;
        if (nC > 0)
        {
            m_dDOP += static_cast<double>(nC);
            m_dMOP += static_cast<double>(nC * nC);
            m_dASOP += static_cast<double>(nC + nC * nC);
        }
    }                                     // Step 10 

    // back substitution
    if (fabs(A(n, n)) <= TOL)
        ErrorHandler(Error::MATERR_GAUSSELIMINATION);
    x(n) /= A(n, n);                       // Step 11

    for (i = n - 1; i >= 1; i--)              // Step 12
    {
        T sum = T(0);
        for (j = i + 1; j <= n; j++)
            sum += A(i, j) * x(j);         // Step 13
        if ((n - i) > 0)
        {
            m_dASOP += static_cast<double>(n - i);
            m_dMOP += static_cast<double>(n - i);
        }
        x(i) = (x(i) - sum) / A(i, i);       // Step 14
    }                                     // Step 15
    m_dASOP += static_cast<double>(n);
    m_dDOP += static_cast<double>(n + 1);
}

template <class T>
void CMatToolBox<T>::LDLTFactorization(CMatrix<T>& A,
    T TOL)
    // ==================================================================
    // Function: carries out LDL(T) factorization of matrix A
    //           A is replaced with L and D. A is a symmetric matrix.
    //    Input: matrix A and tolerance value to detect singular A
    //   Output: matrix A 
    // ==================================================================
{
    int m = A.GetRows();
    int n = A.GetColumns();

    // Check matrix is square, and dims of b and x
    if (m != n) {
        ErrorHandler(Error::MATERR_LDLTFACTORIZATION_SQUARE);
    }

    T c;    // summing variable
    for (int i = 1; i <= n; i++) {
        c = 0.0;
        for (int j = 1; j <= (i - 1); j++) {        // find diagonals
            c = c + A(i, j) * A(i, j) * A(j, j);
            m_dASOP += static_cast<double>(1);
            m_dMOP += static_cast<double>(2);
        }
        A(i, i) = A(i, i) - c;
        m_dASOP += static_cast<double>(1);
        if (A(i, i) <= TOL) {                  // check for positive definite
            ErrorHandler(Error::MATERR_LDLTFACTORIZATION_POSDEF);
        }

        for (int j = (i + 1); j <= n; j++) {
            c = 0.0;
            for (int k = 1; k <= (i - 1); k++) {
                c = c + A(j, k) * A(k, k) * A(i, k);
                m_dASOP += static_cast<double>(1);
                m_dMOP += static_cast<double>(2);
            }
            A(j, i) = (A(j, i) - c) / A(i, i);
            m_dASOP += static_cast<double>(1);
            m_dDOP += static_cast<double>(1);
        }
    }
}

template <class T>
void CMatToolBox<T>::LDLTSolve(const CMatrix<T>& A,
    CVector<T>& x,
    const CVector<T>& b)
    // ==================================================================
    // Function: carries out forward and backward substitution so as to
    //           solve A x = b. A contains L and D terms.
    //    Input: matrix A, vectors x and b
    //   Output: vector x 
    // ==================================================================
{
    int m = A.GetRows();
    int n = A.GetColumns();

    // Check matrix is square, and dims of b and x
    if (m != n || x.GetSize() != n || b.GetSize() != n) {
        ErrorHandler(Error::MATERR_LDLTSOLVE);
    }

    CVector<T> y(n);
    T c;
    y(1) = b(1);

    // Forward Sub
    for (int i = 2; i <= n; i++) {
        c = 0.0;
        for (int j = 1; j <= (i - 1); j++) {
            c = c + A(i, j) * y(j);
            m_dASOP += static_cast<double>(1);
            m_dMOP += static_cast<double>(1);
        }
        y(i) = (b(i) - c);
        m_dASOP += static_cast<double>(1);
    }

    // Backward Sub
    x(n) = (y(n) / A(n, n));
    for (int i = (n - 1); i >= 1; --i) {
        c = 0.0;
        for (int j = (i + 1); j <= n; j++) {
            c = c + A(j, i) * x(j);
            m_dASOP += static_cast<double>(1);
            m_dMOP += static_cast<double>(1);
        }
        x(i) = (y(i) / A(i, i)) - c;
        m_dASOP += static_cast<double>(1);
        m_dDOP += static_cast<double>(1);
    }
}

template <class T>
void CMatToolBox<T>::GetFLOPStats(double& dAS, double& dM,
    double& dD) const
    // ==================================================================
    // Function: retrieves floating point operations
    //    Input: variables to store +-, * and / operations
    //   Output: variables with their values
    // ==================================================================
{
    dAS = m_dASOP;
    dM = m_dMOP;
    dD = m_dDOP;
}

template <class T>
void CMatToolBox<T>::ResidualVector(const CMatrix<T>& A,
    const CVector<T>& x,
    const CVector<T>& b,
    CVector<T>& R,
    T& AbsError, T& RelError)
    // ==================================================================
    // Function: computes the residual vector arising from the solution
    //           of A x = b, i.e., R = A x - b
    //    Input: matrix A, vectors x and b, 
    //   Output: vector R, abs. and rel. error values via TwoNorm function
    // ==================================================================
{
    // check for incompatible sizes
    int n = A.GetRows();
    int nCols = A.GetColumns();
    int nx = x.GetSize();
    int nb = b.GetSize();

    if (n != nx || nCols != nx || nx != nb)
        ErrorHandler(Error::MATERR_RESIDUALVECTOR);

    MatMultVec(A, x, R);
    for (int i = 1; i <= n; i++)
    {
        R(i) -= b(i);
    }
    m_dASOP += static_cast<double>(n);
    AbsError = TwoNorm(R);
    RelError = TwoNorm(R) / TwoNorm(b);
}

template <class T>
void CMatToolBox<T>::PrintVector(const CVector<T>& A, const std::string& strMessage,
    std::ostream& Out) const
    // ==================================================================
    // Function: displays a message and the elements of a vector
    //    Input: vector, a heading, output stream object 
    //   Output: None
    // ==================================================================
{
    Out << '\n' << strMessage << '\n';
    Out.setf(std::ios::left);
    for (int i = 1; i <= A.GetSize(); i += NUM_ELEMENTS_PER_LINE)
    {
        for (int k = i; k <= std::min(i + NUM_ELEMENTS_PER_LINE - 1, A.GetSize());
            k++)
        {
            Out << "[" << std::setw(4) << k << "]";
            Out << std::setw(15) << A(k) << " ";
        }
        Out << '\n';
    }
}

template <class T>
void CMatToolBox<T>::PrintMatrixRowWise(CMatrix<T>& A, const std::string& heading,
    std::ostream& Out) const
    // ---------------------------------------------------------------------------
    // Function: outputs a matrix into stream Out
    // Input:    matrix, a heading, output stream object
    // Output:   none
    // ---------------------------------------------------------------------------
{
    Out << '\n' << heading << '\n';
    Out << std::setiosflags(std::ios::left);

    int nRows = A.GetRows();
    int nColumns = A.GetColumns();
    for (int i = 1; i <= nRows; i++)
    {
        Out << "Row No: " << i << '\n';
        for (int j = 1; j <= nColumns; j = j + NUM_ELEMENTS_PER_LINE)
        {
            for (int k = j; k <= std::min(j + NUM_ELEMENTS_PER_LINE - 1, nColumns); k++)
            {
                Out << "[" << std::setw(4) << k << "]";
                Out << std::setw(15) << A(i, k) << " ";
            }
            Out << '\n';
        }
    }
}

template <class T>
void CMatToolBox<T>::PrintMatrixColumnWise(CMatrix<T>& A, const std::string& heading,
    std::ostream& Out) const
    // ---------------------------------------------------------------------------
    // Function: outputs a matrix into stream Out
    // Input:    matrix, a heading, output stream object
    // Output:   none
    // ---------------------------------------------------------------------------
{
    Out << '\n' << heading << '\n';
    Out << std::setiosflags(std::ios::left);

    int nRows = A.GetRows();
    int nColumns = A.GetColumns();
    for (int i = 1; i <= nColumns; i++)
    {
        Out << "Column No: " << i << '\n';
        for (int j = 1; j <= nRows; j = j + NUM_ELEMENTS_PER_LINE)
        {
            for (int k = j; k <= std::min(j + NUM_ELEMENTS_PER_LINE - 1, nRows); k++)
            {
                Out << "[" << std::setw(4) << k << "]";
                Out << std::setw(15) << A(k, i) << " ";
            }
            Out << '\n';
        }
    }
}

template <class T>
void CMatToolBox<T>::PrintMatrixColumn(CMatrix<T>& A, const std::string& heading, int i,
    std::ostream& Out) const
    // ---------------------------------------------------------------------------
    // Function: outputs a column of the matrix into stream Out
    // Input:    matrix, a heading, column index, output stream object
    // Output:   none
    // ---------------------------------------------------------------------------
{
    Out << '\n' << heading << '\n';
    Out << std::setiosflags(std::ios::left);

    int nRows = A.GetRows();
    for (int j = 1; j <= nRows; j = j + NUM_ELEMENTS_PER_LINE)
    {
        for (int k = j; k <= std::min(j + NUM_ELEMENTS_PER_LINE - 1, nRows); k++)
        {
            Out << "[" << std::setw(4) << k << "]";
            Out << std::setw(15) << A(k, i) << " ";
        }
        Out << '\n';
    }
}

template <class T>
bool CMatToolBox<T>::IsEqual(const T d1, const T d2, T TOL) const
// ==================================================================
// Function: checks if d1 and d2 are 'nearly' equal
//    Input: d1, d2, tolerance to use
//   Output: true if they are
// ==================================================================
{
    return (abs(d1 - d2) <= TOL);
}

template <class T>
bool CMatToolBox<T>::IsEqual(const CMatrix<T>& dMA,
    const CMatrix<T>& dMB, T TOL) const
    // ==================================================================
    // Function: checks if matrices A and B are 'nearly' equal
    //    Input: A, B, tolerance to use
    //   Output: true if they are
    // ==================================================================
{
    for (int i = 1; i <= dMA.GetRows(); i++)
    {
        for (int j = 1; j <= dMA.GetColumns(); j++)
        {
            if (!IsEqual(dMA(i, j), dMB(i, j), TOL))
                return false;
        }
    }

    return true;
}

template <class T>
bool CMatToolBox<T>::IsEqual(const CVector<T>& dVA,
    const CVector<T>& dVB, T TOL) const
    // ==================================================================
    // Function: checks if vectors A and B are 'nearly' equal
    //    Input: A, B, tolerance to use
    //   Output: true if they are
    // ==================================================================
{
    for (int i = 1; i <= dVA.GetSize(); i++)
    {
        if (!IsEqual(dVA(i), dVB(i), TOL))
            return false;
    }

    return true;
}

template <class T>
void CMatToolBox<T>::ErrorHandler(Error err) const
// ---------------------------------------------------------------------------
// Function: gateway to error handling. useful for setting breakpoint in
//           the debugger
// Input:    error code
// Output:   none
// ---------------------------------------------------------------------------
{
    if (err == CMatToolBox<T>::Error::VECERR_ADD)
    {
        throw std::exception("Vector addition operation cannot take place due to "
            "incompatible vectors.");
    }
    else if (err == CMatToolBox<T>::Error::VECERR_NORMALIZE)
    {
        throw std::exception("Cannot normalize vector since norm is zero.");
    }
    else if (err == CMatToolBox<T>::Error::VECERR_CROSSPRODUCT)
    {
        throw std::exception("All vectors must have exactly THREE dimensions.");
    }
    else if (err == CMatToolBox<T>::Error::UNSUPPORTEDOPERATION)
    {
        throw std::exception("This functionality is not supported.");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_ADD)
    {
        throw std::exception("Matrices must be the same size for addition.");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_SUBTRACT)
    {
        throw std::exception("Matrices must be the same size for subtraction.");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_MULTIPLY)
    {
        throw std::exception("Cannot perform multiplication: improper dimensions of matrices.");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_TRANSPOSE)
    {
        throw std::exception("Cannot transpose matrix: dimensions do not match");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_MATMULTVEC)
    {
        throw std::exception("Cannot multiply matrix and vector: dimensions do not match");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_LUFACTORIZATION_DIM)
    {
        throw std::exception("LU Factorization: A must be a square matrix.");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_LUFACTORIZATION_DEPENDENT)
    {
        throw std::exception("LU Factorization: Dependent equations. Diagonal element too small.");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_LUSOLVE_DIM)
    {
        throw std::exception("LU Solve: Dimensions do not match.");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_LDLTFACTORIZATION_SQUARE)
    {
        throw std::exception("LDLT Factorization: A must be a square matrix.");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_LDLTFACTORIZATION_SYM)
    {
        throw std::exception("LDLT Factorization: A must be symmetric.");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_LDLTFACTORIZATION_POSDEF)
    {
        throw std::exception("LDLT Factorization: The matrix must be postive definite.");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_LDLTSOLVE)
    {
        throw std::exception("LDLT Solve: Dimensions do not match.");
    }
    else if (err == CMatToolBox<T>::Error::MATERR_GAUSSELIMINATION)
    {
        throw std::exception("Gaussian Elimination: Dependent equations. Diagonal element too small.");
    }
    else
        throw std::exception("MatrixToolBox: Unknown error");
}
