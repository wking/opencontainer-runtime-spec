// See README.txt for information and build instructions.

#include <fstream>
#include <iostream>
#include <string>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/util/type_resolver.h>
#include <google/protobuf/util/type_resolver_util.h>

#include "config.pb.h"
#include "runtime_config.pb.h"

using namespace std;

static const char kTypeUrlPrefix[] = "type.googleapis.com";

static string GetTypeUrl(const google::protobuf::Descriptor* message) {
  return string(kTypeUrlPrefix) + "/" + message->full_name();
}

static bool ReadMessage(string path, google::protobuf::Message *message) {
  string binary;
  ifstream input(path.c_str(), ios::in | ios::binary);
  if (!input) {
    cout << path << ": File not found." << endl;
    return false;
  }
  string json( (istreambuf_iterator<char>(input)),
               istreambuf_iterator<char>() );
  google::protobuf::scoped_ptr<google::protobuf::util::TypeResolver> resolver;
  resolver.reset(google::protobuf::util::NewTypeResolverForDescriptorPool(
    kTypeUrlPrefix,
    google::protobuf::DescriptorPool::generated_pool()));
  GOOGLE_CHECK_OK(google::protobuf::util::JsonToBinaryString(
    resolver.get(),
    GetTypeUrl(message->GetDescriptor()),
    json,
    &binary));
  return message->ParseFromString(binary);
}

static bool WriteMessage(const google::protobuf::Message& message) {
  string json;
  google::protobuf::util::JsonOptions options;
  google::protobuf::scoped_ptr<google::protobuf::util::TypeResolver> resolver;
  resolver.reset(google::protobuf::util::NewTypeResolverForDescriptorPool(
    kTypeUrlPrefix,
    google::protobuf::DescriptorPool::generated_pool()));
  options.add_whitespace = true;
  GOOGLE_CHECK_OK(google::protobuf::util::BinaryToJsonString(
    resolver.get(),
    GetTypeUrl(message.GetDescriptor()),
    message.SerializeAsString(),
    &json,
    options));
  cout << json;
  return true;
}

// Main function: Reads the config from a file and writes it to stdout.
int main(int argc, char* argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  oci::Spec config;
  oci::RuntimeSpec runtime;

  if (!ReadMessage("config.json", &config)) {
    cerr << "config.json: Failed to load." << endl;
    return -1;
  }

/*
  oci::LinuxUser user;
  user.set_uid(1);
  user.set_gid(1);
  user.add_additional_gids(5);
  user.add_additional_gids(6);
  config.mutable_process()->mutable_user()->PackFrom(user);
*/

  if (!WriteMessage(config)) {
    cerr << "config.json: Failed to write to stdout." << endl;
    return -1;
  }

  if (!ReadMessage("runtime.json", &runtime)) {
    cerr << "runtime.json: Failed to load." << endl;
    return -1;
  }

  if (!WriteMessage(runtime)) {
    cerr << "runtime.json: Failed to write to stdout." << endl;
    return -1;
  }

  // Optional:  Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
