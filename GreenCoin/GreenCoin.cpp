
#include <iostream>


extern "C" void main_c(); // one way

int main()
{
	main_c();
    std::cout << "Hello World!\n";
}