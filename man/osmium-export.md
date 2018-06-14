
# NAME

osmium-export - export OSM data


# SYNOPSIS

**osmium export** \[*OPTIONS*\] *OSM-FILE*


# DESCRIPTION

The OSM data model with its nodes, ways, and relations is very different from
the data model usually used for geodata with features having point, linestring,
or polygon geometries (or their cousins, the multipoint, multilinestring, or
multipolygon geometries).

The **export** command transforms OSM data into a more usual GIS data model.
Nodes will be translated into points and ways into linestrings or polygons (if
they are closed ways). Multipolygon and boundary relations will be translated
into multipolygons. This transformation is not loss-less, especially
information in non-multipolygon, non-boundary relations is lost.

All tags are preserved in this process. Note that most GIS formats (such as
Shapefiles, etc.) do not support arbitrary tags. Transformation into other GIS
formats will need extra steps mapping tags to a limited list of attributes.
This is outside the scope of this command.

The **osmium export** command has to keep an index of the node locations in
memory or in a temporary file on disk while doing its work. There are several
different ways it can do that which have different advantages and
disadvantages. The default is good enough for most cases, but see the
**osmium-index-types**(5) man page for details.

This command will not work on full history files.

This command will work with negative IDs on OSM objects (for instance on
files created with JOSM).


# OPTIONS

-c, --config=FILE
:   Read configuration from specified file.

-e, --show-errors
:   Output any geometry errors on STDERR. This includes ways with a single
    node or areas that can't be assembled from multipolygon relations. This
    output is not suitable for automated use, there are other tools that can
    create very detailed errors reports that are better for that (see
    https://osmcode.org/osm-area-tools/).

-E, --stop-on-error
:   Usually geometry errors (due to missing node locations or broken polygons)
    are ignored and the features are omitted from the output. If this option
    is set, any error will immediately stop the program.

--geometry-type=TYPES
:   Specify the geometry types that should be written out. Usually all created
    geometries (points, linestrings, and (multi)polygons) are written to the
    output, but you can restrict the types using this option. TYPES is a
    comma-separated list of the types ("point", "linestring", and "polygon").

-i, --index-type=TYPE
:   Set the index type. For details see the **osmium-index-types**(5) man
    page.

-I, --show-index-types
:   Shows a list of available index types. For details see the
    **osmium-index-types**(5) man page.

-n, --keep-untagged
:   If this is set, features without any tags will be in the exported data.
    By default these features will be omitted from the output. Tags are the
    OSM tags, not attributes (like id, version, uid, ...) without the tags
    removed by the **exclude_tags** or **include_tags** settings.

-r, --omit-rs
:   Do not print the RS (0x1e, record separator) character when using the
    GeoJSON Text Sequence Format. Ignored for other formats.

-u, --add-unique-id=TYPE
:   Add a unique ID to each feature. TYPE can be either *counter* in which
    case the first feature will get ID 1, the next ID 2 and so on. The type
    of object does not matter in this case. Or the TYPE is *type_id* in which
    case the ID is a string, the first character is the type of object ('n'
    for nodes, 'w' for linestrings created from ways, and 'a' for areas
    created from ways and/or relations, after that there is a unique ID based
    on the original OSM object ID(s). If the input file has negative IDs, this
    can create IDs such as 'w-12'.

@MAN_COMMON_OPTIONS@
@MAN_PROGRESS_OPTIONS@
@MAN_INPUT_OPTIONS@

# OUTPUT OPTIONS

-f, --output-format=FORMAT
:   The format of the output file. Can be used to set the output file format
    if it can't be autodetected from the output file name. See the OUTPUT
    FORMATS section for a list of formats.

--fsync
:   Call fsync after writing the output file to force flushing buffers to disk.

-o, --output=FILE
:   Name of the output file. Default is '-' (STDOUT).

-O, --overwrite
:   Allow an existing output file to be overwritten. Normally **osmium** will
    refuse to write over an existing file.


# CONFIG FILE

The config file is in JSON format. The top-level is an object which contains
the following optional names:

* `attributes`: An object specifying which attributes of OSM objects to export.
   See the ATTRIBUTES section.
* `linear_tags`: An array of expressions specifying tags that should be treated
   as linear. See the FILTER EXPRESSION and AREA HANDLING sections.
* `area_tags`: An array of expressions specifying tags that should be treated
   as area tags. See the FILTER EXPRESSION and AREA HANDLING sections.
* `exclude_tags`: A list of tag expressions. Tags matching these expressions
   are excluded from the output. See the FILTER EXPRESSION section.
* `include_tags`: A list of tag expressions. Tags matching these expressions
   are included in the output. See the FILTER EXPRESSION section.

The `exclude_tags` and `include_tags` options are mutually exclusive. If you
want to just exclude some tags but leave most tags untouched, use the
`exclude_tags` setting. If you only want a defined list of tags, use
`include_tags`.

When no config file is specified, the following settings are used:

@EXPORT_DEFAULT_CONFIG@


# FILTER EXPRESSIONS

A filter expression specifies a tag or tags that should be matched in the data.

Some examples:

amenity
:   Matches all objects with the key "amenity".

highway=primary
:   Matches all objects with the key "highway" and value "primary".

highway!=primary
:   Matches all objects with the key "highway" and a value other than "primary".

type=multipolygon,boundary
:   Matches all objects with key "type" and value "multipolygon" or "boundary".

name,name:de=Kastanienallee,Kastanienstrasse
:   Matches any object with a "name" or "name:de" tag with the value
    "Kastanienallee" or "Kastanienstrasse".

addr:\*
:   Matches all objects with any key starting with "addr:"

name=\*Paris\*
:   Matches all objects with a name that contains the word "Paris".

If there is no equal sign ("=") in the expression only keys are matched and
values can by anything. If there is an equal sign ("=") in the expression, the
key is to the left and the value to the right. An exclamation sign ("!") before
the equal sign means: A tag with that key, but not the value(s) to the right of
the equal sign. A leading or trailing asterisk ("\*") can be used for substring
or prefix matching, respectively. Commas (",") can be used to separate several
keys or values.

All filter expressions are case-sensitive. There is no way to escape the
special characters such as "=", "\*" and ",". You can not mix
comma-expressions and "\*"-expressions.


# ATTRIBUTES

All OSM objects (nodes, ways, and relations) have *attributes*, areas inherit
their attributes from the ways and/or relations they were created from. The
attributes known to `osmium export` are:

* `type` ('node', 'way', or 'relation')
* `id` (64 bit object ID)
* `version` (version number)
* `changeset` (changeset ID)
* `timestamp` (time of object creation in seconds since Jan 1 1970)
* `uid` (user ID)
* `user` (user name)
* `way_nodes` (ways only, array with node IDs)

For areas, the type will be `way` or `relation` if the area was created
from a closed way or a multipolygon or boundary relation, respectively. The
`id` for areas is the id of the closed way or the multipolygon or boundary
relation.

By default the attributes will not be in the export, because they are not
necessary for most uses of OSM data. If you are interested in some (or all)
attributes, add an `attributes` object to the config file. Add a member for
each attribute you are interested in, the value can be either `false` (do not
output this attribute), `true` (output this attribute with the attribute name
prefixed by the `@` sign) or any string, in which case the string will be used
as the attribute name.

Note that the `id` is not necessarily unique. Even the combination `type` and
`id` is  not unique, because a way may end up as LineString and as Polygon
on the file. See the `--add-unique-id` option for a unique ID.


# AREA HANDLING

Multipolygon relations will be assembled into multipolygon geometries forming
areas. Some closed ways will also form areas. Here are the more detailed rules:

* Non-closed ways (last node not the same as the first node) are always
  linestrings, not areas.
* Relations tagged `type=multipolygon` or `type=boundary` are always assembled
  into areas. If they are not valid, they are omitted from the output (unless
  --stop-on-error/-E is specified). (An error message will be produced if the
  --show-errors/-e option is specified).
* For closed ways the tags are checked. If they have an `area` tag other than
  `area=no`, they are areas and a polygon is created. If they have an `area`
  tag other than `area=yes`, they are linestrings. So closed ways can be both,
  an area and a linestring!
* The configuration options `area_tags` and `linear_tags` can be used to
  augment the area check. If any of the tags on a closed way matches any of
  the expressions in `area_tags`, a polygon is created. If any of the tags on
  a closed way matches any of the expressions in `linear_tags`, a linestring
  is created. Again: If both match, an area and a linestring is created.


# OUTPUT FORMATS

The following output formats are supported:

* `geojson` (alias: `json`): GeoJSON (RFC7946). The output file will contain a
  single `FeatureCollection` object. This is the default format.
* `geojsonseq` (alias: `jsonseq`): GeoJSON Text Sequence (RFC8142). Each line
  (beginning with a RS (0x1e, record separator) and ending in a linefeed
  character) contains one GeoJSON object. Used for streaming GeoJSON.
* `text` (alias: `txt`): A simple text format with the geometry in WKT format
  followed by the comma-delimited tags. This is mainly intended for debugging
  at the moment. THE FORMAT MIGHT CHANGE WITHOUT NOTICE!


# DIAGNOSTICS

**osmium export** exits with exit code

0
  ~ if everything went alright,

1
  ~ if there was an error processing the data, or

2
  ~ if there was a problem with the command line arguments.


# MEMORY USAGE

**osmium export** will usually keep all node locations and all objects needed
for assembling the areas in memory. For larger data files, this can need
several tens of GBytes of memory. See the **osmium-index-types**(5) man page
for details.


# EXAMPLES

Export into GeoJSON format:

    osmium export data.osm.pbf -o data.geojson

Use a config file and export into GeoJSON Text Sequence format:

    osmium export data.osm.pbf -o data.geojsonseq -c export-config.json


# SEE ALSO

* **osmium**(1), **osmium-file-formats**(5), **osmium-index-types**(5),
  **osmium-add-node-locations-to-ways**(1)
* [Osmium website](https://osmcode.org/osmium-tool/)
* [GeoJSON](http://geojson.org/)
* [RFC7946](https://tools.ietf.org/html/rfc7946)
* [RFC8142](https://tools.ietf.org/html/rfc8142)
* [Line delimited JSON](https://en.wikipedia.org/wiki/JSON_Streaming#Line_delimited_JSON)

