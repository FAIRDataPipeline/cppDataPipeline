#ifndef TESTDIR
#define TESTDIR ""
#endif

#include <iostream>
#include <fstream>

#include "fdp/fdp.hxx"
#include "fdp/objects/metadata.hxx"
#include "fdp/utilities/logging.hxx"

#include "gtest/gtest.h"


using namespace FairDataPipeline;

class PimplTest : public ::testing::Test {
protected:
  void SetUp() override {}
  std::string getHomeDirectory() {
    std::string HomeDirectory;
#ifdef _WIN32
    HomeDirectory = getenv("HOMEDRIVE");
    HomeDirectory += getenv("HOMEPATH");
#else
    HomeDirectory = getenv("HOME");
#endif
    return HomeDirectory;
  }

  std::string token =
      read_token(ghc::filesystem::path(getHomeDirectory()) /
                                    ".fair" / "registry" / "token");
  void TearDown() override {}
};

TEST_F(PimplTest, TestDataPipelineInit) {
  const ghc::filesystem::path config_path_ =
      ghc::filesystem::path(TESTDIR) / "data" / "write_csv.yaml";
  const ghc::filesystem::path script_path_ =
      ghc::filesystem::path(TESTDIR) / "test_script.sh";
  logger::get_logger()->set_level( logging::LOG_LEVEL::DEBUG );
  DataPipeline::sptr dp = DataPipeline::construct(config_path_.string(), script_path_.string(), token );

  std::string data_product = "test/csv/pimpl";
  ghc::filesystem::path currentLink = ghc::filesystem::path(dp->link_write(data_product));
  EXPECT_GT(currentLink.string().size(), 1);

  std::ofstream testCSV;
  testCSV.open(currentLink.string());
  testCSV << "Test";
  testCSV.close();

  dp->finalise();

  ghc::filesystem::path config_path_read_ =
      ghc::filesystem::path(TESTDIR) / "data" / "read_csv.yaml";

  dp = DataPipeline::construct(config_path_read_.string(), script_path_.string(), token );

  currentLink = dp->link_read(data_product);
  EXPECT_GT(currentLink.string().size(), 1);

  dp->finalise();

}
