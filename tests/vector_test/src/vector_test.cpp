#include <iostream>
#include "IVector.h"
#include "ILogger.h"
#include "RC.h"
#include <cmath>

#define TOLERANCE 1e-5

bool testCreate(ILogger *pLogger) {
    double data[] = {1.0, NAN};
    IVector * vec = IVector::createVector(2, data, pLogger);
    if (vec != nullptr) {
        delete vec;
        return false;
    }
    data[1] = 2;
    vec = IVector::createVector(2, data, pLogger);
    if (vec == nullptr) {
        return false;
    }
    size_t dim = vec->getDim();
    if (dim != 2) {
        delete vec;
        return false;
    }
    for (size_t i = 0; i < dim; ++i) {
        if (vec->getCoord(i) != data[i]) {
            delete vec;
            return false;
        }
    }
    delete vec;
    return true;
}
bool testClone(ILogger *pLogger) {
    double data[] = {1.0, 2.0};
    IVector * vec = IVector::createVector(2, data, pLogger);
    if (vec == nullptr) {
        return false;
    }
    IVector *copy = vec->clone();
    if (copy == nullptr) {
        return false;
    }
    size_t dim = vec->getDim();
    for (size_t i = 0; i < dim; ++i) {
        if (vec->getCoord(i) != copy->getCoord(i)) {
            delete vec;
            delete copy;
            return false;
        }
    }
    delete vec;
    delete copy;
    return true;
}
bool testAdd(ILogger *pLogger) {
    double data1[] = {1.0, 2.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    if (vec1 == nullptr) {
        return false;
    }
    double data2[] = {3.0, -1.0};
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    if (vec2 == nullptr) {
        delete vec1;
        return false;
    }
    IVector * vec3 = IVector::add(vec1, vec2, pLogger);
    if (vec3 == nullptr) {
        delete vec1;
        delete vec2;
        return false;
    }
    size_t dim = vec1->getDim();
    for (size_t i = 0; i < dim; ++i) {
        if (std::fabs(vec3->getCoord(i) - vec2->getCoord(i) - vec1->getCoord(i)) > TOLERANCE) {
            delete vec1;
            delete vec2;
            delete vec3;
            return false;
        }
    }
    delete vec1;
    delete vec2;
    delete vec3;
    return true;
}
bool testSub(ILogger *pLogger) {
    double data1[] = {1.0, 2.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    if (vec1 == nullptr) {
        return false;
    }
    double data2[] = {3.0, -1.0};
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    if (vec2 == nullptr) {
        delete vec1;
        return false;
    }
    IVector * vec3 = IVector::sub(vec1, vec2, pLogger);
    if (vec3 == nullptr) {
        delete vec1;
        delete vec2;
        return false;
    }
    size_t dim = vec1->getDim();
    for (size_t i = 0; i < dim; ++i) {
        if (std::fabs(vec3->getCoord(i) + vec2->getCoord(i) - vec1->getCoord(i)) > TOLERANCE) {
            delete vec1;
            delete vec2;
            delete vec3;
            return false;
        }
    }
    delete vec1;
    delete vec2;
    delete vec3;
    return true;
}
bool testMul(ILogger *pLogger) {
    double data1[] = {1.0, 2.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    if (vec1 == nullptr) {
        return false;
    }
    IVector * vec3 = IVector::mul(vec1, 5, pLogger);
    if (vec3 == nullptr) {
        delete vec1;
        return false;
    }
    size_t dim = vec1->getDim();
    for (size_t i = 0; i < dim; ++i) {
        if (std::fabs(vec3->getCoord(i) - 5 * vec1->getCoord(i)) > TOLERANCE) {
            delete vec1;
            delete vec3;
            return false;
        }
    }
    delete vec1;
    delete vec3;
    return true;
}
bool testEquals(ILogger *pLogger) {
    double data1[] = {1.0, 2.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    if (vec1 == nullptr) {
        return false;
    }
    double data2[] = {1.09, 1.99};
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    if (vec2 == nullptr) {
        delete vec1;
        return false;
    }
    bool res = false;
    RESULT_CODE code = IVector::equals(vec1, vec2, IVector::NORM::NORM_INF, 0.1, &res, pLogger);
    delete vec1;
    delete vec2;
    if (code != RESULT_CODE::SUCCESS && res == false) {
        return false;
    }
    return true;
}
bool testSet(ILogger *pLogger) {
    double data1[] = {1.0, 2.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    if (vec1 == nullptr) {
        return false;
    }
    vec1->setCoord(1, 0.0);
    if (std::fabs(vec1->getCoord(1)) > TOLERANCE) {
        delete vec1;
        return false;
    }
    delete vec1;
    return true;
}


using namespace std;

int main()
{
    int for_log = 5;
    ILogger * log = ILogger::createLogger(&for_log);
    if (testCreate(log)) {
        std::cout << "Test Create | OK" << std::endl;
    } else {
        std::cout << "Test Create | FAIL" << std::endl;
    }
    if (testClone(log)) {
        std::cout << "Test Clone | OK" << std::endl;
    } else {
        std::cout << "Test Clone | FAIL" << std::endl;
    }
    if (testAdd(log)) {
        std::cout << "Test Add | OK" << std::endl;
    } else {
        std::cout << "Test Add | FAIL" << std::endl;
    }
    if (testSub(log)) {
        std::cout << "Test Sub | OK" << std::endl;
    } else {
        std::cout << "Test Sub | FAIL" << std::endl;
    }
    if (testMul(log)) {
        std::cout << "Test Mul | OK" << std::endl;
    } else {
        std::cout << "Test Mul | FAIL" << std::endl;
    }
    if (testEquals(log)) {
        std::cout << "Test Equals | OK" << std::endl;
    } else {
        std::cout << "Test Equals | FAIL" << std::endl;
    }
    if (testSet(log)) {
        std::cout << "Test Set | OK" << std::endl;
    } else {
        std::cout << "Test Set | FAIL" << std::endl;
    }
    std::cout << "Enter any number to exit:" << std::endl;
    std::cin >> for_log;
    log->destroyLogger(&for_log);
    return 0;
}
