#include "./log/Log4cpp.hpp"
#include <iostream>
using namespace std;
int main(int argc, char const *argv[]) 
{
	int number = 10;
	const char * pstr = "hello,world";

    printf("hello,world %d%s\n", number, pstr);

    LogError("hello, world, %d,%p %s %d\n",
             number, &number, pstr, number);

	return 0;
} 
