//
//  Copyright (c) 2018 mutter
//
//  This file is part of MMSample.
//
//  MMSample is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  MMSample is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with MMSample.  If not, see <http://www.gnu.org/licenses/>.
//  
//
// Created by mutter on 4/11/18.
//

#include <iostream>
#include <vector>

using namespace std;

vector< vector<int> > combination(vector<int>& input_v, int m)
{
    if (m == 1) {
        vector< vector<int> > vvi;
        for (auto i : input_v) {
            vector<int> vi;
            vi.push_back(i);
            vvi.push_back(vi);
        }
        return vvi;
    }

    vector< vector<int> > vvi;

    for (int i = 0; i < input_v.size(); i++) {
        int first = input_v[i];
        auto smaller_input_v = vector<int>(input_v.begin() + i + 1, input_v.end());
        auto smaller_vvi = combination(smaller_input_v, m - 1);
        for (auto& vi : smaller_vvi) {
            vi.push_back(first);
            vvi.push_back(vi);
        }
    }

    return vvi;
}

ostream& operator<<(ostream& os, vector<int> vi)
{
    for (auto i : vi) {
        os << i << ' ';
    }
    os << '\n';
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << "<N> <M>\n";
        std::exit(0);
    }

    auto N = static_cast<int>(std::strtol(argv[1], nullptr, 10));
    auto M = static_cast<int>(std::strtol(argv[2], nullptr, 10));

    if (N < M) {
        std::cout << "Invalid input, N >= M\n";
    }

    vector<int> v;
    for (int i = 0; i < N; ++i) {
        v.push_back(i+1);
    }

    auto result = combination(v, M);
    for (auto& vi : result) {
        std::cout << vi;
    }
    std::cout << "Total combinations " << result.size() << std::endl;
    return 0;
}
