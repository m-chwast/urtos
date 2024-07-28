#include <gtest/gtest.h>

extern "C" {
#include "Memory/urtos_memory.h"
#include "urtos.h"
}

TEST(MyFirstTestClass, TestPasses) {
	SUCCEED();
}

TEST(MyFirstTestClass, ThisTestFails) {
	URTOS_Init();

	FAIL();
}
