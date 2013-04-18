#include "utils.h"
#include "exception.h"
#include "affinepreconditioner.h"

#include "Eigen/Eigenvalues"
using Eigen::SelfAdjointEigenSolver;

#include <iostream>

CMT::AffinePreconditioner::AffinePreconditioner(
	const VectorXd& meanIn,
	const VectorXd& meanOut,
	const MatrixXd& preIn,
	const MatrixXd& preOut,
	const MatrixXd& predictor) :
	mMeanIn(meanIn),
	mMeanOut(meanOut),
	mPreIn(preIn),
	mPreInInv(preIn.inverse()),
	mPreOut(preOut),
	mPreOutInv(preOut.inverse()),
	mPredictor(predictor),
	mLogJacobian(mPreOut.partialPivLu().matrixLU().diagonal().array().abs().log().sum()),
	mGradTransform(mPreOut * mPredictor * mPreIn)
{
}



CMT::AffinePreconditioner::AffinePreconditioner(
	const VectorXd& meanIn,
	const VectorXd& meanOut,
	const MatrixXd& preIn,
	const MatrixXd& preInInv,
	const MatrixXd& preOut,
	const MatrixXd& preOutInv,
	const MatrixXd& predictor) :
	mMeanIn(meanIn),
	mMeanOut(meanOut),
	mPreIn(preIn),
	mPreInInv(preInInv),
	mPreOut(preOut),
	mPreOutInv(preOutInv),
	mPredictor(predictor),
	mLogJacobian(mPreOut.partialPivLu().matrixLU().diagonal().array().abs().log().sum()),
	mGradTransform(mPreOut * mPredictor * mPreIn)
{
}



CMT::AffinePreconditioner::AffinePreconditioner() {
}



int CMT::AffinePreconditioner::dimIn() const {
	return mMeanIn.size();
}



int CMT::AffinePreconditioner::dimInPre() const {
	return mPreIn.rows();
}



int CMT::AffinePreconditioner::dimOut() const {
	return mMeanOut.size();
}



int CMT::AffinePreconditioner::dimOutPre() const {
	return mPreOut.rows();
}



pair<ArrayXXd, ArrayXXd> CMT::AffinePreconditioner::operator()(
	const ArrayXXd& input,
	const ArrayXXd& output) const
{
	if(input.cols() != output.cols())
		throw Exception("Number of inputs and outputs must be the same."); 
	if(input.rows() != dimIn())
		throw Exception("Input has wrong dimensionality."); 
	if(output.rows() != dimOut())
		throw Exception("Output has wrong dimensionality."); 
	ArrayXXd inputTr = mPreIn * (input.matrix().colwise() - mMeanIn);
	ArrayXXd outputTr = mPreOut * (output.matrix().colwise() - mMeanOut - mPredictor * inputTr.matrix());
	return make_pair(inputTr, outputTr);
}



pair<ArrayXXd, ArrayXXd> CMT::AffinePreconditioner::inverse(
	const ArrayXXd& input,
	const ArrayXXd& output) const
{
	if(input.cols() != output.cols())
		throw Exception("Number of inputs and outputs must be the same."); 
	if(input.rows() != dimInPre())
		throw Exception("Input has wrong dimensionality."); 
	if(output.rows() != dimOutPre())
		throw Exception("Output has wrong dimensionality."); 
	ArrayXXd outputTr = (mPreOutInv * output.matrix() + mPredictor * input.matrix()).colwise() + mMeanOut;
	ArrayXXd inputTr = (mPreInInv * input.matrix()).colwise() + mMeanIn;
	return make_pair(inputTr, outputTr);
}



ArrayXXd CMT::AffinePreconditioner::operator()(const ArrayXXd& input) const {
	if(input.rows() != dimIn())
		throw Exception("Input has wrong dimensionality."); 
	return mPreIn * (input.matrix().colwise() - mMeanIn);
}



ArrayXXd CMT::AffinePreconditioner::inverse(const ArrayXXd& input) const {
	if(input.rows() != dimInPre())
		throw Exception("Input has wrong dimensionality."); 
	return (mPreInInv * input.matrix()).colwise() + mMeanIn;
}



Array<double, 1, Dynamic> CMT::AffinePreconditioner::logJacobian(const ArrayXXd& input, const ArrayXXd& output) const {
	return Array<double, 1, Dynamic>::Zero(output.cols()) + mLogJacobian;
}



pair<ArrayXXd, ArrayXXd> CMT::AffinePreconditioner::adjustGradient(
	const ArrayXXd& inputGradient,
	const ArrayXXd& outputGradient) const
{
	return make_pair(
		mPreIn.transpose() * inputGradient.matrix() - mGradTransform.transpose() * outputGradient.matrix(),
		mPreOut.transpose() * outputGradient.matrix());
}
