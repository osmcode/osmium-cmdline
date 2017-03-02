/*

Osmium -- OpenStreetMap data manipulation command line tool
http://osmcode.org/osmium-tool/

Copyright (C) 2013-2017  Jochen Topf <jochen@topf.org>

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

#include <iostream>
#include <string>
#include <vector>

#include <osmium/io/file.hpp>
#include <osmium/util/file.hpp>

#include "exception.hpp"
#include "util.hpp"

/**
 * Get the suffix of the given file name. The suffix is everything after
 * the *first* dot (.). So multiple suffixes will all be returned.
 *
 * france.poly    -> poly
 * planet.osm.bz2 -> osm.bz2
 * some/path/planet.osm.bz2 -> osm.bz2
 */
std::string get_filename_suffix(const std::string& file_name) {
    auto slash = file_name.find_last_of('/');
    if (slash == std::string::npos) {
        slash = 0;
    }
    const auto dot = file_name.find_first_of('.', slash);
    if (dot == std::string::npos) {
        return "";
    }
    return file_name.substr(dot + 1);
}

const char* yes_no(bool choice) noexcept {
    return choice ? "yes\n" : "no\n";
}

void warning(const char* text) {
    std::cerr << "WARNING: " << text;
}

void warning(const std::string& text) {
    std::cerr << "WARNING: " << text;
}

std::size_t file_size_sum(const std::vector<osmium::io::File>& files) {
    std::size_t sum = 0;

    for (const auto& file : files) {
        sum += osmium::util::file_size(file.filename());
    }

    return sum;
}

osmium::osm_entity_bits::type get_types(const std::string& str) {
    osmium::osm_entity_bits::type entities{osmium::osm_entity_bits::nothing};

    for (const auto c : str) {
        switch (c) {
            case 'n':
                entities |= osmium::osm_entity_bits::node;
                break;
            case 'w':
                entities |= osmium::osm_entity_bits::way;
                break;
            case 'r':
                entities |= osmium::osm_entity_bits::relation;
                break;
            default:
                throw argument_error{std::string{"Unknown object type '"} + c + "' (allowed are 'n', 'w', and 'r')."};
        }
    }

    return entities;
}

std::pair<osmium::osm_entity_bits::type, std::string> get_filter_expression(const std::string& str) {
    auto pos = str.find('/');

    osmium::osm_entity_bits::type entities{osmium::osm_entity_bits::nwr};
    if (pos == std::string::npos) {
        pos = 0;
    } else if (pos == 0) {
        pos = 1;
    } else {
        entities = get_types(str.substr(0, pos));
        ++pos;
    }

    return std::make_pair(entities, &str[pos]);
}

