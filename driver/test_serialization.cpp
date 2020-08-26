#include <iostream>
#include <string>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/tracking.hpp>

struct a {
  int x;
  int y;

  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar& x;
    ar& y;
  }
};

BOOST_CLASS_IMPLEMENTATION(
    a,
    boost::serialization::level_type::object_serializable)
BOOST_CLASS_TRACKING(a, boost::serialization::track_never)

int main() {
  a b{122, 2};

  boost::archive::text_oarchive archive{
      std::cout, boost::archive::no_header | boost::archive::no_tracking |
                     boost::archive::no_xml_tag_checking};
  archive << b;

  return 0;
}
