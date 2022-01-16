#include "fdp/registry/data_io.hxx"
#include "fdp/objects/config.hxx"
namespace FDP {
YAML::Node parse_yaml_(ghc::filesystem::path yaml_path) {
  APILogger->debug("LocalFileSystem: Reading configuration file '{0}'",
                   yaml_path.string().c_str());
  return YAML::LoadFile(yaml_path.string().c_str());
}

ghc::filesystem::path create_table(const DataTable *table,
                                   const ghc::filesystem::path &data_product,
                                   const ghc::filesystem::path &component,
                                   const Config *config) {
  const ghc::filesystem::path name_space_ =
      config->get_default_output_namespace();
  const ghc::filesystem::path data_store_ = config->get_data_store();

  const std::string output_file_name_ = current_time_stamp(true) + ".h5";
  const ghc::filesystem::path output_dir_ =
      data_store_ / name_space_ / data_product;
  const ghc::filesystem::path output_path_ = output_dir_ / output_file_name_;
  const std::string arr_name_ = std::string(TABLE);

  if (output_path_.extension() != ".h5") {
    throw std::invalid_argument("Output file name for array must be HDF5");
  }

  if (!ghc::filesystem::exists(output_dir_))
    ghc::filesystem::create_directories(output_dir_);

  H5File *output_file_ = new H5File(output_path_.string(), H5F_ACC_TRUNC);

  Group *output_group_ =
      new Group(output_file_->createGroup(component.string().c_str()));

  const int rank_ = 1;

  HDF5::CompType comp_type_;

  std::vector<char *> col_units_;

  for (const auto &int_col : table->get_int_columns()) {
    APILogger->debug("FileSystem:CreateTable: Adding field '{0}' to container",
                     int_col.first);
    char *col_unit_ =
        new char[int_col.second->unit_of_measurement().length() + 1];
    strcpy(col_unit_, int_col.second->unit_of_measurement().c_str());
    col_units_.push_back(col_unit_);
    comp_type_.AddMember<int>(int_col.first);
  }

  for (const auto &float_col : table->get_float_columns()) {
    APILogger->debug("FileSystem:CreateTable: Adding field '{0}' to container",
                     float_col.first);
    char *col_unit_ =
        new char[float_col.second->unit_of_measurement().length() + 1];
    strcpy(col_unit_, float_col.second->unit_of_measurement().c_str());
    col_units_.push_back(col_unit_);
    comp_type_.AddMember<float>(float_col.first);
  }

  for (const auto &str_col : table->get_str_columns()) {
    APILogger->debug("FileSystem:CreateTable: Adding field '{0}' to container",
                     str_col.first);
    char *col_unit_ =
        new char[str_col.second->unit_of_measurement().length() + 1];
    strcpy(col_unit_, str_col.second->unit_of_measurement().c_str());
    col_units_.push_back(col_unit_);
    comp_type_.AddMember<char *>(str_col.first);
  }

  const int length_ = table->size();

  HDF5::CompValueArray array_to_write_(comp_type_, length_);

  APILogger->debug("FileSystem:CreateTable: Writing values to CompType");

  int row_index_counter_ = 0;

  for (const auto &int_col : table->get_int_columns()) {
    APILogger->debug("FileSystem:CreateTable: Writing values to field '{0}'",
                     int_col.first);
    for (const int &value : int_col.second->values()) {
      array_to_write_.SetValue<int>(row_index_counter_, int_col.first, value);
      row_index_counter_ += 1;
    }
    row_index_counter_ = 0;
  }

  for (const auto &float_col : table->get_float_columns()) {
    APILogger->debug("FileSystem:CreateTable: Writing values to field '{0}'",
                     float_col.first);
    for (const float &value : float_col.second->values()) {
      array_to_write_.SetValue<float>(row_index_counter_, float_col.first,
                                      value);
      row_index_counter_ += 1;
    }
  }

  row_index_counter_ = 0;

  for (const auto &str_col : table->get_str_columns()) {
    APILogger->debug("FileSystem:CreateTable: Writing values to field '{0}'",
                     str_col.first);
    for (const std::string &value : str_col.second->values()) {
      char *str = new char[value.length() + 1];
      strcpy(str, value.c_str());
      array_to_write_.SetValue<char *>(row_index_counter_, str_col.first, str);
      row_index_counter_ += 1;
    }
    row_index_counter_ = 0;
  }

  std::vector<hsize_t> dim = {table->size()};

  DataSpace dspace_(rank_, dim.data());

  // Specifier the memory layout of our comp type
  CompType h5_comp_type_(array_to_write_.type.size);

  // We iterate through the fields of our custom compound type
  // and tell HDF5 about that.
  for (auto &x : array_to_write_.type.members) {
    auto &field_name = x.first;
    auto &field_val = x.second;
    APILogger->debug("Name-Offset: {0} {1}", field_name, field_val.offset);
    h5_comp_type_.insertMember(field_name, field_val.offset,
                               *field_val.GetType());
  }

  // Create data set
  H5::DataSet *dataset_ = new DataSet(
      output_group_->createDataSet(std::string(TABLE), h5_comp_type_, dspace_));

  // Do the write. Call the Data() function on our array
  dataset_->write(array_to_write_.Data(), h5_comp_type_);

  array_to_write_.Cleanup();

  // Clean up
  delete dataset_;
  delete output_file_;

  if (!ghc::filesystem::exists(output_path_)) {
    APILogger->error("Failed to create output HDF5 file '{0}' for object '{1}'",
                     output_path_.string(), data_product.string());
    throw std::runtime_error("Failed to write output");
  }

  StrType stype_(H5T_C_S1, H5T_VARIABLE);

  // ---------- WRITE UNITS ------------- //

  const hsize_t unit_dim_[1] = {col_units_.size()};

  const int urank = 1;
  DataSpace *str_space_u_ = new DataSpace(urank, unit_dim_);
  const std::string ulabel_ = std::string(COLUMN_UNITS);

  DataSet *dset_str_u_ = new DataSet(
      output_group_->createDataSet(ulabel_.c_str(), stype_, *str_space_u_));

  dset_str_u_->write(col_units_.data(), stype_);

  dset_str_u_->close();
  str_space_u_->close();

  delete dset_str_u_;
  delete str_space_u_;

  // --------- WRITE ROW NAMES --------- //
  std::vector<char *> row_names_;

  for (int i{0}; i < table->get_row_names().size(); ++i) {
    const std::string name_ = table->get_row_names()[i];
    char *row_name_ = new char[name_.length() + 1];
    strcpy(row_name_, name_.c_str());
    row_names_.push_back(row_name_);
  }
  const hsize_t str_dim_[1] = {table->get_row_names().size()};
  const int rrank = 1;
  DataSpace *str_space_r_ = new DataSpace(rrank, str_dim_);
  const std::string rlabel_ = std::string(ROW_NAMES);

  DataSet *dset_str_r_ = new DataSet(
      output_group_->createDataSet(rlabel_.c_str(), stype_, *str_space_r_));
  dset_str_r_->write(row_names_.data(), stype_);

  dset_str_r_->close();
  str_space_r_->close();

  delete dset_str_r_;
  delete str_space_r_;

  APILogger->debug("FileSystem:CreateTable: Wrote file '{0}'",
                   output_path_.string());

  return output_path_;
}

ghc::filesystem::path
create_distribution(const Distribution *distribution,
                    const ghc::filesystem::path &data_product,
                    const Versioning::version &version_num,
                    const Config *config) {
  const std::string param_name_ = data_product.stem().string();
  const std::string namespace_ = config->get_default_output_namespace();
  const ghc::filesystem::path data_store_ = config->get_data_store();
  toml::value data_{
      {param_name_,
       {{"type", "distribution"}, {"distribution", distribution->get_name()}}}};

  for (auto &param : distribution->get_params()) {
    data_[param_name_][param.first] = param.second;
  }

  const ghc::filesystem::path output_filename_ =
      data_store_ / namespace_ / data_product.parent_path() /
      std::string(version_num.to_string() + ".toml");
  std::ofstream toml_out_;

  if (!ghc::filesystem::exists(output_filename_.parent_path())) {
    ghc::filesystem::create_directories(output_filename_.parent_path());
  }

  toml_out_.open(output_filename_);

  if (!toml_out_) {
    throw std::runtime_error("Failed to open TOML file for writing");
  }

  toml_out_ << toml::format(data_);

  toml_out_.close();

  APILogger->debug("FileSystem:CreateEstimate: Wrote point estimate to '{0}'",
                   output_filename_.string());

  return output_filename_;
}

double read_point_estimate_from_toml(const ghc::filesystem::path var_address) {
  if (!ghc::filesystem::exists(var_address)) {
    throw std::runtime_error("File '" + var_address.string() +
                             "' could not be opened as it does not exist");
  }

  const auto toml_data_ = toml::parse(var_address.string());

  const std::string first_key_ = get_first_key_(toml_data_);

  if (!toml::get<toml::table>(toml_data_).at(first_key_).contains("type")) {
    throw std::runtime_error("Expected 'type' tag but none found");
  }

  if (static_cast<std::string>(
          toml_data_.at(first_key_).at("type").as_string()) !=
      "point-estimate") {
    throw std::runtime_error(
        "Expected 'point-estimate' for type but got '" +
        static_cast<std::string>(toml_data_.at("type").as_string()) + "'");
  }

  return (toml_data_.at(first_key_).at("value").is_floating())
             ? toml_data_.at(first_key_).at("value").as_floating()
             : static_cast<double>(
                   toml_data_.at(first_key_).at("value").as_integer());
}

Distribution *construct_dis_(const toml::value data_table) {
  const std::string first_key_ = get_first_key_(data_table);
  const std::string dis_type_ =
      data_table.at(first_key_).at("distribution").as_string();

  if (dis_type_ == "normal") {
    const double mean =
        (data_table.at(first_key_).at("mu").is_floating())
            ? data_table.at(first_key_).at("mu").as_floating()
            : static_cast<double>(
                  data_table.at(first_key_).at("mu").as_integer());
    const double sd =
        (data_table.at(first_key_).at("sigma").is_floating())
            ? data_table.at(first_key_).at("sigma").as_floating()
            : static_cast<double>(
                  data_table.at(first_key_).at("sigma").as_integer());
    return new Normal(mean, sd);
  }

  else if (dis_type_ == "gamma") {
    const double k = (data_table.at(first_key_).at("k").is_floating())
                         ? data_table.at(first_key_).at("k").as_floating()
                         : static_cast<double>(
                               data_table.at(first_key_).at("k").as_integer());
    const double theta =
        (data_table.at(first_key_).at("theta").is_floating())
            ? data_table.at(first_key_).at("theta").as_floating()
            : static_cast<double>(
                  data_table.at(first_key_).at("theta").as_integer());
    return new Gamma(k, theta);
  }

  throw std::runtime_error("Distribution currently unsupported");
}

Distribution *
read_distribution_from_toml(const ghc::filesystem::path var_address) {
  if (!ghc::filesystem::exists(var_address)) {
    throw std::runtime_error("File '" + var_address.string() +
                             "' could not be opened as it does not exist");
  }

  const auto toml_data_ = toml::parse(var_address.string());

  const std::string first_key_ = get_first_key_(toml_data_);

  if (!toml::get<toml::table>(toml_data_).at(first_key_).contains("type")) {
    throw std::runtime_error("Expected 'type' tag but none found");
  }

  if (static_cast<std::string>(
          toml_data_.at(first_key_).at("type").as_string()) != "distribution") {
    throw std::runtime_error(
        "Expected 'distribution' for type but got '" +
        static_cast<std::string>(toml_data_.at("type").as_string()) + "'");
  }

  return construct_dis_(toml_data_);
}

std::string get_first_key_(const toml::value data_table) {
  for (auto const& i : data_table.as_table()) {
    return i.first;
  }

  return "";
}
}; // namespace FDP