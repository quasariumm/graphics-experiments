module;

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

export module dx_wrapper.external.fastgltf;

export namespace fastgltf
{
// Basic types
using fastgltf::Asset;
using fastgltf::Parser;
using fastgltf::MappedGltfFile;
using fastgltf::Options;
using fastgltf::AssetInfo;
using fastgltf::Mesh;
using fastgltf::Primitive;
using fastgltf::Node;
using fastgltf::TRS;

// Accessors / Buffers
using fastgltf::Accessor;
using fastgltf::AccessorType;

using fastgltf::Buffer;
using fastgltf::BufferInfo;
using fastgltf::BufferView;
using fastgltf::DataSource;
namespace sources
{
using fastgltf::sources::Array;
using fastgltf::sources::Vector;
using fastgltf::sources::URI;
using fastgltf::sources::BufferView;
using fastgltf::sources::ByteView;
}

// Materials
using fastgltf::Material;
using fastgltf::MaterialAnisotropy;
using fastgltf::MaterialClearcoat;
using fastgltf::MaterialDiffuseTransmission;
using fastgltf::MaterialIridescence;
using fastgltf::MaterialPackedTextures;
using fastgltf::MaterialSheen;
using fastgltf::MaterialSpecular;
using fastgltf::MaterialTransmission;
using fastgltf::MaterialVolume;
using fastgltf::AlphaMode;
using fastgltf::Texture;
using fastgltf::TextureInfo;
using fastgltf::NormalTextureInfo;
using fastgltf::OcclusionTextureInfo;
using fastgltf::Sampler;

// Math
namespace math
{
using fastgltf::math::dmat;
using fastgltf::math::dmat2x2;
using fastgltf::math::dmat3x3;
using fastgltf::math::dmat4x4;
using fastgltf::math::dquat;
using fastgltf::math::dvec;
using fastgltf::math::dvec2;
using fastgltf::math::dvec3;
using fastgltf::math::dvec4;
using fastgltf::math::fmat;
using fastgltf::math::fmat2x2;
using fastgltf::math::fmat3x3;
using fastgltf::math::fmat4x4;
using fastgltf::math::fquat;
using fastgltf::math::fvec;
using fastgltf::math::fvec2;
using fastgltf::math::fvec3;
using fastgltf::math::fvec4;
using fastgltf::math::f32vec;
using fastgltf::math::f32vec2;
using fastgltf::math::f32vec3;
using fastgltf::math::f32vec4;
using fastgltf::math::f64vec;
using fastgltf::math::f64vec2;
using fastgltf::math::f64vec3;
using fastgltf::math::f64vec4;
using fastgltf::math::ivec;
using fastgltf::math::ivec2;
using fastgltf::math::ivec3;
using fastgltf::math::ivec4;
using fastgltf::math::nvec2;
using fastgltf::math::nvec3;
using fastgltf::math::nvec4;
using fastgltf::math::uvec;
using fastgltf::math::uvec2;
using fastgltf::math::uvec3;
using fastgltf::math::uvec4;
using fastgltf::math::s8vec;
using fastgltf::math::s8vec2;
using fastgltf::math::s8vec3;
using fastgltf::math::s8vec4;
using fastgltf::math::s16vec;
using fastgltf::math::s16vec2;
using fastgltf::math::s16vec3;
using fastgltf::math::s16vec4;
using fastgltf::math::s32vec;
using fastgltf::math::s32vec2;
using fastgltf::math::s32vec3;
using fastgltf::math::s32vec4;
using fastgltf::math::u8vec;
using fastgltf::math::u8vec2;
using fastgltf::math::u8vec3;
using fastgltf::math::u8vec4;
using fastgltf::math::u16vec;
using fastgltf::math::u16vec2;
using fastgltf::math::u16vec3;
using fastgltf::math::u16vec4;
using fastgltf::math::u32vec;
using fastgltf::math::u32vec2;
using fastgltf::math::u32vec3;
using fastgltf::math::u32vec4;
}

// Functions
using fastgltf::iterateAccessor;
using fastgltf::iterateAccessorWithIndex;
using fastgltf::copyFromAccessor;
using fastgltf::copyComponentsFromAccessor;

// Operators
using fastgltf::operator|;
}