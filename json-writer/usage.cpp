#include <iostream>
#include <cmath>
#include "json_writer.hpp"
using namespace std;

int main()
{
auto writer = new JsonWriter;
writer->StartArray ();

  writer->StartShortObject ();
    writer->KeyValue ("name", "andrewpqc");
    writer->KeyValue ("age", 22);
  writer->EndObject ();

  writer->StartObject ();
    writer->KeyValue ("skills", "c++");
    writer->KeyValue ("skills","python");
    writer->KeyValue ("skills","go");
    writer->KeyValue ("skills","k8s");
    writer->KeyValue ("url", "https://andrewpqc.github.io");

    writer->Key ("path");
    writer->StartArray ();
      writer->Value ("web");
      writer->Value ("algorithm");
      writer->Value ("linux");
    writer->EndArray ();

    writer->Key ("short-array");
    writer->StartShortArray ();
      writer->Value (1);
      writer->Value ((uint64_t)0xabcdef123456);
      writer->Value (M_PI);
    writer->EndContainer ();

  writer->EndObject (),
  writer->Value (false);
writer->EndArray ();
}
