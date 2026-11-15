#ifndef TESTDIR
#define TESTDIR ""
#endif

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "fdp/fdp.h"
#include "fdp/objects/metadata.hxx" // read_token

#include "gtest/gtest.h"
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;
namespace fdp = FairDataPipeline;

std::string home_dir() {
  std::string home;
#ifdef _WIN32
  home = getenv("HOMEDRIVE");
  home += getenv("HOMEPATH");
#else
  home = getenv("HOME");
#endif
  return home;
}

#define BUFFER_SIZE 512

TEST(CTest, link_read_write) {
  fdp_set_log_level(FDP_LOG_DEBUG);
  char buf[BUFFER_SIZE];

  // Initialise
  FdpDataPipeline *pipeline;
  fs::path config = fs::path(TESTDIR) / "data" / "write_csv.yaml";
  fs::path script = fs::path(TESTDIR) / "test_script.sh";
  std::string token =
      fdp::read_token(fs::path(home_dir()) / ".fair" / "registry" / "token");
  ASSERT_EQ(fdp_init(&pipeline, config.string().c_str(),
                     script.string().c_str(), token.c_str()),
            FDP_ERR_NONE);

  // Test link write
  buf[0] = '\0'; // Ensure strlen of output buffer is 0
  EXPECT_EQ(fdp_link_write(pipeline, "test/csv/c", buf, BUFFER_SIZE),
            FDP_ERR_NONE);
  EXPECT_GT(strlen(buf), 1);

  // Write to new path
  std::ofstream fstream(buf);
  fstream << "Test";
  fstream.close();

  // Finalise and re-initialise
  ASSERT_EQ(fdp_finalise(&pipeline), FDP_ERR_NONE);
  config = fs::path(TESTDIR) / "data" / "read_csv.yaml";
  ASSERT_EQ(fdp_init(&pipeline, config.string().c_str(),
                     script.string().c_str(), token.c_str()),
            FDP_ERR_NONE);

  // Test link read
  buf[0] = '\0'; // Ensure strlen of output buffer is 0
  EXPECT_EQ(fdp_link_read(pipeline, "test/csv/c", buf, BUFFER_SIZE),
            FDP_ERR_NONE);
  EXPECT_GT(strlen(buf), 1);

  // Finalise again
  EXPECT_EQ(fdp_finalise(&pipeline), FDP_ERR_NONE);
}

TEST(CTest, cpp_to_c) {
  fdp_set_log_level(FDP_LOG_DEBUG);
  char buf[512];

  // Initialise using C++
  fs::path config = fs::path(TESTDIR) / "data" / "write_csv.yaml";
  fs::path script = fs::path(TESTDIR) / "test_script.sh";
  std::string token =
      fdp::read_token(fs::path(home_dir()) / ".fair" / "registry" / "token");
  auto cpp_pipeline =
      fdp::DataPipeline::construct(config.string(), script.string(), token);

  // Switch to C API, use temporary FdpDataPipeline
  FdpDataPipeline *c_pipeline = fdp::to_c_struct(cpp_pipeline);

  // Test link write
  buf[0] = '\0'; // Ensure strlen of output buffer is 0
  EXPECT_EQ(fdp_link_write(c_pipeline, "test/csv/cpp_to_c", buf, BUFFER_SIZE),
            FDP_ERR_NONE);
  EXPECT_GT(strlen(buf), 1);

  // Write to new path
  std::ofstream fstream(buf);
  fstream << "Test";
  fstream.close();

  // Finish working in C, delete FdpDataPipeline
  fdp::delete_c_struct(c_pipeline);

  // Finalise in C++
  cpp_pipeline->finalise();
}

TEST(CTest, c_to_cpp) {
  fdp_set_log_level(FDP_LOG_DEBUG);

  // Initialise using C
  FdpDataPipeline *c_pipeline;
  fs::path config = fs::path(TESTDIR) / "data" / "write_csv.yaml";
  fs::path script = fs::path(TESTDIR) / "test_script.sh";
  std::string token =
      fdp::read_token(fs::path(home_dir()) / ".fair" / "registry" / "token");
  ASSERT_EQ(fdp_init(&c_pipeline, config.string().c_str(),
                     script.string().c_str(), token.c_str()),
            FDP_ERR_NONE);

  // Switch to C++ API
  auto cpp_pipeline = fdp::from_c_struct(c_pipeline);

  // Test link write
  std::string data_product = "test/csv/c_to_cpp";
  fs::path currentLink = fs::path(cpp_pipeline->link_write(data_product));
  EXPECT_GT(currentLink.string().size(), 1);

  // Write to new path
  std::ofstream fstream(currentLink);
  fstream << "Test";
  fstream.close();

  // Finish working in C++
  cpp_pipeline = nullptr;

  // Finalise in C
  EXPECT_EQ(fdp_finalise(&c_pipeline), FDP_ERR_NONE);
}

TEST(CTest, log_levels) {
  fdp_set_log_level(FDP_LOG_INFO);
  EXPECT_EQ(fdp_get_log_level(), FDP_LOG_INFO);
  fdp_set_log_level(FDP_LOG_DEBUG);
  EXPECT_EQ(fdp_get_log_level(), FDP_LOG_DEBUG);
}
