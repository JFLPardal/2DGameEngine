#include "pch.h"

#include "ECS/ECS.h"

#include "Systems/MovementSystem.h"

class MockTest
{
public:
	MOCK_METHOD(void, SomeMethod, ());
};

struct asfs : ::testing::Test
{
	Registry registry;
	int f;
	asfs()
	{
		f = 5;
		registry = Registry{};
	}
};

TEST(TestCaseName, TestName2)
{
	MockTest mock;
	EXPECT_CALL(mock, SomeMethod);
	mock.SomeMethod();
	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}

TEST_F(asfs, emptyRegistryHasNoMovementSystem)
{
	EXPECT_FALSE(registry.HasSystem<MovementSystem>());
}