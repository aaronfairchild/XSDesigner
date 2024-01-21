/*********************************************
Program Planar Frame
Copyright(c) 2000-22, S. D. Rajan
All rights reserved

Modified by: Aaron Fairchild
Date: 11/20/2023

Introduction to Structural Analysis and Design
Object-Oriented Numerical Analysis
*********************************************/
#pragma once
const int NDIM = 2;		// spatial dimension
const int DOFPN = 3;	// dof per node
const int DOFPE = 6;	// dof per element
const int MAXEPDIM = 4;	// max. # cross-section dimensions
const double PI = 3.14159265;  // value of pi 
const float TOL = std::numeric_limits<float>::epsilon();