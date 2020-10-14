#include "../include/ISet.h"
#include <vector>
#include <cmath>

namespace {
    class SetImpl : public ISet {
    public:
        explicit SetImpl(ILogger* pLogger);
        RESULT_CODE insert(const IVector *pVector, IVector::NORM norm, double tolerance) override;
        RESULT_CODE get(IVector *&pVector, size_t index) const override;
        RESULT_CODE get(IVector *&pVector, IVector const *pSample, IVector::NORM norm, double tolerance) const override;
        size_t getDim() const override; //space dimension
        size_t getSize() const override; //num elements in set
        void clear() override; // delete all
        RESULT_CODE erase(size_t index) override;
        RESULT_CODE erase(IVector const *pSample, IVector::NORM norm, double tolerance) override;
        ISet *clone() const override;
        ~SetImpl() override;

    private:
        std::vector<const IVector *> _vals;
        size_t _vec_dim;
        ILogger *_log;
    };

    SetImpl::SetImpl(ILogger* pLogger)
    : _vals(), _vec_dim(0), _log(pLogger)
    {}
    RESULT_CODE SetImpl::insert(const IVector *pVector, IVector::NORM norm, double tolerance) {
        if (pVector == nullptr) {
            _log->log("Vector pointer is null\n", RESULT_CODE::BAD_REFERENCE);
            return RESULT_CODE::BAD_REFERENCE;
        } else if (std::isnan(tolerance)) {
            _log->log("Tolerance is NaN\n", RESULT_CODE::NAN_VALUE);
            return RESULT_CODE::NAN_VALUE;
        } else if (tolerance < 0) {
            _log->log("Tolerance is less than zero\n", RESULT_CODE::WRONG_ARGUMENT);
            return RESULT_CODE::WRONG_ARGUMENT;
        } else if (_vec_dim != 0 && pVector->getDim() != _vec_dim) {
            _log->log("Vector has wrong dimension\n", RESULT_CODE::WRONG_DIM);
            return RESULT_CODE::WRONG_DIM;
        } else {
            IVector *buf = nullptr;
            this->get(buf, pVector, norm, tolerance);
            if (buf == nullptr) {
                _vals.push_back(pVector->clone());
                //_vals.push_back(pVector);
                if (_vec_dim == 0) {
                    _vec_dim = pVector->getDim();
                }
                return RESULT_CODE::SUCCESS;
            } else {
                _log->log("Vector is already in Set\n", RESULT_CODE::MULTIPLE_DEFINITION);
                return RESULT_CODE::MULTIPLE_DEFINITION;
            }
        }
    }
    RESULT_CODE SetImpl::get(IVector *&pVector, size_t index) const {
        if (index > _vals.size()) {
            pVector = nullptr;
            _log->log("Index is out of bounds\n", RESULT_CODE::OUT_OF_BOUNDS);
            return RESULT_CODE::OUT_OF_BOUNDS;
        } else {
            //pVector = _vals[index]->clone();
            pVector = const_cast<IVector*>(_vals[index]);
            return RESULT_CODE::SUCCESS;
        }
    }
    RESULT_CODE SetImpl::get(IVector *&pVector, IVector const *pSample, IVector::NORM norm, double tolerance) const {
        if (pSample == nullptr) {
            pVector = nullptr;
            _log->log("Sample pointer is null\n", RESULT_CODE::BAD_REFERENCE);
            return RESULT_CODE::BAD_REFERENCE;
        } else if (std::isnan(tolerance)) {
            pVector = nullptr;
            _log->log("Tolerance is NaN\n", RESULT_CODE::NAN_VALUE);
            return RESULT_CODE::NAN_VALUE;
        } else if (tolerance < 0) {
            pVector = nullptr;
            _log->log("Tolerance is less than zero\n", RESULT_CODE::WRONG_ARGUMENT);
            return RESULT_CODE::WRONG_ARGUMENT;
        } else if (_vec_dim != 0 && pSample->getDim() != _vec_dim) {
            pVector = nullptr;
            _log->log("Sample has wrong dimension\n", RESULT_CODE::WRONG_DIM);
            return RESULT_CODE::WRONG_DIM;
        } else {
            size_t size = _vals.size();
            bool equals = false;
            RESULT_CODE code;
            for (size_t i = 0; i < size; ++i) {
                code = IVector::equals(_vals[i], pSample, norm, tolerance, &equals, _log);
                if (code == RESULT_CODE::SUCCESS && equals) {
                    pVector = _vals[i]->clone();
                    //pVector = const_cast<IVector*>(_vals[i]);
                    return RESULT_CODE::SUCCESS;
                }
            }
            pVector = nullptr;
            return RESULT_CODE::SUCCESS;
        }
    }
    size_t SetImpl::getDim() const {
        return _vec_dim;
    }
    size_t SetImpl::getSize() const {
        return _vals.size();
    }
    void SetImpl::clear() {
        size_t size = _vals.size();
        for (size_t i = 0; i < size; ++i) {
            delete _vals[i];
        }
        _vals.clear();
        _vec_dim = 0;
    }
    RESULT_CODE SetImpl::erase(size_t index) {
        if (index > _vals.size()) {
            _log->log("Index is out of bounds\n", RESULT_CODE::NAN_VALUE);
            return RESULT_CODE::OUT_OF_BOUNDS;
        } else {
            delete _vals[index];
            _vals.erase(_vals.begin() + index);
            if (_vals.size() == 0) {
                _vec_dim = 0;
            }
            return RESULT_CODE::SUCCESS;
        }
    }
    RESULT_CODE SetImpl::erase(IVector const *pSample, IVector::NORM norm, double tolerance) {
        if (pSample == nullptr) {
            _log->log("Sample pointer is null\n", RESULT_CODE::NAN_VALUE);
            return RESULT_CODE::BAD_REFERENCE;
        } else if (std::isnan(tolerance)) {
            _log->log("Tolerance is NaN\n", RESULT_CODE::NAN_VALUE);
            return RESULT_CODE::NAN_VALUE;
        } else if (tolerance < 0) {
            _log->log("Tolerance is less than zero\n", RESULT_CODE::NAN_VALUE);
            return RESULT_CODE::WRONG_ARGUMENT;
        } else if (pSample->getDim() != _vec_dim) {
            _log->log("Sample has wrong dimension\n", RESULT_CODE::NAN_VALUE);
            return RESULT_CODE::WRONG_DIM;
        } else {
            size_t size = _vals.size();
            bool equals = false;
            RESULT_CODE code;
            for (size_t i = 0; i < size; ++i) {
                code = IVector::equals(_vals[i], pSample, norm, tolerance, &equals, _log);
                if (code == RESULT_CODE::SUCCESS && equals) {
                    delete _vals[i];
                    _vals.erase(_vals.begin() + i);
                    return RESULT_CODE::SUCCESS;
                }
            }
            return RESULT_CODE::SUCCESS;
        }
    }
    ISet *SetImpl::clone() const {
        SetImpl* set = new (std::nothrow) SetImpl(this->_log);
        if (set == nullptr) {
            _log->log("No memory for Vector\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        } else {
            set->_vec_dim = this->_vec_dim;
            size_t size = this->_vals.size();
            IVector* buf;
            for (size_t i = 0; i < size; ++i) {
                buf = this->_vals[i]->clone();
                if (buf != nullptr) {
                    set->_vals.push_back(buf);
                } else {
                    set->clear();
                    delete set;
                    return nullptr;
                }
            }
            return set;
        }
    }
    SetImpl::~SetImpl() {
        size_t size = _vals.size();
        for (size_t i = 0; i < size; ++i) {
            delete _vals[i];
        }
    }
}

ISet* ISet::createSet(ILogger* pLogger) {
    if (pLogger == nullptr) {
        return nullptr;
    } else {
        ISet* set = new(std::nothrow) SetImpl(pLogger);
        if (set == nullptr) {
            pLogger->log("No memory for Vector\n", RESULT_CODE::OUT_OF_MEMORY);
        }
        return set;
    }
}
ISet* ISet::add(ISet const* pOperand1, ISet const* pOperand2, IVector::NORM norm, double tolerance, ILogger* pLogger) {
    if (pLogger == nullptr) {
        return nullptr;
    } else if (pOperand1 == nullptr || pOperand2 == nullptr) {
        pLogger->log("One of the operands is null\n", RESULT_CODE::BAD_REFERENCE);
        return nullptr;
    } else if (std::isnan(tolerance)) {
        pLogger->log("Tolerance is NaN\n", RESULT_CODE::NAN_VALUE);
        return nullptr;
    } else if (tolerance < 0) {
        pLogger->log("Tolerance is less than zero\n", RESULT_CODE::NAN_VALUE);
        return nullptr;
    } else if ((pOperand1->getDim() != pOperand2->getDim()) && (pOperand1->getDim() != 0 || pOperand2->getDim() != 0)){
        pLogger->log("Sets have different dimensions\n", RESULT_CODE::WRONG_DIM);
        return nullptr;
    } else {
        ISet* add = pOperand1->clone();
        if (add != nullptr) {
            size_t size_2 = pOperand2->getSize();
            IVector *buf_2 = nullptr;
            for (size_t i = 0; i < size_2; ++i) {
                pOperand2->get(buf_2, i);
                if (buf_2 != nullptr) {
                    add->insert(buf_2, norm, tolerance);
                }
            }
        }
        return add;
    }
}
ISet* ISet::intersect(ISet const* pOperand1, ISet const* pOperand2, IVector::NORM norm, double tolerance, ILogger* pLogger) {
    if (pLogger == nullptr) {
        return nullptr;
    } else if (pOperand1 == nullptr || pOperand2 == nullptr) {
        pLogger->log("One of the operands is null\n", RESULT_CODE::BAD_REFERENCE);
        return nullptr;
    } else if (std::isnan(tolerance)) {
        pLogger->log("Tolerance is NaN\n", RESULT_CODE::NAN_VALUE);
        return nullptr;
    } else if (tolerance < 0) {
        pLogger->log("Tolerance is less than zero\n", RESULT_CODE::NAN_VALUE);
        return nullptr;
    } else if ((pOperand1->getDim() != pOperand2->getDim()) && (pOperand1->getDim() != 0 || pOperand2->getDim() != 0)){
        pLogger->log("Sets have different dimensions\n", RESULT_CODE::WRONG_DIM);
        return nullptr;
    } else {
        ISet *inter = new (std::nothrow) SetImpl(pLogger);
        if (inter == nullptr) {
            pLogger->log("No memory for Vector\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        } else {
            size_t size_1 = pOperand1->getSize();
            IVector *buf_1 = nullptr;
            IVector *buf_2 = nullptr;
            for (size_t i = 0; i < size_1; ++i) {
                pOperand1->get(buf_1, i);
                pOperand2->get(buf_2, buf_1, norm, tolerance);
                if (buf_2 != nullptr) {
                    inter->insert(buf_1, norm, tolerance);
                }
            }
            return inter;
        }
    }
}
ISet* ISet::sub(ISet const* pOperand1, ISet const* pOperand2, IVector::NORM norm, double tolerance, ILogger* pLogger) {
    if (pLogger == nullptr) {
        return nullptr;
    } else if (pOperand1 == nullptr || pOperand2 == nullptr) {
        pLogger->log("One of the operands is null\n", RESULT_CODE::BAD_REFERENCE);
        return nullptr;
    } else if (std::isnan(tolerance)) {
        pLogger->log("Tolerance is NaN\n", RESULT_CODE::NAN_VALUE);
        return nullptr;
    } else if (tolerance < 0) {
        pLogger->log("Tolerance is less than zero\n", RESULT_CODE::NAN_VALUE);
        return nullptr;
    } else if ((pOperand1->getDim() != pOperand2->getDim()) && (pOperand1->getDim() != 0 || pOperand2->getDim() != 0)){
        pLogger->log("Sets have different dimensions\n", RESULT_CODE::WRONG_DIM);
        return nullptr;
    } else {
        ISet *inter = new (std::nothrow) SetImpl(pLogger);
        if (inter == nullptr) {
            pLogger->log("No memory for Vector\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        } else {
            size_t size_1 = pOperand1->getSize();
            IVector *buf_1 = nullptr;
            IVector *buf_2 = nullptr;
            for (size_t i = 0; i < size_1; ++i) {
                pOperand1->get(buf_1, i);
                pOperand2->get(buf_2, buf_1, norm, tolerance);
                if (buf_2 == nullptr) {
                    inter->insert(buf_1, norm, tolerance);
                }
            }
            return inter;
        }
    }
}
ISet* ISet::symSub(ISet const* pOperand1, ISet const* pOperand2, IVector::NORM norm, double tolerance, ILogger* pLogger) {
    if (pLogger == nullptr) {
        return nullptr;
    } else if (pOperand1 == nullptr || pOperand2 == nullptr) {
        pLogger->log("One of the operands is null\n", RESULT_CODE::BAD_REFERENCE);
        return nullptr;
    } else if (std::isnan(tolerance)) {
        pLogger->log("Tolerance is NaN\n", RESULT_CODE::NAN_VALUE);
        return nullptr;
    } else if (tolerance < 0) {
        pLogger->log("Tolerance is less than zero\n", RESULT_CODE::NAN_VALUE);
        return nullptr;
    } else if ((pOperand1->getDim() != pOperand2->getDim()) && (pOperand1->getDim() != 0 || pOperand2->getDim() != 0)){
        pLogger->log("Sets have different dimensions\n", RESULT_CODE::WRONG_DIM);
        return nullptr;
    } else {
        ISet *inter = new (std::nothrow) SetImpl(pLogger);
        if (inter == nullptr) {
            pLogger->log("No memory for Vector\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        } else {
            size_t size_1 = pOperand1->getSize();
            IVector *buf_1 = nullptr;
            IVector *buf_2 = nullptr;
            for (size_t i = 0; i < size_1; ++i) {
                pOperand1->get(buf_1, i);
                pOperand2->get(buf_2, buf_1, norm, tolerance);
                if (buf_2 == nullptr) {
                    inter->insert(buf_1, norm, tolerance);
                }
            }
            size_t size_2 = pOperand2->getSize();
            buf_1 = nullptr;
            buf_2 = nullptr;
            for (size_t i = 0; i < size_2; ++i) {
                pOperand2->get(buf_2, i);
                pOperand1->get(buf_1, buf_2, norm, tolerance);
                if (buf_1 == nullptr) {
                    inter->insert(buf_2, norm, tolerance);
                }
            }
            return inter;
        }
    }
}
ISet::~ISet() {}
