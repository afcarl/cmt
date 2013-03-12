#include "exception.h"
#include "preconditionerinterface.h"

#include <iostream>

PyObject* Preconditioner_call(PreconditionerObject* self, PyObject* args, PyObject* kwds) {
	const char* kwlist[] = {"input", "output", 0};

	PyObject* input;
	PyObject* output = 0;

	if(PyArg_ParseTupleAndKeywords(args, kwds, "O|O", const_cast<char**>(kwlist), &input, &output))
		if(output && output != Py_None) {
			input = PyArray_FROM_OTF(input, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
			output = PyArray_FROM_OTF(output, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);

			if(!input || !output) {
				Py_XDECREF(input);
				Py_XDECREF(output);
				PyErr_SetString(PyExc_TypeError, "Input and output should be of type `ndarray`.");
				return 0;
			}

			try {
				pair<ArrayXXd, ArrayXXd> data = self->preconditioner->operator()(
					PyArray_ToMatrixXd(input),
					PyArray_ToMatrixXd(output));

				PyObject* inputObj = PyArray_FromMatrixXd(data.first);
				PyObject* outputObj = PyArray_FromMatrixXd(data.second);

				PyObject* tuple = Py_BuildValue("(OO)", inputObj, outputObj);

				Py_DECREF(input);
				Py_DECREF(output);
				Py_DECREF(inputObj);
				Py_DECREF(outputObj);

				return tuple;

			} catch(Exception exception) {
				Py_DECREF(input);
				Py_DECREF(output);
				PyErr_SetString(PyExc_RuntimeError, exception.message());
				return 0;
			}
		} else {
			input = PyArray_FROM_OTF(input, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);

			if(!input) {
				Py_XDECREF(input);
				PyErr_SetString(PyExc_TypeError, "Input should be of type `ndarray`.");
				return 0;
			}

			try {
				PyObject* inputObj = PyArray_FromMatrixXd(
					self->preconditioner->operator()(PyArray_ToMatrixXd(input)));
				Py_DECREF(input);
				return inputObj;
			} catch(Exception exception) {
				Py_DECREF(input);
				PyErr_SetString(PyExc_RuntimeError, exception.message());
				return 0;
			}
		}

	return 0;
}


const char* Preconditioner_inverse_doc =
	"inverse(self, input, output=None)\n"
	"\n"
	"Computes original inputs and outputs from transformed inputs and outputs."
	"\n"
	"@type  input: ndarray\n"
	"@param input: preconditioned inputs stored in columns\n"
	"\n"
	"@type  output: ndarray\n"
	"@param output: preconditioned outputs stored in columns\n"
	"\n"
	"@rtype: tuple/ndarray\n"
	"@return: tuple or array containing inputs or inputs and outputs, respectively";

PyObject* Preconditioner_inverse(PreconditionerObject* self, PyObject* args, PyObject* kwds) {
	const char* kwlist[] = {"input", "output", 0};

	PyObject* input;
	PyObject* output = 0;

	if(PyArg_ParseTupleAndKeywords(args, kwds, "O|O", const_cast<char**>(kwlist), &input, &output))
		if(output && output != Py_None) {
			input = PyArray_FROM_OTF(input, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
			output = PyArray_FROM_OTF(output, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);

			if(!input || !output) {
				Py_XDECREF(input);
				Py_XDECREF(output);
				PyErr_SetString(PyExc_TypeError, "Input and output should be of type `ndarray`.");
			}

			try {
				pair<ArrayXXd, ArrayXXd> data = self->preconditioner->inverse(
					PyArray_ToMatrixXd(input),
					PyArray_ToMatrixXd(output));

				PyObject* inputObj = PyArray_FromMatrixXd(data.first);
				PyObject* outputObj = PyArray_FromMatrixXd(data.second);

				PyObject* tuple = Py_BuildValue("(OO)", inputObj, outputObj);

				Py_DECREF(input);
				Py_DECREF(output);
				Py_DECREF(inputObj);
				Py_DECREF(outputObj);

				return tuple;

			} catch(Exception exception) {
				Py_DECREF(input);
				Py_DECREF(output);
				PyErr_SetString(PyExc_RuntimeError, exception.message());
				return 0;
			}
		} else {
			input = PyArray_FROM_OTF(input, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);

			if(!input) {
				Py_XDECREF(input);
				PyErr_SetString(PyExc_TypeError, "Input should be of type `ndarray`.");
			}

			try {
				PyObject* inputObj = PyArray_FromMatrixXd(
					self->preconditioner->inverse(PyArray_ToMatrixXd(input)));
				Py_DECREF(input);
				return inputObj;

			} catch(Exception exception) {
				Py_DECREF(input);
				PyErr_SetString(PyExc_RuntimeError, exception.message());
				return 0;
			}
		}
	return 0;
}



const char* Preconditioner_logjacobian_doc =
	"loglikelihood(self, input, output)\n"
	"\n"
	"Computes the conditional log-Jacobian determinant for the given data points "
	"(using the natural logarithm).\n"
	"\n"
	"@type  input: ndarray\n"
	"@param input: inputs stored in columns\n"
	"\n"
	"@type  output: ndarray\n"
	"@param output: outputs stored in columns\n"
	"\n"
	"@rtype: ndarray\n"
	"@return: log-Jacobian of the transformation for each data point";

PyObject* Preconditioner_logjacobian(PreconditionerObject* self, PyObject* args, PyObject* kwds) {
	const char* kwlist[] = {"input", "output", 0};

	PyObject* input;
	PyObject* output;

	// read arguments
	if(!PyArg_ParseTupleAndKeywords(args, kwds, "OO", const_cast<char**>(kwlist), &input, &output))
		return 0;

	// make sure data is stored in NumPy array
	input = PyArray_FROM_OTF(input, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
	output = PyArray_FROM_OTF(output, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);

	if(!input || !output) {
		PyErr_SetString(PyExc_TypeError, "Data has to be stored in NumPy arrays.");
		return 0;
	}

	try {
		PyObject* result = PyArray_FromMatrixXd(
			self->preconditioner->logJacobian(PyArray_ToMatrixXd(input), PyArray_ToMatrixXd(output)));
		Py_DECREF(input);
		Py_DECREF(output);
		return result;
	} catch(Exception exception) {
		Py_DECREF(input);
		Py_DECREF(output);
		PyErr_SetString(PyExc_RuntimeError, exception.message());
		return 0;
	}

	return 0;
}



PyObject* Preconditioner_new(PyTypeObject* type, PyObject*, PyObject*) {
	PyObject* self = type->tp_alloc(type, 0);

	if(self)
		reinterpret_cast<PreconditionerObject*>(self)->preconditioner = 0;

	return self;
}



const char* Preconditioner_doc =
	"Abstract base class for preconditioners of inputs and outputs.\n";

int Preconditioner_init(WhiteningPreconditionerObject* self, PyObject* args, PyObject* kwds) {
	PyErr_SetString(PyExc_NotImplementedError, "This is an abstract class.");
	return -1;
}



void Preconditioner_dealloc(PreconditionerObject* self) {
	// delete actual instance
	delete self->preconditioner;

	// delete Python object
	self->ob_type->tp_free(reinterpret_cast<PyObject*>(self));
}



PyObject* Preconditioner_dim_in(PreconditionerObject* self, PyObject*, void*) {
	return PyInt_FromLong(self->preconditioner->dimIn());
}



PyObject* Preconditioner_dim_out(PreconditionerObject* self, PyObject*, void*) {
	return PyInt_FromLong(self->preconditioner->dimOut());
}



const char* WhiteningPreconditioner_doc =
	"Decorrelates inputs and outputs.\n"
	"\n"
	"@type  input: ndarray\n"
	"@param input: inputs stored in columns\n"
	"\n"
	"@type  output: ndarray\n"
	"@param output: outputs stored in columns";

int WhiteningPreconditioner_init(WhiteningPreconditionerObject* self, PyObject* args, PyObject* kwds) {
	PyObject* meanIn;
	PyObject* meanOut;
	PyObject* whiteIn;
	PyObject* whiteInInv;
	PyObject* whiteOut;
	PyObject* whiteOutInv;
	PyObject* predictor;

	// test if this call to __init__ is the result of unpickling
	if(PyArg_ParseTuple(args, "OOOOOOO", &meanIn, &meanOut, &whiteIn, &whiteInInv, &whiteOut, &whiteOutInv, &predictor)) {
		meanIn = PyArray_FROM_OTF(meanIn, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		meanOut = PyArray_FROM_OTF(meanOut, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		whiteIn = PyArray_FROM_OTF(whiteIn, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		whiteInInv = PyArray_FROM_OTF(whiteInInv, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		whiteOut = PyArray_FROM_OTF(whiteOut, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		whiteOutInv = PyArray_FROM_OTF(whiteOutInv, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		predictor = PyArray_FROM_OTF(predictor, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);

		if(!meanIn || !meanOut || !whiteIn || !whiteInInv || !whiteOut || !whiteOutInv || !predictor) {
			Py_XDECREF(meanIn);
			Py_XDECREF(meanOut);
			Py_XDECREF(whiteIn);
			Py_XDECREF(whiteInInv);
			Py_XDECREF(whiteOut);
			Py_XDECREF(whiteOutInv);
			Py_XDECREF(predictor);
			PyErr_SetString(PyExc_TypeError, "Parameters of preconditioner should be of type `ndarray`.");
			return -1;
		}

		try {
			self->preconditioner = new WhiteningPreconditioner(
				PyArray_ToMatrixXd(meanIn),
				PyArray_ToMatrixXd(meanOut),
				PyArray_ToMatrixXd(whiteIn),
				PyArray_ToMatrixXd(whiteInInv),
				PyArray_ToMatrixXd(whiteOut),
				PyArray_ToMatrixXd(whiteOutInv),
				PyArray_ToMatrixXd(predictor));
		} catch(Exception exception) {
			PyErr_SetString(PyExc_RuntimeError, exception.message());
			Py_DECREF(meanIn);
			Py_DECREF(meanOut);
			Py_DECREF(whiteIn);
			Py_DECREF(whiteInInv);
			Py_DECREF(whiteOut);
			Py_DECREF(whiteOutInv);
			Py_DECREF(predictor);
			return -1;
		}

		Py_DECREF(meanIn);
		Py_DECREF(meanOut);
		Py_DECREF(whiteIn);
		Py_DECREF(whiteInInv);
		Py_DECREF(whiteOut);
		Py_DECREF(whiteOutInv);
		Py_DECREF(predictor);
	} else {
		PyErr_Clear();

		const char* kwlist[] = {"input", "output", 0};

		PyObject* input;
		PyObject* output;


		if(!PyArg_ParseTupleAndKeywords(args, kwds, "OO", const_cast<char**>(kwlist), &input, &output)) {
			return -1;
		}

		input = PyArray_FROM_OTF(input, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		output = PyArray_FROM_OTF(output, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);

		if(!input || !output) {
			Py_XDECREF(input);
			Py_XDECREF(output);
			PyErr_SetString(PyExc_TypeError, "Input and output should be of type `ndarray`.");
			return -1;
		}

		try {
			self->preconditioner = new WhiteningPreconditioner(
				PyArray_ToMatrixXd(input),
				PyArray_ToMatrixXd(output));
		} catch(Exception exception) {
			Py_DECREF(input);
			Py_DECREF(output);
			PyErr_SetString(PyExc_RuntimeError, exception.message());
			return -1;
		}

		Py_DECREF(input);
		Py_DECREF(output);

	}

	return 0;
}



PyObject* WhiteningPreconditioner_mean_in(WhiteningPreconditionerObject* self, PyObject*, void*) {
	return PyArray_FromMatrixXd(self->preconditioner->meanIn());
}



PyObject* WhiteningPreconditioner_mean_out(WhiteningPreconditionerObject* self, PyObject*, void*) {
	return PyArray_FromMatrixXd(self->preconditioner->meanOut());
}



PyObject* WhiteningPreconditioner_reduce(WhiteningPreconditionerObject* self, PyObject*, PyObject*) {
	PyObject* meanIn = PyArray_FromMatrixXd(self->preconditioner->meanIn());
	PyObject* meanOut = PyArray_FromMatrixXd(self->preconditioner->meanOut());
	PyObject* whiteIn = PyArray_FromMatrixXd(self->preconditioner->whiteIn());
	PyObject* whiteInInv = PyArray_FromMatrixXd(self->preconditioner->whiteInInv());
	PyObject* whiteOut = PyArray_FromMatrixXd(self->preconditioner->whiteOut());
	PyObject* whiteOutInv = PyArray_FromMatrixXd(self->preconditioner->whiteOutInv());
	PyObject* predictor = PyArray_FromMatrixXd(self->preconditioner->predictor());

	PyObject* args = Py_BuildValue("(OOOOOOO)", 
		meanIn,
		meanOut,
		whiteIn,
		whiteInInv,
		whiteOut,
		whiteOutInv,
		predictor);
	PyObject* state = Py_BuildValue("()");
	PyObject* result = Py_BuildValue("(OOO)", self->ob_type, args, state);

	Py_DECREF(meanIn);
	Py_DECREF(meanOut);
	Py_DECREF(whiteIn);
	Py_DECREF(whiteInInv);
	Py_DECREF(whiteOut);
	Py_DECREF(whiteOutInv);
	Py_DECREF(predictor);
	Py_DECREF(args);
	Py_DECREF(state);

	return result;
}



PyObject* WhiteningPreconditioner_setstate(WhiteningPreconditionerObject* self, PyObject* state, PyObject*) {
	// WhiteningPreconditioner_init handles everything
	Py_INCREF(Py_None);
	return Py_None;
}



const char* PCAPreconditioner_doc =
	"This preconditioner can be used to reduce the dimensionality of the input.\n"
	"\n"
	"Additionally, the data is decorrelated as by L{WhiteningPreconditioner}.\n"
	"\n"
	"To create a preconditioner which retains (at least) 98.5\% of the input variance, use:\n"
	"\n"
	"\t>>> pca = PCAPreconditioner(input, output, var_explained=98.5)\n"
	"\n"
	"To create a preconditioner which reduces the dimensionality of the input to 10, use:\n"
	"\n"
	"\t>>> pca = PCAPreconditioner(input, output, num_pcs=10)\n"
	"\n"
	"If both arguments are specified, C{var_explained} will be ignored."
	"Afterwards, apply the preconditioner to the data.\n"
	"\n"
	"\t>>> input, output = preconditioner(input, output)\n"
	"\n"
	"To (approximately) reconstruct the data, you can do the following.\n"
	"\n"
	"\t>>> input, output = preconditioner.inverse(input, output)\n"
	"\n"
	"@type  input: ndarray\n"
	"@param input: inputs stored in columns\n"
	"\n"
	"@type  output: ndarray\n"
	"@param output: outputs stored in columns\n"
	"\n"
	"@type  var_explained: double\n"
	"@param var_explained: the amount of variance retained after dimensionality reduction\n"
	"\n"
	"@type  num_pcs: integer\n"
	"@param num_pcs: the number of principal components of the input kept";

int PCAPreconditioner_init(PCAPreconditionerObject* self, PyObject* args, PyObject* kwds) {
	PyObject* eigenvalues;
	PyObject* meanIn;
	PyObject* meanOut;
	PyObject* whiteIn;
	PyObject* whiteInInv;
	PyObject* whiteOut;
	PyObject* whiteOutInv;
	PyObject* predictor;

	// test if this call to __init__ is the result of unpickling
	if(PyArg_ParseTuple(args, "OOOOOOOO",
		&eigenvalues, &meanIn, &meanOut, &whiteIn,
		&whiteInInv, &whiteOut, &whiteOutInv, &predictor))
	{
		eigenvalues = PyArray_FROM_OTF(eigenvalues, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		meanIn = PyArray_FROM_OTF(meanIn, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		meanOut = PyArray_FROM_OTF(meanOut, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		whiteIn = PyArray_FROM_OTF(whiteIn, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		whiteInInv = PyArray_FROM_OTF(whiteInInv, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		whiteOut = PyArray_FROM_OTF(whiteOut, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		whiteOutInv = PyArray_FROM_OTF(whiteOutInv, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		predictor = PyArray_FROM_OTF(predictor, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);

		if(!eigenvalues || !meanIn || !meanOut || !whiteIn || !whiteInInv || !whiteOut || !whiteOutInv || !predictor) {
			Py_XDECREF(eigenvalues);
			Py_XDECREF(meanIn);
			Py_XDECREF(meanOut);
			Py_XDECREF(whiteIn);
			Py_XDECREF(whiteInInv);
			Py_XDECREF(whiteOut);
			Py_XDECREF(whiteOutInv);
			Py_XDECREF(predictor);
			PyErr_SetString(PyExc_TypeError, "Parameters of preconditioner should be of type `ndarray`.");
			return -1;
		}

		try {
			self->preconditioner = new PCAPreconditioner(
				PyArray_ToMatrixXd(eigenvalues),
				PyArray_ToMatrixXd(meanIn),
				PyArray_ToMatrixXd(meanOut),
				PyArray_ToMatrixXd(whiteIn),
				PyArray_ToMatrixXd(whiteInInv),
				PyArray_ToMatrixXd(whiteOut),
				PyArray_ToMatrixXd(whiteOutInv),
				PyArray_ToMatrixXd(predictor));
		} catch(Exception exception) {
			PyErr_SetString(PyExc_RuntimeError, exception.message());
			Py_DECREF(eigenvalues);
			Py_DECREF(meanIn);
			Py_DECREF(meanOut);
			Py_DECREF(whiteIn);
			Py_DECREF(whiteInInv);
			Py_DECREF(whiteOut);
			Py_DECREF(whiteOutInv);
			Py_DECREF(predictor);
			return -1;
		}

		Py_DECREF(eigenvalues);
		Py_DECREF(meanIn);
		Py_DECREF(meanOut);
		Py_DECREF(whiteIn);
		Py_DECREF(whiteInInv);
		Py_DECREF(whiteOut);
		Py_DECREF(whiteOutInv);
		Py_DECREF(predictor);
	} else {
		PyErr_Clear();

		const char* kwlist[] = {"input", "output", "var_explained", "num_pcs", 0};

		PyObject* input;
		PyObject* output;
		double var_explained = 99.;
		int num_pcs = -1;

		if(!PyArg_ParseTupleAndKeywords(args, kwds, "OO|di", const_cast<char**>(kwlist),
			&input, &output, &var_explained, &num_pcs))
		{
			return -1;
		}

		input = PyArray_FROM_OTF(input, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);
		output = PyArray_FROM_OTF(output, NPY_DOUBLE, NPY_F_CONTIGUOUS | NPY_ALIGNED);

		if(!input || !output) {
			Py_XDECREF(input);
			Py_XDECREF(output);
			PyErr_SetString(PyExc_TypeError, "Input and output should be of type `ndarray`.");
			return -1;
		}

		try {
			self->preconditioner = new PCAPreconditioner(
				PyArray_ToMatrixXd(input),
				PyArray_ToMatrixXd(output),
				var_explained,
				num_pcs);
		} catch(Exception exception) {
			Py_DECREF(input);
			Py_DECREF(output);
			PyErr_SetString(PyExc_RuntimeError, exception.message());
			return -1;
		}

		Py_DECREF(input);
		Py_DECREF(output);

	}

	return 0;
}



PyObject* PCAPreconditioner_reduce(PCAPreconditionerObject* self, PyObject*, PyObject*) {
	PyObject* eigenvalues = PyArray_FromMatrixXd(self->preconditioner->eigenvalues());
	PyObject* meanIn = PyArray_FromMatrixXd(self->preconditioner->meanIn());
	PyObject* meanOut = PyArray_FromMatrixXd(self->preconditioner->meanOut());
	PyObject* whiteIn = PyArray_FromMatrixXd(self->preconditioner->whiteIn());
	PyObject* whiteInInv = PyArray_FromMatrixXd(self->preconditioner->whiteInInv());
	PyObject* whiteOut = PyArray_FromMatrixXd(self->preconditioner->whiteOut());
	PyObject* whiteOutInv = PyArray_FromMatrixXd(self->preconditioner->whiteOutInv());
	PyObject* predictor = PyArray_FromMatrixXd(self->preconditioner->predictor());

	PyObject* args = Py_BuildValue("(OOOOOOOO)",
		eigenvalues,
		meanIn,
		meanOut,
		whiteIn,
		whiteInInv,
		whiteOut,
		whiteOutInv,
		predictor);
	PyObject* state = Py_BuildValue("()");
	PyObject* result = Py_BuildValue("(OOO)", self->ob_type, args, state);

	Py_DECREF(eigenvalues);
	Py_DECREF(meanIn);
	Py_DECREF(meanOut);
	Py_DECREF(whiteIn);
	Py_DECREF(whiteInInv);
	Py_DECREF(whiteOut);
	Py_DECREF(whiteOutInv);
	Py_DECREF(predictor);
	Py_DECREF(args);
	Py_DECREF(state);

	return result;
}



PyObject* PCAPreconditioner_setstate(PCAPreconditionerObject* self, PyObject* state, PyObject*) {
	// PCAPreconditioner_init handles everything
	Py_INCREF(Py_None);
	return Py_None;
}
