#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Sample test to verify GoogleTest integration
TEST(SampleTest, BasicAssertion) {
    EXPECT_EQ(7 * 6, 42);
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
}

TEST(SampleTest, StringTest) {
    std::string hello = "Hello";
    std::string world = "World";
    std::string result = hello + " " + world;
    
    EXPECT_EQ(result, "Hello World");
    EXPECT_NE(result, "Goodbye World");
}

// Sample mock test
class MockInterface {
public:
    virtual ~MockInterface() = default;
    virtual int getValue() const = 0;
    virtual void setValue(int value) = 0;
};

class MockImplementation : public MockInterface {
public:
    MOCK_METHOD(int, getValue, (), (const, override));
    MOCK_METHOD(void, setValue, (int value), (override));
};

TEST(MockTest, BasicMocking) {
    MockImplementation mock;
    
    EXPECT_CALL(mock, getValue())
        .WillOnce(testing::Return(42));
    
    EXPECT_CALL(mock, setValue(testing::_))
        .Times(1);
    
    EXPECT_EQ(mock.getValue(), 42);
    mock.setValue(100);
} 