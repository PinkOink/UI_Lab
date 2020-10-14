#include "../include/IVector.h"
#include <cmath>
#include <iostream>

namespace {
    class VectorImpl: public IVector {
    public:
        VectorImpl(size_t dim, double* vals);
        IVector* clone() const override;
        double getCoord(size_t index) const override;
        RESULT_CODE setCoord(size_t index, double value) override;
        double norm(NORM norm) const override;
        size_t getDim() const override;
        ~VectorImpl() override;

    private:
        size_t _dim;
        double *_vals;
    };

    VectorImpl::VectorImpl(size_t dim, double* vals)
        : _dim(dim), _vals(vals)
    {}
    IVector * VectorImpl::clone() const {
        size_t _size = _dim * sizeof(double) + sizeof(VectorImpl);
        void *_data = new (std::nothrow) char[_size];
        if (_data == nullptr) {
            return nullptr;
        } else {
            VectorImpl *vec = new(_data) VectorImpl(this->_dim, (double *) ((char *) _data + sizeof(VectorImpl)));

            for (size_t i = 0; i < this->_dim; ++i) {
                vec->_vals[i] = this->_vals[i];
            }

            return vec;
        }
    }
    double VectorImpl::getCoord(size_t index) const {
        if (index < _dim) {
            return _vals[index];
        } else {
            return NAN;
        }
    }
    RESULT_CODE VectorImpl::setCoord(size_t index, double value) {
        if (!std::isnan(value)) {
            if (index < _dim) {
                _vals[index] = value;
                return RESULT_CODE::SUCCESS;
            } else {
                return RESULT_CODE::OUT_OF_BOUNDS;
            }
        } else {
            return RESULT_CODE::NAN_VALUE;
        }
    }
    double VectorImpl::norm(NORM norm) const {
        double normVal = 0;
        switch (norm) {
            case NORM::NORM_1:
                for (size_t i = 0; i < _dim; ++i) {
                    normVal += std::fabs(_vals[i]);
                }
                break;
            case NORM::NORM_2:
                for (size_t i = 0; i < _dim; ++i) {
                    normVal += _vals[i] * _vals[i];
                }
                normVal = sqrt(normVal);
                break;
            case NORM::NORM_INF:
                size_t max = 0;
                for (size_t i = 1; i < _dim; ++i) {
                    if (std::fabs(_vals[i]) > std::fabs(_vals[i])) {
                        max = i;
                    }
                }
                normVal = std::fabs(_vals[max]);
                break;
        }
        return normVal;
    }
    size_t VectorImpl::getDim() const {
        return _dim;
    }
    VectorImpl::~VectorImpl()
    {}
}

IVector* IVector::createVector(size_t dim, double* pData, ILogger* pLogger) {
    if (pLogger == nullptr) {
        return nullptr;
    } else if (pData == nullptr) {
        pLogger->log("No double-array for creating Vector\n", RESULT_CODE::BAD_REFERENCE);
        return nullptr;
    } else if (dim == 0) {
        pLogger->log("Zero dimension Vector can't be created\n", RESULT_CODE::OUT_OF_BOUNDS);
        return nullptr;
    } else {
        for (size_t i = 0; i < dim; ++i) {
            if (std::isnan(pData[i])) {
                pLogger->log("Nan-value in double-array\n", RESULT_CODE::NAN_VALUE);
                return nullptr;
            }
        }

        size_t _size = dim * sizeof(double) + sizeof(VectorImpl);
        void *_data = new(std::nothrow) char[_size];
        if (_data == nullptr) {
            pLogger->log("No memory for Vector\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        } else {
            IVector *vec = new(_data) VectorImpl(dim, (double *) ((char *) _data + sizeof(VectorImpl)));

            for (size_t i = 0; i < dim; ++i) {
                vec->setCoord(i, pData[i]);
            }

            return vec;
        }
    }
}
IVector* IVector::add(IVector const* pOperand1, IVector const* pOperand2, ILogger* pLogger) {
    if (pLogger == nullptr) {
        return nullptr;
    } else if (pOperand1 == nullptr || pOperand2 == nullptr) {
        pLogger->log("One of the operands is null\n", RESULT_CODE::BAD_REFERENCE);
        return nullptr;
    } else if (pOperand1->getDim() != pOperand2->getDim()) {
        pLogger->log("Vectors of different dimensions\n", RESULT_CODE::WRONG_DIM);
        return nullptr;
    } else {
        IVector *res = pOperand1->clone();
        if (res == nullptr) {
            pLogger->log("No memory for result\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        } else {
            size_t dim = pOperand1->getDim();
            for (size_t i = 0; i < dim; ++i) {
                res->setCoord(i, res->getCoord(i) + pOperand2->getCoord(i));
            }
            return res;
        }
    }
}
IVector* IVector::sub(IVector const* pOperand1, IVector const* pOperand2, ILogger* pLogger) {
    if (pLogger == nullptr) {
        return nullptr;
    } else if (pOperand1 == nullptr || pOperand2 == nullptr) {
        pLogger->log("One of the operands is null\n", RESULT_CODE::BAD_REFERENCE);
        return nullptr;
    } else if (pOperand1->getDim() != pOperand2->getDim()) {
        pLogger->log("Vectors of different dimensions\n", RESULT_CODE::WRONG_DIM);
        return nullptr;
    } else {
        IVector *res = pOperand1->clone();
        if (res == nullptr) {
            pLogger->log("No memory for result\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        } else {
            size_t dim = pOperand1->getDim();
            for (size_t i = 0; i < dim; ++i) {
                res->setCoord(i, res->getCoord(i) - pOperand2->getCoord(i));
            }
            return res;
        }
    }
}
IVector* IVector::mul(IVector const* pOperand1, double scaleParam, ILogger* pLogger) {
    if (pLogger == nullptr) {
        return nullptr;
    } else if (pOperand1 == nullptr) {
        pLogger->log("Operand is null\n", RESULT_CODE::BAD_REFERENCE);
        return nullptr;
    } else if (std::isnan(scaleParam)) {
        pLogger->log("Scale Param is NaN\n", RESULT_CODE::NAN_VALUE);
        return nullptr;
    } else {
        IVector *res = pOperand1->clone();
        if (res == nullptr) {
            pLogger->log("No memory for result\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        } else {
            size_t dim = pOperand1->getDim();
            for (size_t i = 0; i < dim; ++i) {
                res->setCoord(i, res->getCoord(i) * scaleParam);
            }
            return res;
        }
    }
}
double IVector::mul(IVector const* pOperand1, IVector const* pOperand2, ILogger* pLogger) {
    if (pLogger == nullptr) {
        return NAN;
    } else if (pOperand1 == nullptr || pOperand2 == nullptr) {
        pLogger->log("One of the operands is null\n", RESULT_CODE::BAD_REFERENCE);
        return NAN;
    } else if (pOperand1->getDim() != pOperand2->getDim()) {
        pLogger->log("Vectors of different dimensions\n", RESULT_CODE::WRONG_DIM);
        return NAN;
    } else {
        double res = 0;
        size_t dim = pOperand1->getDim();
        for (size_t i = 0; i < dim; ++i) {
            res += pOperand1->getCoord(i) * pOperand2->getCoord(i);
        }
        return res;
    }
}
RESULT_CODE IVector::equals(IVector const* pOperand1, IVector const* pOperand2, NORM norm, double tolerance, bool* result, ILogger* pLogger) {
    if (pLogger == nullptr) {
        return RESULT_CODE::BAD_REFERENCE;
    } else if (pOperand1 == nullptr || pOperand2 == nullptr || result == nullptr) {
        pLogger->log("One of the operands is null\n", RESULT_CODE::BAD_REFERENCE);
        return RESULT_CODE::BAD_REFERENCE;
    } else if (pOperand1->getDim() != pOperand2->getDim()) {
        pLogger->log("Vectors of different dimensions\n", RESULT_CODE::WRONG_DIM);
        return RESULT_CODE::WRONG_DIM;
    } else if (std::isnan(tolerance)) {
        pLogger->log("Tolerance param is NaN\n", RESULT_CODE::NAN_VALUE);
        return RESULT_CODE::NAN_VALUE;
    } else if (tolerance < 0) {
        pLogger->log("Tolerance is less than zero\n", RESULT_CODE::WRONG_ARGUMENT);
        return RESULT_CODE::WRONG_ARGUMENT;
    } else {
        IVector *buf = IVector::sub(pOperand1, pOperand2, pLogger);
        if (buf == nullptr) {
            // error will be logged in sub
            return RESULT_CODE::OUT_OF_MEMORY;
        } else {
            double sub_norm = buf->norm(norm);
            delete buf;
            *result = (sub_norm <= tolerance);

            return RESULT_CODE::SUCCESS;
        }
    }
}
IVector::~IVector() {}
