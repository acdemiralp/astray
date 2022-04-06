#pragma once

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <cstddef>
#include <filesystem>
#include <stdexcept>
#include <vector>

#include <astray/math/indexing.hpp>
#include <astray/math/linear_algebra.hpp>
#include <astray/third_party/stb/stb_image.h>
#include <astray/third_party/stb/stb_image_write.h>
#include <astray/utility/multi_vector.hpp>

namespace ast
{
template <typename type>
class image : public multi_vector<type, 2, std::experimental::layout_left>
{
public:
  using base_type = multi_vector<type, 2, std::experimental::layout_left>;
  using size_type = typename base_type::multi_size_type;

  explicit image  (const std::filesystem::path& filepath)
  {
    load(filepath);
  }
  explicit image  (const size_type&   size, const type& value = type()) : base_type(size, value)
  {
    
  }
  image           (const image&       that) = default;
  image           (      image&&      temp) = default;
 ~image           ()                        = default;
  image& operator=(const image&       that) = default;
  image& operator=(      image&&      temp) = default;
  
  void                     load(const std::filesystem::path& filepath)
  {
    if (!exists(filepath))
      throw std::runtime_error("File does not exist!");

    auto components = 0;
    auto* const raw = reinterpret_cast<type*>(stbi_load(filepath.string().c_str(), &size_[0], &size_[1], &components, sizeof(type)));
    data_ = base_type::storage_type(raw, raw + size_[0] * size_[1]); // Copy will not be necessary if https://github.com/nothings/stb/issues/58 is resolved.
    stbi_image_free(raw);

    // TODO
  }
  void                     save(const std::filesystem::path& filepath) const
  {
    const auto extension = filepath.extension();
    if      (extension == ".bmp") stbi_write_bmp(filepath.string().c_str(), size[0], size[1], sizeof(type), base_type::storage_.data());
    else if (extension == ".jpg") stbi_write_jpg(filepath.string().c_str(), size[0], size[1], sizeof(type), base_type::storage_.data(), 100);
    else if (extension == ".png") stbi_write_png(filepath.string().c_str(), size[0], size[1], sizeof(type), base_type::storage_.data(), size[0] * sizeof(type));
    else if (extension == ".tga") stbi_write_tga(filepath.string().c_str(), size[0], size[1], sizeof(type), base_type::storage_.data());
    else throw std::runtime_error("Unsupported extension!");

    // TODO
  }
};
}