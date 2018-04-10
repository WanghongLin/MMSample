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


/**
 * Given an array of integers sorted in ascending order, find the starting and ending position of a given target value.
 *
 * Your algorithm's runtime complexity must be in the order of O(log n).
 *
 * If the target is not found in the array, return [-1, -1].
 *
 * For example,
 *
 * Given [5, 7, 7, 8, 8, 10] and target value 8,
 *
 * return [3, 4].
 */

#include <algorithm>
#include <iostream>
#include <ctime>

using namespace std;

vector<int> search_for_a_range(const vector<int>& v, int target, int low, int high)
{
    if (high >= low) {
        int m = (high+low) / 2;

        if (target > v[m]) {
            return search_for_a_range(v, target, m+1, high);
        } else if (target < v[m]) {
            return search_for_a_range(v, target, low, m-1);
        } else {
            vector<int> vi;
            for (int i = m-1; i >= 0 && v[i] == target; i--) {
                vi.push_back(i);
            }
            vi.push_back(m);
            for (int j = m+1; j < v.size() && v[j] == target; j++) {
                vi.push_back(j);
            }
            return vi;
        }
    } else {
        vector<int> vi;
        vi.push_back(-1);
        return vi;
    }
}

ostream& operator<<(ostream& os, vector<int>& vi)
{
    for (auto i : vi) {
        os << i << ' ';
    }
    return os;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <size-of-sorted-list> <target>\n";
        std::exit(0);
    }

    auto size = static_cast<int>(std::strtol(argv[1], nullptr, 10));
    auto target = static_cast<int>(std::strtol(argv[2], nullptr, 10));

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    vector<int> v;

    for (int i = 0; i < size; i++) {
        v.push_back(std::rand() % size);
    }

    std::sort(v.begin(), v.end());
    std::cout << v << std::endl;

    vector<int> range = search_for_a_range(v, target, 0, static_cast<int>(v.size() - 1));
    std::cout << range << endl;

    return 0;
}
