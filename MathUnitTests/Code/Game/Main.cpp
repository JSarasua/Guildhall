//-----------------------------------------------------------------------------------------------
// Math Unit Tests: Main.cpp (version 4.1) - for SMU Guildhall assignments MP1-A1 through MP1-A8
//
#include "Game/UnitTests_MP1A1.hpp"
#include "Game/UnitTests_MP1A2.hpp"	// Uncomment this line after adding the MP1-A2 test code
#include "Game/UnitTests_MP1A3.hpp"	// Uncomment this line after adding the MP1-A3 test code
#include "Game/UnitTests_MP1A4.hpp"	// Uncomment this line after adding the MP1-A4 test code
#include "Game/UnitTests_MP1A5.hpp"	// Uncomment this line after adding the MP1-A5 test code
// #include "Game/UnitTests_MP1A6.hpp"	// Uncomment this line after adding the MP1-A6 test code
// #include "Game/UnitTests_MP1A7.hpp"	// Uncomment this line after adding the MP1-A7 test code
#include "Game/UnitTests_Custom.hpp"	// Uncomment this line after adding the custom tests file
#include "Game/GameCommon.hpp"
#include <stdio.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------------------------
void RunTestSets()
{
	RunTests_MP1A1();
	RunTests_MP1A2();	// Uncomment this line after adding the MP1-A2 test code
	RunTests_MP1A3();	// Uncomment this line after adding the MP1-A3 test code
 	RunTests_MP1A4();	// Uncomment this line after adding the MP1-A4 test code
 	RunTests_MP1A5();	// Uncomment this line after adding the MP1-A5 test code
// 	RunTests_MP1A6();	// Uncomment this line after adding the MP1-A6 test code
// 	RunTests_MP1A7();	// Uncomment this line after adding the MP1-A7 test code
	RunTests_Custom();	// Uncomment this line after adding the custom tests file
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//		DO NOT MODIFY ANY CODE BELOW HERE WITHOUT EXPRESS PERMISSION FROM YOUR PROFESSOR
//		(as doing so will be considered cheating and have serious academic consequences)
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
// Global variables for test result tracking
//
int g_numTotalTestsPassed			= 0;
int g_numTotalTestsFailed			= 0;
int g_numTotalTestsExpected			= 0;
int g_numTotalTestsSkipped			= 0;

int g_numGradedTestsPassed			= 0;
int g_numGradedTestsFailed			= 0;
int g_numGradedTestsExpected		= 0;
int g_numGradedTestsSkipped			= 0;

int g_numNonGradedTestsPassed		= 0;
int g_numNonGradedTestsFailed		= 0;
int g_numNonGradedTestsExpected		= 0;
int g_numNonGradedTestsSkipped		= 0;


//-----------------------------------------------------------------------------------------------
void VerifyTestResult( bool isCorrect, const char* testName )
{
	if( isCorrect )
	{
		++ g_numTotalTestsPassed;
	}
	else
	{
		++ g_numTotalTestsFailed;
		printf( "\n  TEST FAILED: %s", testName );
	}
}


//-----------------------------------------------------------------------------------------------
void RunTestSet( bool isGraded, TestSetFunctionType testSetFunction, const char* testSetName )
{
	const char* gradedText = isGraded ? "graded" : "non-graded";
	printf( "Running %s test set \"%s\"... ", gradedText, testSetName );

	int numTestsPassedBefore		= g_numTotalTestsPassed;
	int numTestsFailedBefore		= g_numTotalTestsFailed;

	// RUN THE ACTUAL TEST SET; tests modify g_numTotalTestsPassed & Failed directly
	int numTestsExpected = testSetFunction();

	int numTestsJustPassed = g_numTotalTestsPassed - numTestsPassedBefore;
	int numTestsJustFailed = g_numTotalTestsFailed - numTestsFailedBefore;
	int numTestsJustTried = numTestsJustPassed + numTestsJustFailed;
	int numTestsJustSkipped = numTestsExpected - numTestsJustTried;

	// Check for error in test set reporting (expected vs. attempted # of tests, etc.)
	bool wasError = false;
	if( numTestsJustSkipped < 0 )
	{
		wasError = true;
		numTestsJustSkipped = 0;
	}
	else if( numTestsJustTried > 0 && numTestsJustTried != numTestsExpected )
	{
		wasError = true;
	}

	if( wasError )
	{
		printf( "\n\n" );
		printf( "########################################################################################\n");
		printf( "ERROR: Test set \"%s\"\n", testSetName );
		printf( "  said it expected %i test(s), but it actually ran %i test(s)!\n", numTestsExpected, numTestsJustTried );
		printf( "  Please change the return value for that test set function from %i to %i.\n", numTestsExpected, numTestsJustTried );
		printf( "########################################################################################\n");
	}
	
	// Update global test stats based on this test set's results
	g_numTotalTestsSkipped += numTestsJustSkipped;
	if( isGraded )
	{
		g_numGradedTestsExpected += numTestsExpected;
		g_numGradedTestsPassed += numTestsJustPassed;
		g_numGradedTestsFailed += numTestsJustFailed;
		g_numGradedTestsSkipped += numTestsJustSkipped;
	}
	else
	{
		g_numNonGradedTestsExpected += numTestsExpected;
		g_numNonGradedTestsPassed += numTestsJustPassed;
		g_numNonGradedTestsFailed += numTestsJustFailed;
		g_numNonGradedTestsSkipped += numTestsJustSkipped;
	}

	// Report test set summary; for actual failures, skip this part, since each failure prints its own message
	if( numTestsExpected == 0 )
	{
		printf( "(0 tests)" );
	}
	else if( numTestsJustSkipped > 0 )
	{
		printf( "test set DISABLED; %i tests SKIPPED", numTestsJustSkipped );
	}
	else if( numTestsJustFailed == 0 )
	{
		printf( "all %i tests passed", numTestsJustPassed );
	}

	printf( "\n" );
}


//-----------------------------------------------------------------------------------------------
int main( int, char** )
{
	// Run test sets
	printf( "Running math library unit tests:\n\n" );
	RunTestSets();

	// Report results
	int pointsOffPerIncorrectTest = 2;
	int numGradedIncorrect = g_numGradedTestsFailed + g_numGradedTestsSkipped;
	int assignmentGradePenalty = pointsOffPerIncorrectTest * numGradedIncorrect;
	int assignmentGrade = 100 - assignmentGradePenalty;
	if( assignmentGrade < 0 )
	{
		assignmentGrade = 0;
	}

	printf( "\n" );
	printf( "========================================================================================\n");
	printf( "  Number of GRADED tests passed: %i of %i (%i failed, %i skipped)\n", g_numGradedTestsPassed, g_numGradedTestsExpected, g_numGradedTestsFailed, g_numGradedTestsSkipped );
	printf( "  Number of non-graded tests passed: %i of %i (%i failed, %i skipped)\n", g_numNonGradedTestsPassed, g_numNonGradedTestsExpected, g_numNonGradedTestsFailed, g_numNonGradedTestsSkipped );
	printf( "\n" );
	printf( "  Assignment grade is calculated as: 100 minus %i point(s) per incorrect graded test.\n", pointsOffPerIncorrectTest );
	printf( "\n" );
	printf( "  Grade for assignment: 100 - (%i incorrect x %i-point penalty each) = %i%%\n", numGradedIncorrect, pointsOffPerIncorrectTest, assignmentGrade );
	printf( "========================================================================================\n");
	printf( "\n" );

//	system( "pause" ); // block and wait for keypress; no longer needed since VS2019 now automatically pauses
	return 0;
}


