/*

Osmium -- OpenStreetMap data manipulation command line tool
http://osmcode.org/osmium

Copyright (C) 2013-2016  Jochen Topf <jochen@topf.org>

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

#include <algorithm>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <osmium/io/header.hpp>
#include <osmium/io/reader.hpp>
#include <osmium/io/writer.hpp>
#include <osmium/io/input_iterator.hpp>
#include <osmium/io/output_iterator.hpp>
#include <osmium/memory/buffer.hpp>
#include <osmium/osm/entity_bits.hpp>
#include <osmium/osm/object.hpp>
#include <osmium/util/verbose_output.hpp>

#include "command_merge.hpp"

namespace osmium { namespace io {
    class File;
}}

bool CommandMerge::setup(const std::vector<std::string>& arguments) {
    po::options_description opts_cmd{"COMMAND OPTIONS"};

    po::options_description opts_common{add_common_options()};
    po::options_description opts_input{add_multiple_inputs_options()};
    po::options_description opts_output{add_output_options()};

    po::options_description hidden;
    hidden.add_options()
    ("input-filenames", po::value<std::vector<std::string>>(), "Input files")
    ;

    po::options_description desc;
    desc.add(opts_cmd).add(opts_common).add(opts_input).add(opts_output);

    po::options_description parsed_options;
    parsed_options.add(desc).add(hidden);

    po::positional_options_description positional;
    positional.add("input-filenames", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(arguments).options(parsed_options).positional(positional).run(), vm);
    po::notify(vm);

    setup_common(vm, desc);
    setup_input_files(vm);
    setup_output_file(vm);

    return true;
}

void CommandMerge::show_arguments() {
    show_multiple_inputs_arguments(m_vout);
    show_output_arguments(m_vout);
}

class DataSource {

    using it_type = osmium::io::InputIterator<osmium::io::Reader, osmium::OSMObject>;

    std::unique_ptr<osmium::io::Reader> reader;
    it_type iterator;

public:

    explicit DataSource(const osmium::io::File& file) :
        reader(new osmium::io::Reader{file}),
        iterator(*reader) {
    }

    bool empty() const noexcept {
        return iterator == it_type{};
    }

    bool next() noexcept {
        ++iterator;
        return iterator != it_type{};
    }

    const osmium::OSMObject* get() noexcept {
        return &*iterator;
    }

}; // DataSource

class QueueElement {

    const osmium::OSMObject* m_object;
    int m_data_source_index;

public:

    QueueElement(const osmium::OSMObject* object, int data_source_index) noexcept :
        m_object(object),
        m_data_source_index(data_source_index) {
    }

    const osmium::OSMObject& object() const noexcept {
        return *m_object;
    }

    int data_source_index() const noexcept {
        return m_data_source_index;
    }

}; // QueueElement

bool operator<(const QueueElement& lhs, const QueueElement& rhs) noexcept {
    return lhs.object() > rhs.object();
}

bool operator==(const QueueElement& lhs, const QueueElement& rhs) noexcept {
    return lhs.object() == rhs.object();
}

bool operator!=(const QueueElement& lhs, const QueueElement& rhs) noexcept {
    return ! (lhs == rhs);
}

osmium::Box merge_box(std::vector<osmium::io::File> m_input_files) {
    osmium::Box box;
    for (const osmium::io::File& file : m_input_files) {
        osmium::io::Reader reader{file, osmium::osm_entity_bits::nothing};
        osmium::io::Header header{reader.header()};
        if (header.box()) {
            box.extend(header.box());
        } else {
            return osmium::Box();
        }
    }
    return box;
}

bool CommandMerge::run() {
    m_vout << "Opening output file...\n";

    osmium::io::Header header;

    osmium::Box box = merge_box(m_input_files);
    if (box.valid()) {
        header.add_box(box);
    }

    header.set("generator", m_generator);
    for (const auto& h : m_output_headers) {
        header.set(h);
    }

    osmium::io::Writer writer{m_output_file, header, m_output_overwrite, m_fsync};

    if (m_input_files.size() == 1) {
        m_vout << "Single input file. Copying to output file...\n";
        osmium::io::Reader reader{m_input_files[0]};
        while (osmium::memory::Buffer buffer = reader.read()) {
            writer(std::move(buffer));
        }
    } else if (m_input_files.size() == 2) {
        // Use simpler code when there are exactly two files to merge
        m_vout << "Merging 2 input files to output file...\n";
        osmium::io::Reader reader1(m_input_files[0], osmium::osm_entity_bits::object);
        osmium::io::Reader reader2(m_input_files[1], osmium::osm_entity_bits::object);
        auto in1 = osmium::io::make_input_iterator_range<osmium::OSMObject>(reader1);
        auto in2 = osmium::io::make_input_iterator_range<osmium::OSMObject>(reader2);
        auto out = osmium::io::make_output_iterator(writer);

        std::set_union(in1.cbegin(), in1.cend(),
                       in2.cbegin(), in2.cend(),
                       out);
    } else {
        // Three or more files to merge
        m_vout << "Merging " << m_input_files.size() << " input files to output file...\n";
        std::vector<DataSource> data_sources;
        data_sources.reserve(m_input_files.size());

        std::priority_queue<QueueElement> queue;

        int index = 0;
        for (const osmium::io::File& file : m_input_files) {
            data_sources.emplace_back(file);

            if (!data_sources.back().empty()) {
                queue.emplace(data_sources.back().get(), index);
            }

            ++index;
        }

        while (!queue.empty()) {
            auto element = queue.top();
            queue.pop();
            if (queue.empty() || element != queue.top()) {
                writer(element.object());
            }

            const int index = element.data_source_index();
            if (data_sources[index].next()) {
                queue.emplace(data_sources[index].get(), index);
            }
        }
    }

    m_vout << "Closing output file...\n";
    writer.close();

    show_memory_used();
    m_vout << "Done.\n";

    return true;
}

namespace {

    const bool register_merge_command = CommandFactory::add("merge", "Merge several sorted OSM files into one", []() {
        return new CommandMerge();
    });

}

