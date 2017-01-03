#include "gv/util/string_fmt.hpp"
#include "gv/util/to_string.hpp"


#include <iostream>
#include <cassert>

using namespace std;
using namespace gv::util;


int main() {
    {
        assert( format("A=%d", 54) == "A=54" );;
        cout << "INT_FORMAT        - pass" << endl;
    }

    {
        constexpr const char* fmt = "A=%d";
        assert( format_static<length(fmt)*2>("A=%d", 54) == "A=54" );
        cout << "INT_FORMAT_STATIC - pass" << endl;
    }

}
