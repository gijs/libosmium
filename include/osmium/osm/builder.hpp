#ifndef OSMIUM_OSM_BUILDER_HPP
#define OSMIUM_OSM_BUILDER_HPP

/*

This file is part of Osmium (http://osmcode.org/osmium).

Copyright 2013 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <osmium/memory/builder.hpp>
#include <osmium/osm.hpp>

namespace osmium {

    namespace osm {

        template <class T>
        class OSMObjectBuilder : public osmium::memory::ObjectBuilder<T> {

        public:

            OSMObjectBuilder(osmium::memory::Buffer& buffer, osmium::memory::Builder* parent=nullptr) :
                osmium::memory::ObjectBuilder<T>(buffer, parent) {
                static_cast<osmium::memory::Builder*>(this)->reserve_space_for<string_size_type>();
                static_cast<osmium::memory::Builder*>(this)->add_size(sizeof(string_size_type));
            }

        }; // class OSMObjectBuilder

        typedef OSMObjectBuilder<osmium::Node> NodeBuilder;
        typedef OSMObjectBuilder<osmium::Way> WayBuilder;
        typedef OSMObjectBuilder<osmium::Relation> RelationBuilder;

        typedef osmium::memory::ObjectBuilder<osmium::Changeset> ChangesetBuilder;

        class TagListBuilder : public osmium::memory::ObjectBuilder<TagList> {

        public:

            TagListBuilder(osmium::memory::Buffer& buffer, Builder* parent=nullptr) :
                osmium::memory::ObjectBuilder<TagList>(buffer, parent) {
            }

            ~TagListBuilder() {
                add_padding();
            }

            void add_tag(const char* key, const char* value) {
                add_size(append(key) + append(value));
            }

        }; // class TagListBuilder

        class WayNodeListBuilder : public osmium::memory::ObjectBuilder<WayNodeList> {

        public:

            WayNodeListBuilder(osmium::memory::Buffer& buffer, Builder* parent=nullptr) :
                osmium::memory::ObjectBuilder<WayNodeList>(buffer, parent) {
            }

            ~WayNodeListBuilder() {
                add_padding();
            }

            void add_way_node(const WayNode& way_node) {
                new (reserve_space_for<osmium::WayNode>()) osmium::WayNode(way_node);
                add_size(sizeof(osmium::WayNode));
            }

            void add_way_node(const object_id_type ref, const Location location=Location()) {
                add_way_node(WayNode(ref, location));
            }

        }; // class WayNodeListBuilder

        class RelationMemberListBuilder : public osmium::memory::ObjectBuilder<RelationMemberList> {

            void add_role(osmium::RelationMember* member, const char* role) {
                member->set_role_size(std::strlen(role) + 1);
                add_size(append(role));
                add_padding(true);
            }

        public:

            RelationMemberListBuilder(osmium::memory::Buffer& buffer, Builder* parent=nullptr) :
                osmium::memory::ObjectBuilder<RelationMemberList>(buffer, parent) {
            }

            ~RelationMemberListBuilder() {
                add_padding();
            }

            void add_member(osmium::item_type type, object_id_type ref, const char* role, const osmium::Object* full_member = nullptr) {
                osmium::RelationMember* member = reserve_space_for<osmium::RelationMember>();
                new (member) osmium::RelationMember(ref, type, full_member != nullptr);
                add_size(sizeof(RelationMember));
                add_role(member, role);
                if (full_member) {
                    add_item(full_member);
                }
            }

        }; // class RelationMemberListBuilder

    } // namespace osm

} // namespace osmium

#endif // OSMIUM_OSM_BUILDER_HPP
