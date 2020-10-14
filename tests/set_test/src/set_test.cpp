#include <iostream>
#include "../src/IVector.h"
#include "../src/ILogger.h"
#include "../src/RC.h"
#include "../src/ISet.h"

#define TOLERANCE 1e-5

using namespace std;

bool testCreateGet(ILogger * pLogger) {
    ISet * set = ISet::createSet(pLogger);
    if (set == nullptr) {
        return false;
    }
    double data1[] = {1.0, 2.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    double data2[] = {1.0, 2.0, 7.0};
    IVector * vec2 = IVector::createVector(3, data2, pLogger);
    double data3[] = {4.0, 7.0};
    IVector * vec3 = IVector::createVector(2, data3, pLogger);
    RESULT_CODE code = set->insert(vec1, IVector::NORM::NORM_INF, TOLERANCE);
    if (code != RESULT_CODE::SUCCESS) {
        delete set;
        delete vec1;
        delete vec2;
        delete vec3;
    }
    code = set->insert(vec2, IVector::NORM::NORM_INF, TOLERANCE);
    if (code == RESULT_CODE::SUCCESS) {
        delete set;
        delete vec1;
        delete vec2;
        delete vec3;
    }
    code = set->insert(vec3, IVector::NORM::NORM_INF, TOLERANCE);
    if (code != RESULT_CODE::SUCCESS) {
        delete set;
        delete vec1;
        delete vec2;
        delete vec3;
    }

    vec1->setCoord(0, 1.05);
    IVector * vec4 = nullptr;
    code = set->get(vec4, vec1, IVector::NORM::NORM_INF, 0.1);
    if (vec4 == nullptr) {
        delete set;
        delete vec1;
        delete vec2;
        delete vec3;

        return false;
    }
    bool res = false;
    IVector::equals(vec1, vec4, IVector::NORM::NORM_INF, 0.1, &res, pLogger);
    delete set;
    delete vec1;
    delete vec2;
    delete vec3;
    delete vec4;
    if (res == false) {
        return false;
    }
    return true;
}
bool testErase(ILogger * pLogger) {
    ISet * set = ISet::createSet(pLogger);
    if (set == nullptr) {
        return false;
    }
    double data1[] = {1.0, 2.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    double data2[] = {11.0, 2.0};
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    double data3[] = {4.0, 7.0};
    IVector * vec3 = IVector::createVector(2, data3, pLogger);
    set->insert(vec1, IVector::NORM::NORM_INF, TOLERANCE);
    set->insert(vec2, IVector::NORM::NORM_INF, TOLERANCE);
    set->insert(vec3, IVector::NORM::NORM_INF, TOLERANCE);

    vec1->setCoord(1, 2.01);
    set->erase(vec1, IVector::NORM::NORM_INF, 0.1);
    vec1->setCoord(1, 2.0);
    IVector * vec4 = nullptr;
    set->get(vec4, vec1, IVector::NORM::NORM_INF, 0.1);
    if (vec4 != nullptr) {
        delete set;
        delete vec1;
        delete vec2;
        delete vec3;
        delete vec4;

        return false;
    }
    delete set;
    delete vec1;
    delete vec2;
    delete vec3;

    return true;
}
bool testAdd(ILogger * pLogger) {
    ISet * set1 = ISet::createSet(pLogger);
    double data1[] = {11.0, 2.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    double data2[] = {1.0, 2.0};
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    double data3[] = {4.0, 7.0};
    IVector * vec3 = IVector::createVector(2, data3, pLogger);
    set1->insert(vec1, IVector::NORM::NORM_INF, TOLERANCE);
    set1->insert(vec2, IVector::NORM::NORM_INF, TOLERANCE);
    set1->insert(vec3, IVector::NORM::NORM_INF, TOLERANCE);

    ISet * set2 = ISet::createSet(pLogger);
    double data4[] = {9.0, -2.0};
    IVector * vec4 = IVector::createVector(2, data4, pLogger);
    double data5[] = {0.0, 12.0};
    IVector * vec5 = IVector::createVector(2, data5, pLogger);
    double data6[] = {40.0, 1.0};
    IVector * vec6 = IVector::createVector(2, data6, pLogger);
    set2->insert(vec4, IVector::NORM::NORM_INF, TOLERANCE);
    set2->insert(vec5, IVector::NORM::NORM_INF, TOLERANCE);
    set2->insert(vec6, IVector::NORM::NORM_INF, TOLERANCE);

    ISet * set3 = ISet::add(set1, set2, IVector::NORM::NORM_INF, TOLERANCE, pLogger);

    IVector * vec_get;
    bool res = false;

    vec4->setCoord(1, -1.99);
    set3->get(vec_get, vec4, IVector::NORM::NORM_INF, 0.1);
    vec4->setCoord(1, -2);

    IVector::equals(vec_get, vec4, IVector::NORM::NORM_INF, 0.1, &res, pLogger);
    if (res == false) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;

        delete vec_get;

        return false;
    }

    vec2->setCoord(1, 1.99);
    set3->get(vec_get, vec2, IVector::NORM::NORM_INF, 0.1);
    vec2->setCoord(1, 2);

    IVector::equals(vec_get, vec2, IVector::NORM::NORM_INF, 0.1, &res, pLogger);
    if (res == false) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;

        delete vec_get;

        return false;
    }


    delete set1;
    delete vec1;
    delete vec2;
    delete vec3;

    delete set2;
    delete vec4;
    delete vec5;
    delete vec6;

    delete vec_get;

    return true;
}
bool testIntersect(ILogger * pLogger) {
    ISet * set1 = ISet::createSet(pLogger);
    double data1[] = {11.0, 2.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    double data2[] = {1.0, 2.0};
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    double data3[] = {4.0, 7.0};
    IVector * vec3 = IVector::createVector(2, data3, pLogger);
    set1->insert(vec1, IVector::NORM::NORM_INF, TOLERANCE);
    set1->insert(vec2, IVector::NORM::NORM_INF, TOLERANCE);
    set1->insert(vec3, IVector::NORM::NORM_INF, TOLERANCE);

    ISet * set2 = ISet::createSet(pLogger);
    double data4[] = {9.0, -2.0};
    IVector * vec4 = IVector::createVector(2, data4, pLogger);
    double data5[] = {1.01, 1.99};
    IVector * vec5 = IVector::createVector(2, data5, pLogger);
    double data6[] = {40.0, 1.0};
    IVector * vec6 = IVector::createVector(2, data6, pLogger);
    set2->insert(vec4, IVector::NORM::NORM_INF, TOLERANCE);
    set2->insert(vec5, IVector::NORM::NORM_INF, TOLERANCE);
    set2->insert(vec6, IVector::NORM::NORM_INF, TOLERANCE);

    ISet * set3 = ISet::intersect(set1, set2, IVector::NORM::NORM_INF, 0.1, pLogger);

    if (set3->getSize() != 1) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;

        return false;
    }

    IVector * vec_get;
    bool res = false;

    set3->get(vec_get, vec5, IVector::NORM::NORM_INF, 0.1);

    IVector::equals(vec_get, vec5, IVector::NORM::NORM_INF, 0.1, &res, pLogger);
    if (res == false) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;

        delete vec_get;

        return false;
    }
    delete vec_get;
    set3->get(vec_get, vec3, IVector::NORM::NORM_INF, TOLERANCE);
    if (vec_get != nullptr) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;


        return false;
    }


    delete set1;
    delete vec1;
    delete vec2;
    delete vec3;

    delete set2;
    delete vec4;
    delete vec5;
    delete vec6;


    return true;
}
bool testSub(ILogger * pLogger) {
    ISet * set1 = ISet::createSet(pLogger);
    double data1[] = {11.0, 2.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    double data2[] = {1.0, 2.0};
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    double data3[] = {4.0, 7.0};
    IVector * vec3 = IVector::createVector(2, data3, pLogger);
    set1->insert(vec1, IVector::NORM::NORM_INF, TOLERANCE);
    set1->insert(vec2, IVector::NORM::NORM_INF, TOLERANCE);
    set1->insert(vec3, IVector::NORM::NORM_INF, TOLERANCE);

    ISet * set2 = ISet::createSet(pLogger);
    double data4[] = {9.0, -2.0};
    IVector * vec4 = IVector::createVector(2, data4, pLogger);
    double data5[] = {1.01, 1.99};
    IVector * vec5 = IVector::createVector(2, data5, pLogger);
    double data6[] = {40.0, 1.0};
    IVector * vec6 = IVector::createVector(2, data6, pLogger);
    set2->insert(vec4, IVector::NORM::NORM_INF, TOLERANCE);
    set2->insert(vec5, IVector::NORM::NORM_INF, TOLERANCE);
    set2->insert(vec6, IVector::NORM::NORM_INF, TOLERANCE);

    ISet * set3 = ISet::sub(set1, set2, IVector::NORM::NORM_INF, 0.1, pLogger);

    if (set3->getSize() != 2) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;

        return false;
    }

    IVector * vec_get;
    bool res = false;

    set3->get(vec_get, vec1, IVector::NORM::NORM_INF, TOLERANCE);

    IVector::equals(vec_get, vec1, IVector::NORM::NORM_INF, 0.1, &res, pLogger);
    if (res == false) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;

        delete vec_get;

        return false;
    }
    delete vec_get;
    set3->get(vec_get, vec2, IVector::NORM::NORM_INF, TOLERANCE);
    if (vec_get != nullptr) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;


        return false;
    }
    delete vec_get;
    set3->get(vec_get, vec6, IVector::NORM::NORM_INF, TOLERANCE);
    if (vec_get != nullptr) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;

        delete vec_get;

        return false;
    }

    delete set1;
    delete vec1;
    delete vec2;
    delete vec3;

    delete set2;
    delete vec4;
    delete vec5;
    delete vec6;


    return true;
}
bool testSymSub(ILogger * pLogger) {
    ISet * set1 = ISet::createSet(pLogger);
    double data1[] = {11.0, 2.0};
    IVector * vec1 = IVector::createVector(2, data1, pLogger);
    double data2[] = {1.0, 2.0};
    IVector * vec2 = IVector::createVector(2, data2, pLogger);
    double data3[] = {4.0, 7.0};
    IVector * vec3 = IVector::createVector(2, data3, pLogger);
    set1->insert(vec1, IVector::NORM::NORM_INF, TOLERANCE);
    set1->insert(vec2, IVector::NORM::NORM_INF, TOLERANCE);
    set1->insert(vec3, IVector::NORM::NORM_INF, TOLERANCE);

    ISet * set2 = ISet::createSet(pLogger);
    double data4[] = {9.0, -2.0};
    IVector * vec4 = IVector::createVector(2, data4, pLogger);
    double data5[] = {1.01, 1.99};
    IVector * vec5 = IVector::createVector(2, data5, pLogger);
    double data6[] = {40.0, 1.0};
    IVector * vec6 = IVector::createVector(2, data6, pLogger);
    set2->insert(vec4, IVector::NORM::NORM_INF, TOLERANCE);
    set2->insert(vec5, IVector::NORM::NORM_INF, TOLERANCE);
    set2->insert(vec6, IVector::NORM::NORM_INF, TOLERANCE);

    ISet * set3 = ISet::symSub(set1, set2, IVector::NORM::NORM_INF, 0.1, pLogger);

    if (set3->getSize() != 4) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;

        return false;
    }

    IVector * vec_get;
    bool res = false;

    set3->get(vec_get, vec1, IVector::NORM::NORM_INF, TOLERANCE);

    IVector::equals(vec_get, vec1, IVector::NORM::NORM_INF, 0.1, &res, pLogger);
    if (res == false) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;

        delete vec_get;

        return false;
    }
    delete vec_get;
    set3->get(vec_get, vec2, IVector::NORM::NORM_INF, TOLERANCE);
    if (vec_get != nullptr) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;

        delete vec_get;

        return false;
    }

    set3->get(vec_get, vec6, IVector::NORM::NORM_INF, TOLERANCE);
    if (vec_get == nullptr) {
        delete set1;
        delete vec1;
        delete vec2;
        delete vec3;

        delete set2;
        delete vec4;
        delete vec5;
        delete vec6;


        return false;
    }

    delete set1;
    delete vec1;
    delete vec2;
    delete vec3;

    delete set2;
    delete vec4;
    delete vec5;
    delete vec6;

    delete vec_get;

    return true;
}


int main()
{
    int for_log = 5;
    ILogger * log = ILogger::createLogger(&for_log);
    if (testCreateGet(log)) {
        std::cout << "Test Create Get | OK" << std::endl;
    } else {
        std::cout << "Test Create Get | FAIL" << std::endl;
    }
    if (testErase(log)) {
        std::cout << "Test Erase | OK" << std::endl;
    } else {
        std::cout << "Test Erase | FAIL" << std::endl;
    }
    if (testAdd(log)) {
        std::cout << "Test Add | OK" << std::endl;
    } else {
        std::cout << "Test Add | FAIL" << std::endl;
    }
    if (testIntersect(log)) {
        std::cout << "Test Intersect | OK" << std::endl;
    } else {
        std::cout << "Test Intersect | FAIL" << std::endl;
    }
    if (testSub(log)) {
        std::cout << "Test Sub | OK" << std::endl;
    } else {
        std::cout << "Test Sub | FAIL" << std::endl;
    }
    if (testSymSub(log)) {
        std::cout << "Test Sym Sub | OK" << std::endl;
    } else {
        std::cout << "Test Sym Sub | FAIL" << std::endl;
    }
    std::cout << "Enter any number to exit:" << std::endl;
    std::cin >> for_log;
    log->destroyLogger(&for_log);
    return 0;
}
