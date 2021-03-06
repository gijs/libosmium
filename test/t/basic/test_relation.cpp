#ifdef STAND_ALONE
# define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include <osmium/osm/relation.hpp>
#include <osmium/osm/ostream.hpp>

#include "helper.hpp"

BOOST_AUTO_TEST_SUITE(Basic_Relation)

BOOST_AUTO_TEST_CASE(relation_builder) {
    osmium::memory::Buffer buffer(10000);

    osmium::Relation& relation = buffer_add_relation(buffer,
        "foo", {
            {"type", "multipolygon"},
            {"name", "Sherwood Forest"}
        }, {
            std::make_tuple('w', 1, "inner"),
            std::make_tuple('w', 2, ""),
            std::make_tuple('w', 3, "outer")
        });

    relation.id(17)
        .version(3)
        .visible(true)
        .changeset(333)
        .uid(21)
        .timestamp(123);

    BOOST_CHECK_EQUAL(17, relation.id());
    BOOST_CHECK_EQUAL(3, relation.version());
    BOOST_CHECK_EQUAL(true, relation.visible());
    BOOST_CHECK_EQUAL(333, relation.changeset());
    BOOST_CHECK_EQUAL(21, relation.uid());
    BOOST_CHECK(!strcmp("foo", relation.user()));
    BOOST_CHECK_EQUAL(123, relation.timestamp());
    BOOST_CHECK_EQUAL(2, relation.tags().size());
    BOOST_CHECK_EQUAL(3, relation.members().size());

    int n=1;
    for (auto& member : relation.members()) {
        BOOST_CHECK_EQUAL(osmium::item_type::way, member.type());
        BOOST_CHECK_EQUAL(n, member.ref());
        switch (n) {
            case 1:
                BOOST_CHECK(!strcmp("inner", member.role()));
                break;
            case 2:
                BOOST_CHECK(!strcmp("", member.role()));
                break;
            case 3:
                BOOST_CHECK(!strcmp("outer", member.role()));
                break;
            default:
                BOOST_CHECK(false);
        }
        ++n;
    }
}

BOOST_AUTO_TEST_SUITE_END()
