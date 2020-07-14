#ifndef STATIC_LINK_VW
#define BOOST_TEST_DYN_LINK
#endif

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include "test_common.h"
#include "vw.h"

BOOST_AUTO_TEST_CASE(chain_hashing_between_formats)
{
  feature_index txt_idx;
  feature_index json_idx;

  std::string text("1 |f a:b");
  std::string json_text = R"(
    {
      "_label": 1,
      "f": {
        "a": "b"
      }
    })";

  auto vw = VW::initialize("--quiet --chain_hash", nullptr, false, nullptr, nullptr);
  {
    auto example = &VW::get_unused_example(vw);
    VW::read_line(*vw, example, const_cast<char*>(text.c_str()));
    auto& indices = example->feature_space['f'].indicies;
    txt_idx = indices[0];
  }
  {
    auto examples = parse_json(*vw, json_text);
    auto example = examples[0];

    auto& indices = example->feature_space['f'].indicies;
    json_idx = indices[0];
  }
  BOOST_CHECK_EQUAL(txt_idx, json_idx);
}