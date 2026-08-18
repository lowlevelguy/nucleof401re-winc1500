#include <unity.h>

void setUp(void) {}

void tearDown(void) {}

static void test_smoke_trivial(void) {
	TEST_ASSERT_TRUE(1);
}

int main(void) {
	UNITY_BEGIN();

	RUN_TEST(test_smoke_trivial);

	return UNITY_END();
}
