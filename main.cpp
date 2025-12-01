#include <iostream>
#include <thread>
#include <barrier>
#include <syncstream>

using namespace std;

constexpr int nt = 4;

void f(char set_name, int index) {
    osyncstream out(cout);
    out << "From set " << set_name << " action " << index << " executed.\n";
}

void run_range(char set_name, int from, int to) {
    for (int i = from; i <= to; ++i) {
        f(set_name, i);
    }
}

int main() {
    {
        osyncstream out(cout);
        out << "Computation started.\n";
    }

    static barrier sync_point(nt);

    auto worker = [](int id) {
        if (id == 0) {
            run_range('a', 1, 4);
        }
        else if (id == 1) {
            run_range('a', 5, 6);
            run_range('b', 1, 2);
        }
        else if (id == 2) {
            run_range('b', 3, 6);
        }
        else if (id == 3) {
            run_range('b', 7, 9);
        }

        sync_point.arrive_and_wait();

        if (id == 0) {
            run_range('c', 1, 8);
            run_range('d', 1, 1);
        }
        else if (id == 1) {
            run_range('d', 2, 7);
            run_range('e', 1, 3);
        }
        else if (id == 2) {
            run_range('e', 4, 5);
            run_range('f', 1, 7);
        }
        else if (id == 3) {
            run_range('g', 1, 5);
            run_range('h', 1, 4);
        }

        sync_point.arrive_and_wait();

        if (id == 0) {
            run_range('i', 1, 4);
        }
        else if (id == 1) {
            run_range('i', 5, 8);
        }
        else if (id == 2) {
            run_range('i', 9, 9);
            run_range('j', 1, 3);
        }
        else if (id == 3) {
            run_range('j', 4, 7);
        }
        };

    {
        jthread t0(worker, 0);
        jthread t1(worker, 1);
        jthread t2(worker, 2);
        jthread t3(worker, 3);
    }

    {
        osyncstream out(cout);
        out << "Computation finished.\n";
    }

    return 0;
}
