#ifndef OSMIUM_OSM_VISITOR_HPP
#define OSMIUM_OSM_VISITOR_HPP

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

#include <stdexcept>

#include <osmium/osm.hpp>
#include <osmium/memory/buffer.hpp>

namespace osmium {

    namespace osm {

        class Visitor {

        public:

            void operator()(const osmium::memory::Item&) const {
            }

            void operator()(osmium::memory::Item&) const {
            }

        }; // Visitor

        namespace {

            template <typename T, typename U>
            using MaybeConst = typename std::conditional<std::is_const<T>::value, typename std::add_const<U>::type, U>::type;

            template <class TVisitor, typename TItem>
            void apply_visitor_impl(TVisitor& visitor, TItem& item) {

                switch (item.type()) {
                    case osmium::item_type::node:
                        visitor(static_cast<MaybeConst<TItem, osmium::Node>&>(item));
                        break;
                    case osmium::item_type::way:
                        visitor(static_cast<MaybeConst<TItem, osmium::Way>&>(item));
                        break;
                    case osmium::item_type::relation:
                        visitor(static_cast<MaybeConst<TItem, osmium::Relation>&>(item));
                        break;
                    case osmium::item_type::changeset:
                        visitor(static_cast<MaybeConst<TItem, osmium::Changeset>&>(item));
                        break;
                    case osmium::item_type::tag_list:
                        visitor(static_cast<MaybeConst<TItem, osmium::TagList>&>(item));
                        break;
                    case osmium::item_type::way_node_list:
                        visitor(static_cast<MaybeConst<TItem, osmium::WayNodeList>&>(item));
                        break;
                    case osmium::item_type::relation_member_list:
                    case osmium::item_type::relation_member_list_with_full_members:
                        visitor(static_cast<MaybeConst<TItem, osmium::RelationMemberList>&>(item));
                        break;
                    default:
                        throw std::runtime_error("unknown type");
                }
            }

        } // anonymous namespace

        template <class TVisitor>
        void apply_visitor(TVisitor& visitor, const osmium::memory::Item& item) {
            apply_visitor_impl(visitor, item);
        }

        template <class TVisitor>
        void apply_visitor(TVisitor& visitor, osmium::memory::Item& item) {
            apply_visitor_impl(visitor, item);
        }

        template <class TVisitor>
        void apply_visitor(TVisitor& visitor, const osmium::memory::Buffer::const_iterator& begin, const osmium::memory::Buffer::const_iterator& end) {
            for (auto it = begin; it != end; ++it) {
                apply_visitor(visitor, *it);
            }
        }

        template <class TVisitor>
        void apply_visitor(TVisitor& visitor, const osmium::memory::Buffer::iterator& begin, const osmium::memory::Buffer::iterator& end) {
            for (auto it = begin; it != end; ++it) {
                apply_visitor(visitor, *it);
            }
        }

        template <class TVisitor>
        void apply_visitor(TVisitor& visitor, const osmium::memory::Buffer& buffer) {
            apply_visitor(visitor, buffer.cbegin(), buffer.cend());
        }

        template <class TVisitor>
        void apply_visitor(TVisitor& visitor, osmium::memory::Buffer& buffer) {
            apply_visitor(visitor, buffer.begin(), buffer.end());
        }

    } // namespace osm

} // namespace osmium

#endif // OSMIUM_OSM_VISITOR_HPP
