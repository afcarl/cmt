#include "utils.h"
#include "exception.h"
#include "pcatransform.h"

#include "Eigen/Eigenvalues"
using Eigen::SelfAdjointEigenSolver;

#include <iostream>
using std::cout;
using std::endl;

CMT::PCATransform::PCATransform(
	const ArrayXXd& input,
	const ArrayXXd& output,
	double varExplained,
	int numPCs)
{
	initialize(input, varExplained, numPCs, output.rows());
}



CMT::PCATransform::PCATransform(
	const ArrayXXd& input,
	double varExplained,
	int numPCs,
	int dimOut)
{
	initialize(input, varExplained, numPCs, dimOut);
}



CMT::PCATransform::PCATransform(
	const VectorXd& eigenvalues,
	const VectorXd& meanIn,
	const MatrixXd& preIn,
	const MatrixXd& preInInv,
	int dimOut) :
	AffineTransform(meanIn, preIn, preInInv, dimOut),
	mEigenvalues(eigenvalues)
{
}



CMT::PCATransform::PCATransform(const PCATransform& transform) :
	AffineTransform(transform),
	mEigenvalues(transform.mEigenvalues)
{
}



void CMT::PCATransform::initialize(
	const ArrayXXd& input,
	double varExplained,
	int numPCs,
	int dimOut)
{
	mMeanOut = VectorXd::Zero(dimOut);
	mPreOut = MatrixXd::Identity(dimOut, dimOut);
	mPreOutInv = MatrixXd::Identity(dimOut, dimOut);
	mGradTransform = MatrixXd::Zero(dimOut, input.rows());
	mLogJacobian = 0.;

	if(input.rows() < 1)
		return;

	mMeanIn = input.rowwise().mean();

	// compute covariances
	MatrixXd covXX = covariance(input);

	SelfAdjointEigenSolver<MatrixXd> eigenSolver;
	eigenSolver.compute(covXX);
	mEigenvalues = eigenSolver.eigenvalues();

	if(numPCs < 0) {
		double totalVariance = mEigenvalues.sum();
		double varExplainedSoFar = 0.;
		numPCs = 0;

		for(int i = mEigenvalues.size() - 1; i >= 0; --i) {
			numPCs += 1;
			varExplainedSoFar += mEigenvalues[i] / totalVariance * 100.;

			if(varExplainedSoFar > varExplained)
				break;
		}
	} else if(numPCs > mEigenvalues.size()) {
		numPCs = mEigenvalues.size();
	}

	mPredictor = MatrixXd::Zero(dimOut, numPCs);

	// make sure directions of zero variance aren't touched
	VectorXd tmp = mEigenvalues;
	for(int i = 0; i < tmp.size(); ++i)
		if(tmp[i] < 1e-8)
			tmp[i] = 1.;

	// input whitening
	mPreIn = tmp.tail(numPCs).cwiseSqrt().cwiseInverse().asDiagonal() *
		eigenSolver.eigenvectors().rightCols(numPCs).transpose();
	mPreInInv = eigenSolver.eigenvectors().rightCols(numPCs) *
		tmp.tail(numPCs).cwiseSqrt().asDiagonal();
}
