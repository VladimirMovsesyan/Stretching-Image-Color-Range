#include <iostream>
#include <fstream>
#include <cmath>
#include <omp.h>
#include <iomanip>

using namespace std;

typedef long double ld;
typedef unsigned char uchar;

int temp[256], threads = 1;

// getting data start position
inline int get_start(const char* a, int length) {
    int cnt = 0;
    for (int i = 0; i < length; i++) {
        if (a[i] == '\n')
            cnt++;
        if (cnt == 4)
            return i + 1;
    }
    return 0;
}

// finding arr[k] of sorted array
int kOrd(const int* arr, int k) {
    for (int i = 0; i < 256; i++) {
        k -= arr[i];
        if (k <= 0) {
            return i;
        }
    }
    return 0;
}

// stretching color range and making contrast coefficient
inline void normalize(char* a, int start, int length, int k) {
    omp_set_num_threads(threads);
    uchar mn = a[start], mx = a[start];

#pragma omp parallel for schedule(static, 1)
    for (int i = start; i < length; i++) {
        temp[(uchar)a[i]]++;
    }

    uchar leftBound, rightBound;

#pragma omp sections
    {
#pragma omp section
        {
            leftBound = kOrd(temp, k);
        }
#pragma omp section
        {
            rightBound = kOrd(temp, (length - start) - k);
        }
    }

#pragma omp parallel for schedule(static, 1)
    for (int i = start; i < length; i++) {
        if (leftBound < (uchar)a[i] && (uchar)a[i] < rightBound) {
            mn = min(mn, (uchar)a[i]);
            mx = max(mx, (uchar)a[i]);
        }
    }
#pragma omp parallel for schedule(static, 1)
    for (int i = start; i < length; i++) {
        if (leftBound < (uchar)a[i] && (uchar)a[i] < rightBound) {
            a[i] = ld((uchar)a[i] - mn) / ld(mx - mn) * 255.0L;
        } else if (leftBound >= (uchar)a[i]) {
            a[i] = 0;
        } else if ((uchar)a[i] >= rightBound) {
            a[i] = 255;
        }
    }
}

int main(int args, char* argv[]) {
    ifstream in;
    ofstream out;

    // read count of threads
    threads = stoi(argv[1]);

    // setting count of treads
    omp_set_num_threads(threads);

    // read coefficient parameter
    double k = stof(argv[4]);


    //reading file
    in.exceptions(ifstream::failbit);
    in.open(argv[2], ifstream::binary);
    in.seekg(0, in.end);
    int length = in.tellg();
    in.seekg(0, in.beg);
    char* a = new char[length];
    in.read(a, length);
    in.close();

    // getting start global time
    double startTime = omp_get_wtime();

    // getting start position
    int start = get_start(a, length);

    // stretching color range and making contrast coefficient
    normalize(a, start, length, k * (length - start));

    // getting end time
    double endTime = omp_get_wtime();

    // printing algo time
    printf("Time (%i thread(s)): %g ms\n", threads, (endTime - startTime) * 1000);

    // printing out image data
    out.open(argv[3], ofstream::binary);
    for (int i = 0; i < length; i++) {
        out << a[i];
    }
    out.close();

    return 0;
}