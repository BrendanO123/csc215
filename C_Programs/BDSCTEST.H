#include <stdio.h>

unsigned tests;
unsigned passedTests;
unsigned failedTests;

unsigned asserts;
unsigned passAsserts;
unsigned failedAsserts;

unsigned currentAssertNumber;
unsigned failedCurrentTest;

unsigned firstTest;

START_TESTING(file)
char* file; 
{
    tests = 0;
    passedTests = 0;
    failedTests = 0;

    asserts = 0;
    passAsserts = 0;
    failedAsserts = 0;

    failedCurrentTest = 0;
    currentAssertNumber = 0;
    firstTest = 1;
    printf("\n\t\033[36m---\033[0m %s \033[36m---\033[0m\n", file);
}

incPass() {
    if (tests != 1 && !failedCurrentTest) {
        passedTests++;
    }
}

#define TEST_CASE(description)  printf("\n\t%s\n", description); tests++; incPass(); currentAssertNumber = 0; failedCurrentTest = 0; firstTest = 0;

ASSERT(boolExp) 
int boolExp; {
    if (boolExp)
        PASS();
    else
        FAIL();
}

#define ASSERT_EQU(val1, val2) if (val1 == val2) PASS(); else FAIL();

ASSERT_STR(strI, strJ) 
char *strI[], *strJ[]; {
    if (strcmp(strI, strJ) == 0) {
        PASS();
    } else {
        printf("\t%s != %s", strI, strJ);
        FAIL();
    }
}

PASS() {
    asserts++;
    currentAssertNumber++;
    passAsserts++;
    printf("\t\033[0;32mAssertion %d passed\033[0;37m.\n", currentAssertNumber);
}

FAIL() {
    asserts++;
    currentAssertNumber++;
    if (!failedCurrentTest) {
        failedCurrentTest = 1;
        failedTests++;
    }
    printf("\t\033[0;31mAssertion %d failed\033[0;37m.\n", currentAssertNumber);
    failedAsserts++;
}

END_TESTING() {
    incPass();

    printf("\n\t====================================\n"); 
    (tests == 1) ?  printf("\t1 test.\n") : printf("\t%d tests.\n", tests);
    printf("\t\033[0;32m%d passed\033[0;37m, \033[0;31m %d failed\033[0;37m.", passedTests, failedTests);

    (asserts == 1) ? printf("\n\n1 assertion.\n") : printf("\n\n\t%d assertions.\n", asserts);
    printf("\t\033[0;32m%d passed\033[0;37m, \033[0;31m %d failed\033[0;37m.", passAsserts, failedAsserts);
    printf("\n\t====================================\n");
}