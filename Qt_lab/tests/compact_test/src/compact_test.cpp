#include <iostream>
#include "../src/IVector.h"
#include "../src/ILogger.h"
#include "../src/RC.h"
#include "../src/ICompact.h"

#define TOLERANCE 1e-5

using namespace std;

bool testCreate(ILogger * pLogger) {
    double data1[] = {1.0, 2.0};
    double data2[] = {3.0, 4.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    IVector * vec2 = IVector::createVector(2, data2, pLogger);

    ICompact * compact = ICompact::createCompact(vec1, vec2, pLogger);
    if (compact == nullptr) {
        delete vec1;
        delete vec2;

        return false;
    }

    double data3[] = {2.0, 3.0};
    IVector * vec3 = IVector::createVector(2, data3, pLogger);

    bool res = false;

    compact->isContains(vec3, res);

    if (res == false) {
        delete vec1;
        delete vec2;
        delete vec3;

        delete compact;

        return false;
    }

    delete vec1;
    delete vec2;
    delete vec3;

    delete compact;

    return true;
}
bool testIntersect(ILogger * pLogger) {
    double data1[] = {1.0, 2.0};
    double data2[] = {3.0, 4.0};
    double data3[] = {2.0, 3.0};
    double data4[] = {2.0, 3.0};
    double data5[] = {2.0, 3.0};
    double data6[] = {5.0, 7.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    IVector * vec3 = IVector::createVector(2, data3, pLogger);
    IVector * vec4 = IVector::createVector(2, data4, pLogger);
    IVector * vec5 = IVector::createVector(2, data5, pLogger);
    IVector * vec6 = IVector::createVector(2, data6, pLogger);

    ICompact * compact1 = ICompact::createCompact(vec1, vec2, pLogger);
    ICompact * compact2 = ICompact::createCompact(vec3, vec4, pLogger);
    ICompact * compact3 = ICompact::createCompact(vec5, vec6, pLogger);

    bool res = false;
    compact1->isSubSet(compact2, res);
    if (res == false) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;
        delete vec5;
        delete vec6;

        delete compact1;
        delete compact2;
        delete compact3;

        return false;
    }
    res = false;
    compact1->isIntersects(compact3, res);
    if (res == false) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;
        delete vec5;
        delete vec6;

        delete compact1;
        delete compact2;
        delete compact3;

        return false;
    }

    delete vec1;
    delete vec2;
    delete vec3;
    delete vec4;
    delete vec5;
    delete vec6;

    delete compact1;
    delete compact2;
    delete compact3;

    return true;
}
bool testAdd(ILogger * pLogger) {
    double data1[] = {1.0, 2.0};
    double data2[] = {3.0, 4.0};
    double data3[] = {3.0, 2.0};
    double data4[] = {5.0, 4.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    IVector * vec3 = IVector::createVector(2, data3, pLogger);
    IVector * vec4 = IVector::createVector(2, data4, pLogger);

    ICompact * compact1 = ICompact::createCompact(vec1, vec2, pLogger);
    ICompact * compact2 = ICompact::createCompact(vec3, vec4, pLogger);

    ICompact * compact3 = ICompact::add(compact1, compact2, pLogger);
    if (compact3 == nullptr) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;

        delete compact1;
        delete compact2;

        return false;
    }

    IVector * start = compact3->getBegin();
    IVector * end = compact3->getEnd();

    bool res = false;
    IVector::equals(start, vec1, IVector::NORM::NORM_INF, TOLERANCE, &res, pLogger);
    if (res == false) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;
        delete start;
        delete end;

        delete compact1;
        delete compact2;
        delete compact3;

        return false;
    }
    res = false;
    IVector::equals(end, vec4, IVector::NORM::NORM_INF, TOLERANCE, &res, pLogger);
    if (res == false) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;
        delete start;
        delete end;

        delete compact1;
        delete compact2;
        delete compact3;

        return false;
    }

    delete vec1;
    delete vec2;
    delete vec3;
    delete vec4;
    delete start;
    delete end;

    delete compact1;
    delete compact2;
    delete compact3;

    return true;
}
bool testConvex(ILogger * pLogger) {
    double data1[] = {1.0, 2.0};
    double data2[] = {2.0, 4.0};
    double data3[] = {3.0, 6.0};
    double data4[] = {5.0, 7.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    IVector * vec3 = IVector::createVector(2, data3, pLogger);
    IVector * vec4 = IVector::createVector(2, data4, pLogger);

    ICompact * compact1 = ICompact::createCompact(vec1, vec2, pLogger);
    ICompact * compact2 = ICompact::createCompact(vec3, vec4, pLogger);

    ICompact * compact3 = ICompact::makeConvex(compact1, compact2, pLogger);
    if (compact3 == nullptr) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;

        delete compact1;
        delete compact2;

        return false;
    }

    IVector * start = compact3->getBegin();
    IVector * end = compact3->getEnd();

    bool res = false;
    IVector::equals(start, vec1, IVector::NORM::NORM_INF, TOLERANCE, &res, pLogger);
    if (res == false) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;
        delete start;
        delete end;

        delete compact1;
        delete compact2;
        delete compact3;

        return false;
    }
    res = false;
    IVector::equals(end, vec4, IVector::NORM::NORM_INF, TOLERANCE, &res, pLogger);
    if (res == false) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;
        delete start;
        delete end;

        delete compact1;
        delete compact2;
        delete compact3;

        return false;
    }

    delete vec1;
    delete vec2;
    delete vec3;
    delete vec4;
    delete start;
    delete end;

    delete compact1;
    delete compact2;
    delete compact3;

    return true;
}
bool testIntersection(ILogger * pLogger) {
    double data1[] = {1.0, 2.0};
    double data2[] = {4.0, 4.0};
    double data3[] = {3.0, 3.0};
    double data4[] = {5.0, 7.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    IVector * vec3 = IVector::createVector(2, data3, pLogger);
    IVector * vec4 = IVector::createVector(2, data4, pLogger);

    ICompact * compact1 = ICompact::createCompact(vec1, vec2, pLogger);
    ICompact * compact2 = ICompact::createCompact(vec3, vec4, pLogger);

    ICompact * compact3 = ICompact::intersection(compact1, compact2, pLogger);
    if (compact3 == nullptr) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;

        delete compact1;
        delete compact2;

        return false;
    }

    IVector * start = compact3->getBegin();
    IVector * end = compact3->getEnd();

    bool res = false;
    IVector::equals(start, vec3, IVector::NORM::NORM_INF, TOLERANCE, &res, pLogger);
    if (res == false) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;
        delete start;
        delete end;

        delete compact1;
        delete compact2;
        delete compact3;

        return false;
    }
    res = false;
    IVector::equals(end, vec2, IVector::NORM::NORM_INF, TOLERANCE, &res, pLogger);
    if (res == false) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;
        delete start;
        delete end;

        delete compact1;
        delete compact2;
        delete compact3;

        return false;
    }

    delete vec1;
    delete vec2;
    delete vec3;
    delete vec4;
    delete start;
    delete end;

    delete compact1;
    delete compact2;
    delete compact3;

    return true;
}
bool testIterator(ILogger * pLogger) {
    double data1[] = {1.0, 2.0};
    double data2[] = {4.0, 4.0};
    double data3[] = {0.5, 0.4};
    double data8[] = {0, 1};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    IVector * vec3 = IVector::createVector(2, data3, pLogger);
    IVector * vec8 = IVector::createVector(2, data8, pLogger);

    ICompact * compact1 = ICompact::createCompact(vec1, vec2, pLogger);

    ICompact::iterator * iter = compact1->begin(vec3);
    iter->setDirection(vec8);

    IVector * vec4 = iter->getPoint();
    bool res = false;
    IVector::equals(vec4, vec1, IVector::NORM::NORM_INF, TOLERANCE, &res, pLogger);
    if (res == false) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;
        delete vec8;

        delete compact1;

        delete iter;

        return false;
    }

    while (iter->doStep() == RESULT_CODE::SUCCESS);
    res = false;
    IVector * vec5 = iter->getPoint();
    IVector::equals(vec5, vec2, IVector::NORM::NORM_INF, TOLERANCE, &res, pLogger);
    if (res == false) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;
        delete vec5;
        delete vec8;

        delete compact1;

        delete iter;

        return false;
    }

    IVector * vec7 = IVector::mul(vec3, -1, pLogger);
    ICompact::iterator * iter2 = compact1->end(vec7);
    iter2->setDirection(vec8);
    while (iter2->doStep() == RESULT_CODE::SUCCESS);
    res = false;
    IVector * vec6 = iter2->getPoint();
    IVector::equals(vec6, vec1, IVector::NORM::NORM_INF, TOLERANCE, &res, pLogger);
    if (res == false) {
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;
        delete vec5;
        delete vec6;
        delete vec7;
        delete vec8;

        delete compact1;

        delete iter;
        delete iter2;

        return false;
    }

    delete vec1;
    delete vec2;
    delete vec3;
    delete vec4;
    delete vec5;
    delete vec6;
    delete vec7;
    delete vec8;

    delete iter;
    delete iter2;

    delete compact1;

    return true;
}


int main()
{
    int for_log = 5;
    ILogger * log = ILogger::createLogger(&for_log);
    if (testCreate(log)) {
        std::cout << "Test Create | OK" << std::endl;
    } else {
        std::cout << "Test Create | FAIL" << std::endl;
    }
    if (testIntersect(log)) {
        std::cout << "Test Intersect SubSet | OK" << std::endl;
    } else {
        std::cout << "Test Intersect SubSet | FAIL" << std::endl;
    }
    if (testAdd(log)) {
        std::cout << "Test Add | OK" << std::endl;
    } else {
        std::cout << "Test Add | FAIL" << std::endl;
    }
    if (testConvex(log)) {
        std::cout << "Test Convex | OK" << std::endl;
    } else {
        std::cout << "Test Convex | FAIL" << std::endl;
    }
    if (testIntersection(log)) {
        std::cout << "Test Intersection | OK" << std::endl;
    } else {
        std::cout << "Test Intersection | FAIL" << std::endl;
    }
    if (testIterator(log)) {
        std::cout << "Test Iterator | OK" << std::endl;
    } else {
        std::cout << "Test Iterator | FAIL" << std::endl;
    }
    std::cout << "Enter any number to exit:" << std::endl;
    std::cin >> for_log;
    log->destroyLogger(&for_log);
    return 0;
}
