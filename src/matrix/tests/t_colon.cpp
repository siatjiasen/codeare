#include "Matrix.hpp"
#include "Creators.hpp"
#include "Print.hpp"

int main (int narg, char** argv) {
    Matrix<float> A = randn<float>(3,4);

    
    std::cout << "A" << std::endl;
    std::cout << A << std::endl;
    std::cout << "A(\":\")" << std::endl;
    std::cout << A(":") << std::endl;
    std::cout << "A(Range())" << std::endl;
    std::cout << A(Range()) << std::endl;

    std::cout << "A(\"0:6\")" << std::endl;
    std::cout << A("0:6") << std::endl;
    std::cout << "A(\"3:8\")" << std::endl;
    std::cout << A("3:8") << std::endl;
    std::cout << "A(\"3:3\")" << std::endl;
    std::cout << A("3:3") << std::endl;
    std::cout << "A(\"6:end\")" << std::endl;
    std::cout << A("6:end-3") << std::endl;

    std::cout << "A(\"0,0:2\")" << std::endl;
    std::cout << A("0,0:2") << std::endl;
    std::cout << "A(\"0:1,0:2\")" << std::endl;
    std::cout << A("0:1,0:2") << std::endl;
    std::cout << "A(\"1:1,0:2\")" << std::endl;
    std::cout << A("1:1,0:2") << std::endl;
    std::cout << "A(\"1:1,:\")" << std::endl;
    std::cout << A("1:1,:") << std::endl;
    std::cout << "A(\"0:2,2:2\")" << std::endl;
    std::cout << A("0:2,2:2") << std::endl;
    std::cout << "A(\":,2:2\")" << std::endl;
    std::cout << A(":,2:2") << std::endl;
    std::cout << "A(\":,1:2\")" << std::endl;
    std::cout << A(":,1:2") << std::endl;

    std::cout << "A(Range(2,1,3))" << std::endl;
    std::cout << A(Range(2,1,3)) << std::endl;
    std::cout << "A(Range(0,2),Range(2,2))" << std::endl;
    std::cout << A(Range(0,2),Range(2,2)) << std::endl;
    std::cout << "A(Range(0,2),Range(2))" << std::endl;
    std::cout << A(Range(0,2),Range(2)) << std::endl;
    std::cout << "A(Range(),Range(3,-1,2))" << std::endl;
    std::cout << A(Range(),Range(3,-1,2)) << std::endl;

    //Matrix<float> B;
    //copy (A(Range(2,1,3)), B(Range(2,1,3)));


    /*std::cout << "A(\"3:-1:2\")" << std::endl;
    std::cout << A("3:-1:2") << std::endl;
    std::cout << "A(\"0:1:end\")" << std::endl;
    std::cout << A("0:1:end") << std::endl;
    std::cout << "A(\"end:-1:0\")" << std::endl;
    std::cout << A("end:-1:0") << std::endl;*/
    
    return 0;
}