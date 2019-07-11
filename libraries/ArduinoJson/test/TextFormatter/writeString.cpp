// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2019
// MIT License

#include <catch.hpp>

<<<<<<< HEAD:libraries/ArduinoJson/test/TextFormatter/writeString.cpp
#include <ArduinoJson/Json/TextFormatter.hpp>
#include <ArduinoJson/Serialization/StaticStringWriter.hpp>
=======
#include <ArduinoJson/Serialization/JsonWriter.hpp>
#include <ArduinoJson/Serialization/StaticStringBuilder.hpp>
>>>>>>> FastLedi2c:libraries/ArduinoJson/test/JsonWriter/writeString.cpp

using namespace ArduinoJson::Internals;

void check(const char* input, std::string expected) {
  char output[1024];
<<<<<<< HEAD:libraries/ArduinoJson/test/TextFormatter/writeString.cpp
  StaticStringWriter sb(output, sizeof(output));
  TextFormatter<StaticStringWriter> writer(sb);
=======
  StaticStringBuilder sb(output, sizeof(output));
  JsonWriter<StaticStringBuilder> writer(sb);
>>>>>>> FastLedi2c:libraries/ArduinoJson/test/JsonWriter/writeString.cpp
  writer.writeString(input);
  REQUIRE(expected == output);
  REQUIRE(writer.bytesWritten() == expected.size());
}

TEST_CASE("TextFormatter::writeString()") {
  SECTION("Null") {
    check(0, "null");
  }

  SECTION("EmptyString") {
    check("", "\"\"");
  }

  SECTION("QuotationMark") {
    check("\"", "\"\\\"\"");
  }

  SECTION("ReverseSolidus") {
    check("\\", "\"\\\\\"");
  }

  SECTION("Solidus") {
    check("/", "\"/\"");  // but the JSON format allows \/
  }

  SECTION("Backspace") {
    check("\b", "\"\\b\"");
  }

  SECTION("Formfeed") {
    check("\f", "\"\\f\"");
  }

  SECTION("Newline") {
    check("\n", "\"\\n\"");
  }

  SECTION("CarriageReturn") {
    check("\r", "\"\\r\"");
  }

  SECTION("HorizontalTab") {
    check("\t", "\"\\t\"");
  }
}
