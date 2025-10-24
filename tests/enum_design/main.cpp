#include "enum_simple.cpp"
#include "enum_wrap.cpp"
#include "enum_crtp.cpp"

int main()
{
	simple::test();
	wrap::test();
	crtp::test();
}
