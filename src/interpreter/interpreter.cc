/*
    interpreter.cc - General interpreter functions
    Copyright (C) 2020 Ethan Onstott

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "../storm.h"
#include "interpreter.h"

void allocateMemory(std::string::iterator loc) { // load data into memory
	loc++;

	while (*loc != 0x0B) {
		if (char(*loc - 0x80) == '[') { // move past name declaration, will be used later
			while (char(*loc - 0x80) != ']') loc++;
			loc += 2;
			interpreter.heap_ptrs.push_back(interpreter.heap.size());
		}

		
		interpreter.heap.push_back(*loc);

		loc++;
	}
	interpreter.heap_ptrs.push_back(interpreter.heap.size());
}

std::string getVal(std::string::iterator loc) {
	loc++;

	std::vector<int> num_list;
	int num = 0;

	while (char(*loc - 0x80) != ']') {
		// get integer representation of digit in char
		num_list.push_back((int)(char(*loc - 0x80) - '0') );
		loc++;
	}

	for (int n = 0; n < num_list.size(); n++)
		num += num_list[n] * (n + 1);

	std::string value = "";
	
	int i = interpreter.heap_ptrs[num];

	while(i < interpreter.heap_ptrs[num + 1]) {
		value += interpreter.heap[i] - 0x80;
		i++;
	}

	stripString(&value);
	return value;
}

void move(std::string::iterator loc) {
	loc++;
	
	// determine register
	int reg = *loc - 0x0F;

	loc++;

	// load value from next bytecode
	switch (*loc) {
		case 0x40: // read
		case 0x41: // write
			interpreter.registers[reg] = *loc;
			loc++;
			break;
		case char('[' + 0x80):
			interpreter.registers[reg] = getVal(loc);
			break;
	} 
	
}

void interpret(std::string contents) {
	for (auto loc = contents.begin(); loc != contents.end(); loc++) {
		
		switch (*loc) {
			case 0x0A:
				execute(loc);
			case 0x0C:
				allocateMemory(loc);
				break;
			case 0x0E:
				move(loc);
				break;
		}
	}
}