#include <iostream>
#include "../src/duckx.hpp"
using namespace std;

int main() {
    duckx::Document doc("/home/semir/Programme/DuckX_Fork/DuckX/samples/my_test.docx");
    doc.open();

    for (auto p : doc.paragraphs()) {
        for (auto r : p.runs()) {
            cout << r.get_text() << endl;
        }
    }

    return 0;
}
