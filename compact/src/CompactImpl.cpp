#include "../include/ICompact.h"
//#include "../src/IVector.h"
#include <iostream>
#include <cmath>

namespace {

#define TOLERANCE 1e-5

bool _is_IVec_left_equal(const IVector *left, const IVector *right) {
    size_t dim = left->getDim();
    for (size_t i = 0; i < dim; ++i) {
        if (left->getCoord(i) > right->getCoord(i)) {
            return false;
        }
    }
    return true;
}

class CompactImpl : public ICompact {
public:
    class iterator;

    CompactImpl(IVector * begin, IVector * end, ILogger * logger)
        :_log(logger), _left(begin), _right(end)
    {}
    virtual IVector* getBegin() const override {
        return _left->clone();
    }
    virtual IVector* getEnd() const override {
        return _right->clone();
    }
    virtual ICompact::iterator* end(IVector const* const step = 0) override {
        if (step == nullptr) {
            _log->log("Vector pointer is null\n", RESULT_CODE::BAD_REFERENCE);
            return nullptr;
        } else if (step->getDim() != _left->getDim()) {
            _log->log("Step and compact have different dimensions\n", RESULT_CODE::WRONG_DIM);
            return nullptr;
        } else {
            size_t dim = step->getDim();
            for (size_t i = 0; i < dim; ++i) {
                if (step->getCoord(i) >= 0) {
                    _log->log("One of the steps is above or equals zero\n", RESULT_CODE::WRONG_ARGUMENT);
                    return nullptr;
                }
            }
            IVector * step_iter = step->clone();
            if (step_iter == nullptr) {
                _log->log("No memory for Iterator\n", RESULT_CODE::OUT_OF_MEMORY);
                return nullptr;
            }
            ICompact * comp_iter = this->clone();
            if (comp_iter == nullptr) {
                _log->log("No memory for Iterator\n", RESULT_CODE::OUT_OF_MEMORY);
                delete step_iter;
                return nullptr;
            }
            size_t * order_iter = new size_t[step_iter->getDim()];
            if (order_iter == nullptr) {
                _log->log("No memory for Iterator\n", RESULT_CODE::OUT_OF_MEMORY);
                delete step_iter;
                delete comp_iter;
                return nullptr;
            }
            for (size_t i = 0; i < dim; ++i) {
                order_iter[i] = i;
            }
            IVector * cur_point_iter = this->getEnd();
            if (cur_point_iter == nullptr) {
                _log->log("No memory for Iterator\n", RESULT_CODE::OUT_OF_MEMORY);
                delete step_iter;
                delete comp_iter;
                delete[] order_iter;
                return nullptr;
            }
            ICompact::iterator * iter = new iteratorImpl(comp_iter, step_iter, order_iter, cur_point_iter, true, _log);
            if (iter == nullptr) {
                _log->log("No memory for Iterator\n", RESULT_CODE::OUT_OF_MEMORY);
                delete step_iter;
                delete comp_iter;
                delete[] order_iter;
                delete cur_point_iter;
                return nullptr;
            } else {
                return iter;
            }
        }
    }
    virtual ICompact::iterator* begin(IVector const* const step = 0) override{
        if (step == nullptr) {
            _log->log("Vector pointer is null\n", RESULT_CODE::BAD_REFERENCE);
            return nullptr;
        } else if (step->getDim() != _left->getDim()) {
            _log->log("Step and compact have different dimensions\n", RESULT_CODE::WRONG_DIM);
            return nullptr;
        } else {
            size_t dim = step->getDim();
            for (size_t i = 0; i < dim; ++i) {
                if (step->getCoord(i) <= 0) {
                    _log->log("One of the steps is below or equals zero\n", RESULT_CODE::WRONG_ARGUMENT);
                    return nullptr;
                }
            }
            IVector * step_iter = step->clone();
            if (step_iter == nullptr) {
                _log->log("No memory for Iterator\n", RESULT_CODE::OUT_OF_MEMORY);
                return nullptr;
            }
            ICompact * comp_iter = this->clone();
            if (comp_iter == nullptr) {
                _log->log("No memory for Iterator\n", RESULT_CODE::OUT_OF_MEMORY);
                delete step_iter;
                return nullptr;
            }
            size_t * order_iter = new size_t[step_iter->getDim()];
            if (order_iter == nullptr) {
                _log->log("No memory for Iterator\n", RESULT_CODE::OUT_OF_MEMORY);
                delete step_iter;
                delete comp_iter;
                return nullptr;
            }
            for (size_t i = 0; i < dim; ++i) {
                order_iter[i] = i;
            }
            IVector * cur_point_iter = this->getBegin();
            if (cur_point_iter == nullptr) {
                _log->log("No memory for Iterator\n", RESULT_CODE::OUT_OF_MEMORY);
                delete step_iter;
                delete comp_iter;
                delete[] order_iter;
                return nullptr;
            }
            ICompact::iterator * iter = new iteratorImpl(comp_iter, step_iter, order_iter, cur_point_iter, false, _log);
            if (iter == nullptr) {
                _log->log("No memory for Iterator\n", RESULT_CODE::OUT_OF_MEMORY);
                delete step_iter;
                delete comp_iter;
                delete[] order_iter;
                delete cur_point_iter;
                return nullptr;
            } else {
                return iter;
            }
        }
    }
    virtual RESULT_CODE isContains(IVector const* const vec, bool& result) const override {
        if (vec == nullptr) {
            _log->log("Vector pointer is null\n", RESULT_CODE::BAD_REFERENCE);
            return RESULT_CODE::BAD_REFERENCE;
        } else if (vec->getDim() != _left->getDim()) {
            _log->log("Vector has wrong dimension\n", RESULT_CODE::WRONG_DIM);
            return RESULT_CODE::WRONG_DIM;
        } else {
            result = (_is_IVec_left_equal(_left, vec) && _is_IVec_left_equal(vec, _right));
            return RESULT_CODE::SUCCESS;
        }
    }
    virtual RESULT_CODE isSubSet(ICompact const* const other,bool& result) const override {
        if (other == nullptr) {
            _log->log("Vector pointer is null\n", RESULT_CODE::BAD_REFERENCE);
            return RESULT_CODE::BAD_REFERENCE;
        } else if (other->getDim() != _left->getDim()) {
            _log->log("Vector has wrong dimension\n", RESULT_CODE::WRONG_DIM);
            return RESULT_CODE::WRONG_DIM;
        } else {
            IVector * left_buf = other->getBegin();
            IVector * right_buf = other->getEnd();
            result = (_is_IVec_left_equal(_left, left_buf) && _is_IVec_left_equal(right_buf, _right));
            delete left_buf;
            delete right_buf;
            return RESULT_CODE::SUCCESS;
        }
    }
    virtual RESULT_CODE isIntersects(ICompact const* const other, bool& result) const override {
        if (other == nullptr) {
            _log->log("Vector pointer is null\n", RESULT_CODE::BAD_REFERENCE);
            return RESULT_CODE::BAD_REFERENCE;
        } else if (other->getDim() != _left->getDim()) {
            _log->log("Vector has wrong dimension\n", RESULT_CODE::WRONG_DIM);
            return RESULT_CODE::WRONG_DIM;
        } else {
            IVector * left_buf = other->getBegin();
            IVector * right_buf = other->getEnd();
            size_t intersec_dims = 0;
            size_t dim = _left->getDim();
            for (size_t i = 0; i < dim; ++i) {
                if ((left_buf->getCoord(i) <= _left->getCoord(i) && _left->getCoord(i) <= right_buf->getCoord(i)) ||
                        (left_buf->getCoord(i) <= _right->getCoord(i) && _right->getCoord(i) <= right_buf->getCoord(i))) {
                    intersec_dims++;
                }
            }
            result = (intersec_dims == _left->getDim());
            delete left_buf;
            delete right_buf;
            return RESULT_CODE::SUCCESS;
        }
    }
    virtual size_t getDim() const override {
        return _left->getDim();
    }
    virtual ICompact* clone() const override {
        IVector * new_left = _left->clone();
        if (new_left == nullptr) {
            _log->log("No memory for Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        }
        IVector * new_right = _right->clone();
        if (new_right == nullptr) {
            _log->log("No memory for Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            delete new_left;
            return nullptr;
        }
        ICompact * compact =  new (std::nothrow) CompactImpl(new_left, new_right, _log);
        if (compact == nullptr) {
            delete new_left;
            delete new_right;
            _log->log("No memory for Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        }
        return compact;
    }
    ~CompactImpl() {
        delete _left;
        delete _right;
    }

    class iteratorImpl : public ICompact::iterator {
    public:
        iteratorImpl(ICompact * parent, IVector * steps, size_t * step_order, IVector * cur_point, bool reverse, ILogger * log)
            :_parent_compact(parent), _step_order(step_order), _steps(steps), _cur_point(cur_point), _reverse(reverse), _log(log)
        {}
        virtual RESULT_CODE doStep() override {
            size_t dim = _parent_compact->getDim();
            IVector * end_point;
            if (_reverse) {
                end_point = _parent_compact->getBegin();
            } else {
                end_point = _parent_compact->getEnd();
            }
            if (end_point == nullptr) {
                _log->log("No memory for stepping\n", RESULT_CODE::OUT_OF_MEMORY);
                return RESULT_CODE::OUT_OF_MEMORY;
            }
            IVector * start_point;
            if (_reverse) {
                start_point = _parent_compact->getEnd();
            } else {
                start_point = _parent_compact->getBegin();
            }
            if (start_point == nullptr) {
                _log->log("No memory for stepping\n", RESULT_CODE::OUT_OF_MEMORY);
                delete end_point;
                return RESULT_CODE::OUT_OF_MEMORY;
            }
            IVector * cur_point_clone = _cur_point->clone();
            if (cur_point_clone == nullptr) {
                _log->log("No memory for stepping\n", RESULT_CODE::OUT_OF_MEMORY);
                delete end_point;
                delete start_point;
                return RESULT_CODE::OUT_OF_MEMORY;
            }
            size_t cur_dim;
            for (size_t i = 0; i < dim; ++i) {
                cur_dim = _step_order[i];

                if (_reverse) {
                    if ((_cur_point->getCoord(cur_dim) + _steps->getCoord(cur_dim) >= end_point->getCoord(cur_dim)) ||
                    (std::fabs(_cur_point->getCoord(cur_dim) + _steps->getCoord(cur_dim) - end_point->getCoord(cur_dim)) < TOLERANCE)){
                        _cur_point->setCoord(cur_dim, _cur_point->getCoord(cur_dim) + _steps->getCoord(cur_dim));
                        delete end_point;
                        delete start_point;
                        delete cur_point_clone;

                        return RESULT_CODE::SUCCESS;
                    }
                } else {
                    if ((_cur_point->getCoord(cur_dim) + _steps->getCoord(cur_dim) <= end_point->getCoord(cur_dim)) ||
                    (std::fabs(_cur_point->getCoord(cur_dim) + _steps->getCoord(cur_dim) - end_point->getCoord(cur_dim)) < TOLERANCE)){
                        _cur_point->setCoord(cur_dim, _cur_point->getCoord(cur_dim) + _steps->getCoord(cur_dim));
                        delete end_point;
                        delete start_point;
                        delete cur_point_clone;

                        return RESULT_CODE::SUCCESS;
                    }
                }
                _cur_point->setCoord(cur_dim, start_point->getCoord(cur_dim));
            }
            delete end_point;
            delete start_point;
            delete _cur_point;
            _cur_point = cur_point_clone;

            return RESULT_CODE::OUT_OF_BOUNDS;
        }
        virtual IVector* getPoint() const override {
            return _cur_point->clone();
        }
        virtual RESULT_CODE setDirection(IVector const* const dir) override {
            if (dir == nullptr) {
                _log->log("Vector pointer is null\n", RESULT_CODE::BAD_REFERENCE);
                return RESULT_CODE::BAD_REFERENCE;
            } else if (dir->getDim() != _parent_compact->getDim()) {
                _log->log("Vector has wrong dimension\n", RESULT_CODE::WRONG_DIM);
                return RESULT_CODE::WRONG_DIM;
            } else {
                size_t dim = dir->getDim();
                size_t count = 0;
                for (size_t i = 0; i < dim; ++i) {
                    for (size_t j = 0; j < dim; ++j) {
                        if ((size_t)(std::round(dir->getCoord(j))) == i) {
                            count++;
                            break;
                        }
                    }
                }
                if (count != dim) {
                    _log->log("Wrong direction order of dimensions\n", RESULT_CODE::WRONG_ARGUMENT);
                    return RESULT_CODE::WRONG_ARGUMENT;
                } else {
                    IVector * new_cur;
                    if (_reverse) {
                        new_cur = _parent_compact->getEnd();
                    } else {
                        new_cur = _parent_compact->getBegin();
                    }
                    if (new_cur == nullptr) {
                        return RESULT_CODE::OUT_OF_MEMORY;
                    } else {
                        delete _cur_point;
                        _cur_point = new_cur;

                        for (size_t i = 0; i < dim; ++i) {
                            _step_order[i] = (size_t)std::fabs(std::round(dir->getCoord(i)));
                        }

                        return RESULT_CODE::SUCCESS;
                    }
                }
            }
        }
        virtual ~iteratorImpl() {
            delete _parent_compact;
            delete _step_order;
            delete _steps;
            delete _cur_point;
        }

    private:
        ICompact * _parent_compact;
        size_t * _step_order;
        IVector * _steps;
        IVector * _cur_point;
        bool _reverse;
        ILogger *_log;
    };

private:
    ILogger *_log;
    IVector *_left;
    IVector *_right;
};

}

ICompact* ICompact::createCompact(IVector const* const begin, IVector const* const end, ILogger*logger) {
    if (logger == nullptr) {
        return nullptr;
    } else if (begin == nullptr || end == nullptr) {
        logger->log("One of the operands is null\n", RESULT_CODE::BAD_REFERENCE);
        return nullptr;
    } else if (begin->getDim() != end->getDim()) {
        logger->log("Vectors have different dimensions\n", RESULT_CODE::WRONG_DIM);
        return nullptr;
    } else if (!_is_IVec_left_equal(begin, end)) {
        logger->log("Vectors can't make compact\n", RESULT_CODE::WRONG_ARGUMENT);
        return nullptr;
    } else {
        IVector * begin_comp = begin->clone();
        if (begin_comp == nullptr) {
            logger->log("No memory for Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        }
        IVector * end_comp = end->clone();
        if (end_comp == nullptr) {
            logger->log("No memory for Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            delete begin_comp;
            return nullptr;
        }
        ICompact * comp = new CompactImpl(begin_comp, end_comp, logger);
        if (comp == nullptr) {
            delete begin_comp;
            delete end_comp;
            logger->log("No memory for Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        } else {
            return comp;
        }
    }
}
ICompact* ICompact::intersection(ICompact const* const left, ICompact const* const right, ILogger*logger) {
    if (logger == nullptr) {
        return nullptr;
    } else if (left == nullptr || right == nullptr) {
        logger->log("One of the operands is null\n", RESULT_CODE::BAD_REFERENCE);
        return nullptr;
    } else if (left->getDim() != right->getDim()) {
        logger->log("Compacts have different dimensions\n", RESULT_CODE::WRONG_DIM);
        return nullptr;
    } else {
        bool result;
        RESULT_CODE res = left->isSubSet(right, result);
        if (res == RESULT_CODE::SUCCESS && result) {
            ICompact * intersec = right->clone();
            return intersec;
        }
        res = right->isSubSet(left, result);
        if (res == RESULT_CODE::SUCCESS && result) {
            ICompact * intersec = left->clone();
            return intersec;
        }
        res = left->isIntersects(right, result);
        if (res == RESULT_CODE::SUCCESS && result) {
            size_t dims = left->getDim();
            double * coord_left = new double[dims];
            if (coord_left == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                return nullptr;
            }
            double * coord_right = new double[dims];
            if (coord_right == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                delete[] coord_left;
                return nullptr;
            }
            IVector * left_begin = left->getBegin();
            if (left_begin == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                delete[] coord_left;
                delete[] coord_right;
                return nullptr;
            }
            IVector * left_end = left->getEnd();
            if (left_end == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                delete[] coord_left;
                delete[] coord_right;
                delete left_begin;
                return nullptr;
            }
            IVector * right_begin = right->getBegin();
            if (right_begin == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                delete[] coord_left;
                delete[] coord_right;
                delete left_begin;
                delete left_end;
                return nullptr;
            }
            IVector * right_end = right->getEnd();
            if (right_end == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                delete[] coord_left;
                delete[] coord_right;
                delete left_begin;
                delete left_end;
                delete right_begin;
                return nullptr;
            }
            for (size_t i = 0; i < dims; ++i) {
                coord_left[i] = (left_begin->getCoord(i) < right_begin->getCoord(i)) ? right_begin->getCoord(i) : left_begin->getCoord(i);
                coord_right[i] = (left_end->getCoord(i) < right_end->getCoord(i)) ? left_end->getCoord(i) : right_end->getCoord(i);
            }
            delete left_begin;
            delete left_end;
            delete right_begin;
            delete right_end;
            IVector * begin_vec = IVector::createVector(dims, coord_left, logger);
            delete[] coord_left;
            if (begin_vec == nullptr) {
                delete[] coord_right;
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                return nullptr;
            }
            IVector * end_vec = IVector::createVector(dims, coord_right, logger);
            delete[] coord_right;
            if (begin_vec == nullptr) {
                delete begin_vec;
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                return nullptr;
            }
            ICompact * new_comp = new CompactImpl(begin_vec, end_vec, logger);
            if (new_comp == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                return nullptr;
            }
            return new_comp;
        }
        logger->log("Compacts don't intersect\n", RESULT_CODE::WRONG_ARGUMENT);
        return nullptr;
    }
}
ICompact* ICompact::add(ICompact const* const left, ICompact const* const right, ILogger*logger) {
    if (logger == nullptr) {
        return nullptr;
    } else if (left == nullptr || right == nullptr) {
        logger->log("One of the operands is null\n", RESULT_CODE::BAD_REFERENCE);
        return nullptr;
    } else if (left->getDim() != right->getDim()) {
        logger->log("Compacts have different dimensions\n", RESULT_CODE::WRONG_DIM);
        return nullptr;
    } else {
        bool result;
        RESULT_CODE res = left->isSubSet(right, result);
        if (res == RESULT_CODE::SUCCESS && result) {
            ICompact * add = left->clone();
            return add;
        }
        res = right->isSubSet(left, result);
        if (res == RESULT_CODE::SUCCESS && result) {
            ICompact * add = right->clone();
            return add;
        }
        res = left->isIntersects(right, result);
        if (res == RESULT_CODE::SUCCESS && result) {
            IVector * left_begin = left->getBegin();
            if (left_begin == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                return nullptr;
            }
            IVector * left_end = left->getEnd();
            if (left_end == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                delete left_begin;
                return nullptr;
            }
            IVector * right_begin = right->getBegin();
            if (right_begin == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                delete left_begin;
                delete left_end;
                return nullptr;
            }
            IVector * right_end = right->getEnd();
            if (right_end == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                delete left_begin;
                delete left_end;
                delete right_begin;
                return nullptr;
            }
            IVector * new_left = IVector::sub(left_begin, right_begin, logger);
            if (new_left == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                delete left_begin;
                delete left_end;
                delete right_begin;
                delete right_end;
                return nullptr;
            }
            IVector * new_right = IVector::sub(left_end, right_end, logger);
            if (new_right == nullptr) {
                logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                delete left_begin;
                delete left_end;
                delete right_begin;
                delete right_end;
                delete new_left;
                return nullptr;
            }
            size_t dims = left->getDim();
            size_t left_count = 0;
            size_t right_count = 0;
            size_t left_indx = 0;
            size_t right_indx = 0;
            for (size_t i = 0; i < dims; i++) {
                if (new_left->getCoord(i) != 0) {
                    left_count++;
                    left_indx = i;
                }
                if (new_right->getCoord(i) != 0) {
                    right_count++;
                    right_indx = i;
                }
            }
            delete left_begin;
            delete left_end;
            delete right_begin;
            delete right_end;
            delete new_left;
            delete new_right;
            if (left_count <= 1 && right_count <= 1 && left_indx == right_indx) {
                ICompact * res = ICompact::makeConvex(left, right, logger);
                if (res == nullptr) {
                    logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
                    return nullptr;
                }
                return res;
            }
            logger->log("Wrong compact args\n", RESULT_CODE::WRONG_ARGUMENT);
            return nullptr;
        }
        logger->log("Compacts don't intersect\n", RESULT_CODE::WRONG_ARGUMENT);
        return nullptr;
    }
}
ICompact* ICompact::makeConvex(ICompact const* const left, ICompact const* const right, ILogger*logger) {
    if (logger == nullptr) {
        return nullptr;
    } else if (left == nullptr || right == nullptr) {
        logger->log("One of the operands is null\n", RESULT_CODE::BAD_REFERENCE);
        return nullptr;
    } else if (left->getDim() != right->getDim()) {
        logger->log("Compacts have different dimensions\n", RESULT_CODE::WRONG_DIM);
        return nullptr;
    } else {
        size_t dims = left->getDim();
        double * coord_left = new double[dims];
        if (coord_left == nullptr) {
            logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        }
        double * coord_right = new double[dims];
        if (coord_right == nullptr) {
            logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            delete[] coord_left;
            return nullptr;
        }
        IVector * left_begin = left->getBegin();
        if (left_begin == nullptr) {
            logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            delete[] coord_left;
            delete[] coord_right;
            return nullptr;
        }
        IVector * left_end = left->getEnd();
        if (left_end == nullptr) {
            logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            delete[] coord_left;
            delete[] coord_right;
            delete left_begin;
            return nullptr;
        }
        IVector * right_begin = right->getBegin();
        if (right_begin == nullptr) {
            logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            delete[] coord_left;
            delete[] coord_right;
            delete left_begin;
            delete left_end;
            return nullptr;
        }
        IVector * right_end = right->getEnd();
        if (right_end == nullptr) {
            logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            delete[] coord_left;
            delete[] coord_right;
            delete left_begin;
            delete left_end;
            delete right_begin;
            return nullptr;
        }
        for (size_t i = 0; i < dims; ++i) {
            coord_left[i] = (left_begin->getCoord(i) < right_begin->getCoord(i)) ? left_begin->getCoord(i) : right_begin->getCoord(i);
            coord_right[i] = (left_end->getCoord(i) < right_end->getCoord(i)) ? right_end->getCoord(i) : left_end->getCoord(i);
        }
        delete left_begin;
        delete left_end;
        delete right_begin;
        delete right_end;
        IVector * begin_vec = IVector::createVector(dims, coord_left, logger);
        delete[] coord_left;
        if (begin_vec == nullptr) {
            delete[] coord_right;
            logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        }
        IVector * end_vec = IVector::createVector(dims, coord_right, logger);
        delete[] coord_right;
        if (begin_vec == nullptr) {
            delete begin_vec;
            logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        }
        ICompact * new_comp = new CompactImpl(begin_vec, end_vec, logger);
        if (new_comp == nullptr) {
            logger->log("No memory for new Compact\n", RESULT_CODE::OUT_OF_MEMORY);
            return nullptr;
        }
        return new_comp;
    }
    logger->log("Compacts don't intersect\n", RESULT_CODE::WRONG_ARGUMENT);
    return nullptr;
}
ICompact::~ICompact() {};
