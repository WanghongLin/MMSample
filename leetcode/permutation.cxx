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
// Created by mutter on 4/10/18.
//

#include <iostream>
#include <vector>

using namespace std;

vector< vector<int> > arrangement(vector<int> vec, int m)
{

    if (m == 1) {
        vector< vector<int> > vv;

        for (int it : vec) {
            vector<int> v;
            v.push_back(it);
            vv.push_back(v);
        }

        return vv;
    }

    vector< vector<int> > vv;

    for (auto it = vec.begin(); it != vec.end(); it++) {
        vector<int> v = vector<int>(vec);

        int e = *it;

        for (auto iit = v.begin(); iit != v.end(); iit++) {
            if (*iit == e) {
                v.erase(iit);
                break;
            }
        }

        vector< vector<int> > tvv = arrangement(v, m-1);

        for (auto& itv : tvv) {
            itv.push_back(e);
            vv.push_back(itv);
        }
    }

    return vv;
}


int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <N> <M>\n";
        std::exit(0);
    }

    auto n = (int)std::strtol(argv[1], nullptr, 10);
    auto m = (int)std::strtol(argv[2], nullptr, 10);

    vector<int> v;
    for (int i = 1; i <= n; i++) {
        v.push_back(i);
    }

    vector< vector<int> > vvi = arrangement(v, m);

    for(auto& i : vvi) {
        for (auto j : i) {
            cout << j << ' ';
        }
        cout << '\n';
    }

    cout << "total permutation " << vvi.size() << endl;
    return 0;
}
