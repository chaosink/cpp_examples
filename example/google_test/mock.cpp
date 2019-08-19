#include <gmock/gmock.h>
using namespace testing;

class DifficultCollaborator {
public:
    virtual bool calculate(int * /*result*/) {
        throw 1;
    }
};

class Target {
public:
    int Execute(DifficultCollaborator *calculator) {
        int i;
        if(!calculator->calculate(&i))
            return 0;
        return i;
    }
};

/*----------------------------------------*/

class DifficultCollaboratorMock : public DifficultCollaborator {
public:
    MOCK_METHOD1(calculate, bool(int *));
};

TEST(ATarget, ReturnsAnAmountWhenCalculatePasses) {
    DifficultCollaboratorMock difficult;
    Target calc;
    EXPECT_CALL(difficult, calculate(_))
        .WillOnce(DoAll(
            SetArgPointee<0>(3), // set the value after the func returns
            Return(true)));

    auto result = calc.Execute(&difficult);

    ASSERT_THAT(result, Eq(3));
}

int main(int argc, char **argv) {
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
