#include "util/returnvalue.hpp"
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

StatusRet<string> returns_ok() {
    return StatusRet<string>::ok("hi, this is the returned string");
}


void test_status_code(string expected, int code) {
    const Status& st = Status::by_code(code);
    if (st != Status::unknown()) {
        cout << expected << " found, code: " << st.code()
             << ", description: " << st.description() << endl;
    } else {
        cout << expected << " not found!" << endl;
    }

}

int main() {

    StatusRet<string> r = returns_ok();
    cout << "Returned code: " << r.status() //.code 
         << ", description: " << r.status().description()
         << ", value: " << r.value() << endl;

    test_status_code("Status[-1]", -1);
    test_status_code("Status[-2]", -2);

    Status s(15, "My Custom Status");
    assert (s==15);
    assert (s.description() == "My Custom Status");
    cout << "Custom status works!" << endl;
}
