#include "fdp/objects/metadata.hxx"

namespace FairDataPipeline {
std::string calculate_hash_from_file(const ghc::filesystem::path &file_path) {
  if (!ghc::filesystem::exists(file_path)) {
    throw std::invalid_argument("File '" + file_path.string() + "' not found");
  }

  std::ifstream file_(file_path.string(), std::ios_base::in | std::ios_base::binary);

  const std::string hash_ = digestpp::sha1().absorb(file_).hexdigest();

  file_.close();

  return hash_;
}

std::string calculate_hash_from_string(const std::string &input) {
  return digestpp::sha1().absorb(input).hexdigest();
}

std::string generate_random_hash() {
  std::string random_string;

  // Use Both the random_device and chrono high resolution clock to generate
  // a good random seed

  std::random_device rd;
  std::mt19937::result_type seed =
      rd() ^
      ((std::mt19937::result_type)
           std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count() +
       (std::mt19937::result_type)
           std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch())
               .count());

  // Using mt19937 and a uniform distribution generate a 24 character random
  // string
  static const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  std::mt19937 gen(seed);
  std::uniform_int_distribution<std::size_t> distrib(0, sizeof(charset) - 2);

  for (unsigned long j = 0; j < 24; ++j) {
    random_string += charset[distrib(gen)];
  }

  return calculate_hash_from_string(random_string);
}

std::string current_time_stamp(bool file_name) {
  auto t_ = std::time(nullptr);
  auto tm_ = std::localtime(&t_);
  char buffer_[80];

  if (!file_name) {
    strftime(buffer_, sizeof(buffer_), "%Y-%m-%d %H:%M:%S", tm_);
  } else {
    strftime(buffer_, sizeof(buffer_), "%Y%m%d-%H%M%S", tm_);
  }

  return std::string(buffer_);
}

std::string remove_local_from_root(const std::string &root){
  return std::regex_replace(root, std::regex(std::string("file:\\/\\/")), "");
}

std::string remove_backslash_from_path(const std::string &path){
  return std::regex_replace(path, std::regex(std::string("\\\\")), "/");
}

bool file_exists( const std::string &Filename )
{
    return access( Filename.c_str(), 0 ) == 0;
}

std::string read_token(const ghc::filesystem::path &token_path){
  std::ifstream key_(token_path.string(), std::ios::in);
  std::string key_str_;
  key_ >> key_str_;
  key_.close();
  return key_str_;
}

}; // namespace FairDataPipeline
