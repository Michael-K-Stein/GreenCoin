
#include <iostream>


extern "C" void main_c(int argc, char ** argv); // one way

int main(int argc, char ** argv)
{
	main_c(argc, argv);
}