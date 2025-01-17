#include "contrib/catch.hpp"

#include <bbp/sonata/nodes.h>

#include <iostream>
#include <string>
#include <vector>


using namespace bbp::sonata;


TEST_CASE("NodePopulation", "[base]")
{
    const NodePopulation population("./data/nodes1.h5", "", "nodes-A");

    CHECK(population.name() == "nodes-A");

    REQUIRE(population.size() == 6);

    REQUIRE(population.attributeNames() == std::set<std::string>{
      "attr-X", "attr-Y", "attr-Z",
      "A-double", "A-float", "A-int64", "A-int32", "A-int16", "A-int8", "A-uint64", "A-uint32", "A-uint16", "A-uint8", "A-string", "A-enum"
    });

    CHECK(
       population.getAttribute<double>("attr-X", Selection({{0, 1}, {5, 6}})) == std::vector<double>{11.0, 16.0}
    );
    CHECK(
       population.getAttribute<float>("attr-X", Selection({{0, 1}})) == std::vector<float>{11.0f}
    );
    CHECK(
       population.getAttribute<uint64_t>("attr-Y", Selection({{0, 1}, {5, 6}})) == std::vector<uint64_t>{21, 26}
    );
    CHECK(
       population.getAttribute<int64_t>("attr-Y", Selection({{0, 1}})) == std::vector<int64_t>{21}
    );
    CHECK(
       population.getAttribute<uint32_t>("attr-Y", Selection({{0, 1}})) == std::vector<uint32_t>{21}
    );
    CHECK(
       population.getAttribute<int32_t>("attr-Y", Selection({{0, 1}})) == std::vector<int32_t>{21}
    );
    CHECK(
       population.getAttribute<uint8_t>("attr-Y", Selection({{0, 1}})) == std::vector<uint8_t>{21}
    );
    CHECK(
       population.getAttribute<int8_t>("attr-Y", Selection({{0, 1}})) == std::vector<int8_t>{21}
    );
    CHECK(
       population.getAttribute<std::string>("attr-Z", Selection({{0, 2}})) == std::vector<std::string>{"aa", "bb"}
    );
    CHECK(
       population.getAttribute<std::string>("attr-Z", Selection({{0, 1}, {5, 6}})) == std::vector<std::string>{"aa", "ff"}
    );
    CHECK_THROWS_AS(
       population.getAttribute<double>("no-such-attribute", Selection({{0, 1}})),
       SonataError
    );

    // getAttribute with default value
    CHECK(
       population.getAttribute<double>("attr-X", Selection({{0, 1}, {5, 6}}), 42.0) == std::vector<double>{11.0, 16.0}
    );
    CHECK_THROWS_AS(
       population.getAttribute<double>("no-such-attribute", Selection({{0, 1}}), 42.0),
       SonataError
    );

    CHECK(population._attributeDataType("A-double") == "double");
    CHECK(population._attributeDataType("A-float") == "float");
    CHECK(population._attributeDataType("A-int64") == "int64_t");
    CHECK(population._attributeDataType("A-int32") == "int32_t");
    CHECK(population._attributeDataType("A-int16") == "int16_t");
    CHECK(population._attributeDataType("A-int8") == "int8_t");
    CHECK(population._attributeDataType("A-uint64") == "uint64_t");
    CHECK(population._attributeDataType("A-uint32") == "uint32_t");
    CHECK(population._attributeDataType("A-uint16") == "uint16_t");
    CHECK(population._attributeDataType("A-uint8") == "uint8_t");
    CHECK(population._attributeDataType("A-string") == "string");
    CHECK_THROWS_AS(
      population._attributeDataType("A-enum"),
      SonataError
    );
    CHECK_THROWS_AS(
       population._attributeDataType("no-such-attribute"),
       SonataError
    );

    REQUIRE(population.dynamicsAttributeNames() == std::set<std::string>{"dparam-X", "dparam-Y", "dparam-Z"});

    CHECK(
       population.getDynamicsAttribute<double>("dparam-X", Selection({{0, 1}, {5, 6}})) == std::vector<double>{1011.0, 1016.0}
    );
    CHECK(
       population.getDynamicsAttribute<float>("dparam-X", Selection({{0, 1}})) == std::vector<float>{1011.0f}
    );
    CHECK(
       population.getDynamicsAttribute<uint64_t>("dparam-Y", Selection({{0, 1}, {5, 6}})) == std::vector<uint64_t>{1021, 1026}
    );
    CHECK(
       population.getDynamicsAttribute<int64_t>("dparam-Y", Selection({{0, 1}})) == std::vector<int64_t>{1021}
    );
    CHECK(
       population.getDynamicsAttribute<uint32_t>("dparam-Y", Selection({{0, 1}})) == std::vector<uint32_t>{1021}
    );
    CHECK(
       population.getDynamicsAttribute<int32_t>("dparam-Y", Selection({{0, 1}})) == std::vector<int32_t>{1021}
    );
    CHECK(
       population.getDynamicsAttribute<uint16_t>("dparam-Y", Selection({{0, 1}})) == std::vector<uint16_t>{1021}
    );
    CHECK(
       population.getDynamicsAttribute<int16_t>("dparam-Y", Selection({{0, 1}})) == std::vector<int16_t>{1021}
    );
    CHECK(
       population.getDynamicsAttribute<std::string>("dparam-Z", Selection({{0, 2}})) == std::vector<std::string>{"d-aa", "d-bb"}
    );
    CHECK(
       population.getDynamicsAttribute<std::string>("dparam-Z", Selection({{0, 1}, {5, 6}})) == std::vector<std::string>{"d-aa", "d-ff"}
    );
    CHECK_THROWS_AS(
       population.getDynamicsAttribute<double>("no-such-attribute", Selection({{0, 1}})),
       SonataError
    );

    // getDynamicsAttribute with default value
    CHECK(
       population.getDynamicsAttribute<double>("dparam-X", Selection({{0, 1}, {5, 6}}), 42.0) == std::vector<double>{1011.0, 1016.0}
    );
    CHECK_THROWS_AS(
       population.getDynamicsAttribute<double>("no-such-attribute", Selection({{0, 1}}), 42.0),
       SonataError
    );

    CHECK(population._dynamicsAttributeDataType("dparam-X") == "double");
    CHECK(population._dynamicsAttributeDataType("dparam-Y") == "int64_t");
    CHECK(population._dynamicsAttributeDataType("dparam-Z") == "string");
}
