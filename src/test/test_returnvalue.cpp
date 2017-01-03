#include "gv/util/returnvalue.hpp"
#include <string>
#include <iostream>
#include <cassert>

using namespace std;
using namespace gv::util;

class Initializer {
public:
    Initializer() {
        Status::record(-1, "Error [-1]");
        Status::record(1, "Error [1]");
    }
} static instance;


string ok_string = "hi, this is the returned string";
StatusRet<string> returns_ok_string() {
    return StatusRet<string>::ok(ok_string);
}

int minus1_int = 82;
StatusRet<int> returns_minus1_int() {
    return StatusRet<int>(Status::by_code(-1), minus1_int);
}

int plus1_bool = true;
StatusRet<bool> returns_plus1_bool() {
    return StatusRet<bool>(Status::by_code(1), plus1_bool);
}

int main() {
    {
        StatusRet<string> r = returns_ok_string();
        assert(r.status().is_ok());
        assert(r.value() == ok_string);
        cout << "OK_STRING - pass" << endl;
    }

    {   
        StatusRet<int> r = returns_minus1_int();
        assert(r.status().is_system_error());
        assert(r.value() == minus1_int);
        cout << "M1_INT    - pass" << endl;
    }

    {   
        StatusRet<bool> r = returns_plus1_bool();
        assert(r.status().is_business_error());
        assert(r.value() == plus1_bool);
        cout << "P1_BOOL   - pass" << endl;
    }

    {
        assert( Status::by_code(-1) == -1 );
        assert( Status::by_code(-2) == Status::unknown() );
        cout << "REGISTER  - pass" << endl;
    }
    {
        Status s(15, "My Custom Status");
        assert (s==15);
        assert (s.description() == "My Custom Status");
        cout << "CUSTOM    - pass" << endl;
    }
}
