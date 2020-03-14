/*
    svalues.cc - The compiler's memory management
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

#include "../Storm/storm.h"
#include "parser.h"

/*
 variable class constructor
 set name
 create ident
*/
variable::variable(std::string _name) {
	name = _name;

	char hval_ident[8];
	// get hex of var
	sprintf(hval_ident, "%X", TotalNumber);

	ident.push_back('[' + 0x80);

	for (char n : std::string(hval_ident)) // create name
		ident.push_back(n + 0x80);

	ident.push_back(']' + 0x80);

	// add identifier to data
	parser.data.insert(parser.data.end(), 
		ident.begin(), ident.end());

	TotalNumber++;
}

// Convert std::string to bytecode
std::vector<uint8_t> addStringToByteCode(std::string lit) {
	std::vector<uint8_t> bytecode;

	for (char c : lit)
		bytecode.push_back(int(c) + 0x80); // bytecode ascii has an offset of 0x80

	return bytecode;
}

// find var by name and return ident
void find(std::string name, variable &buf) {
	for (variable v : parser.vars) {
		if (v.name == name) {
			buf = v;
			return;
		}
	}
}

void addLitToData(std::string literal) {
	std::vector<uint8_t> strByteCode = addStringToByteCode(literal);
	
	// the literal
	parser.data.insert(parser.data.end(),
		strByteCode.begin(), strByteCode.end());
}

// declare variable
void declare(std::vector<std::string>::iterator &chunk, std::string name) {
	variable v(name);

	if (*chunk == name)
		chunk += 2;

	if ((*chunk)[0] == '\"') {// string literal
		v.type = STRING;
		parser.data.push_back(v.type);
		
		addLitToData(*chunk);
	}
	else if (isInt(*chunk)) {
		v.type = INTEGER;

		parser.data.push_back(v.type);
		
		// push_back value of int
		for (char c : *chunk)
			parser.data.push_back(c - '0');
	}
	else if (*(chunk+1) == "[") { // function
		inlineFunc(chunk);
		return;
	}
	else {
		variable *match;
		find(*chunk, *match);
		v.type = match->type;
		
		parser.data.push_back(v.type);
		parser.data.insert(parser.data.end(),
			match->ident.begin(), match->ident.end());
	}

	parser.vars.push_back(v);
}