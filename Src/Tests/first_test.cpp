#include <gtest/gtest.h>

extern "C" {
#include "URTOS/Memory/urtos_memory.h"
#include "URTOS/urtos.h"
}

TEST(MyFirstTestClass, TestPasses) {
	SUCCEED();
}

TEST(MyFirstTestClass, Test2Passes) {
	SUCCEED();
}


TEST(MyFirstTestClass, ThisTestFails) {
	URTOS_Init();

	FAIL();
}
