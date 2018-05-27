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
// Created by mutter on 5/27/18.
//

/**
 * Given a set of distinct integers, nums, return all possible subsets (the power set).
 *
 * Note: The solution set must not contain duplicate subsets.
 *
 * Example:
 *
 * Input: nums = [1,2,3]
 * Output:
 *
 * [
 * [3],
 * [1],
 * [2],
 * [1,2,3],
 * [1,3],
 * [2,3],
 * [1,2],
 * []
 * ]
 */

#include <iostream>
#include <vector>

using namespace std;

static void pvv(vector< vector<int> >&& vv)
{
    for (auto& v : vv) {
        cout << '[';

        for (auto i = v.begin(); i != v.end(); i++) {
            cout << *i;
            if (i != v.end()-1) {
                cout << ", ";
            }
        }

        cout << ']' << endl;
    }
}

static vector< vector<int> > combinations(vector<int>& v, int should_take_n_element_from_vector)
{
    if (should_take_n_element_from_vector == v.size()) {
        vector< vector<int> > vv;
        vector<int> vi = vector<int>(v);
        vv.push_back(vi);
        return vv;
    }

    if (should_take_n_element_from_vector == 1) {
        vector< vector<int> > vv;
        for (int i : v) {
            vector<int> vi;
            vi.push_back(i);
            vv.push_back(vi);
        }
        return vv;
    }

    if (should_take_n_element_from_vector == 0) {
        vector< vector<int> > vv;
        vector<int> vi;
        vv.push_back(vi);
        return vv;
    }

    vector<int> sub_vector = vector<int>(v);
    int first = sub_vector[0];
    sub_vector.erase(sub_vector.begin());
    auto others = combinations(sub_vector, should_take_n_element_from_vector-1);

    for (vector<int>& ov : others) {
        ov.push_back(first);
    }

    return others;
}

int main(int argc, char *argv[])
{
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);

    for (int i = 0; i <= v.size(); ++i) {
        pvv(combinations(v, i));
    }

    return 0;
}

