#ifndef COMMAND_TAGS_FILTER_HPP
#define COMMAND_TAGS_FILTER_HPP

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

#include <map>
#include <set>
#include <string>
#include <vector>

#include <osmium/fwd.hpp>
#include <osmium/index/id_set.hpp>
#include <osmium/index/nwr_array.hpp>
#include <osmium/osm/entity_bits.hpp>
#include <osmium/osm/item_type.hpp>
#include <osmium/osm/types.hpp>
#include <osmium/tags/filter.hpp>

#include "cmd.hpp" // IWYU pragma: export


class CommandTagsFilter : public Command, public with_single_osm_input, public with_osm_output {

    bool m_add_referenced_objects = false;

    osmium::nwr_array<osmium::tags::KeyFilter> m_filters;

    osmium::nwr_array<osmium::index::IdSetDense<osmium::unsigned_object_id_type>> m_ids;

    osmium::osm_entity_bits::type get_needed_types() const;

    void find_referenced_objects();

    void add_nodes(const osmium::Way& way);
    void add_members(const osmium::Relation& relation);

    void mark_rel_ids(const std::multimap<osmium::object_id_type, osmium::object_id_type>& rel_in_rel, osmium::object_id_type id);
    bool find_relations_in_relations();
    void find_nodes_and_ways_in_relations();
    void find_nodes_in_ways();

    void parse_and_add_key(const std::string& line);
    void read_filter_file(const std::string& file_name);

public:

    CommandTagsFilter() = default;

    bool setup(const std::vector<std::string>& arguments) override final;

    void show_arguments() override final;

    bool run() override final;

    const char* name() const noexcept override final {
        return "tags-filter";
    }

    const char* synopsis() const noexcept override final {
        return "osmium tags-filter [OPTIONS] OSM-FILE";
    }

}; // class CommandTagsFilter


#endif // COMMAND_TAGS_FILTER_HPP
