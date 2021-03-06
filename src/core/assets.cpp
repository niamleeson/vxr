// ----------------------------------------------------------------------------------------
// MIT License
// 
// Copyright(c) 2018 V�ctor �vila
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ----------------------------------------------------------------------------------------

#include "../../include/core/assets.h"

#include "../../include/engine/engine.h"
#include "../../include/core/gameobject.h"
#include "../../include/components/renderer.h"
#include "../../include/components/mesh_filter.h"
#include "../../include/graphics/texture.h"
#include "../../include/graphics/materials/standard.h"
#include "../../include/graphics/materials/skybox.h"
#include "../../include/graphics/materials/unlit.h"
#include "../../include/graphics/materials/standard_pass.h"
#include "../../include/graphics/materials/pass_filters.h"

#include "../../deps/mesh/tiny_obj_loader.h"

namespace vxr
{

  ref_ptr<GameObject> Asset::loadModelOBJ(const char* file)
  {
    VXR_TRACE_SCOPE("VXR", "Load Model OBJ");
    std::vector<tinyobj::shape_t> m_shapes;
    std::vector<tinyobj::material_t> m_materials;

    string name = file;
    string path = name.substr(0, name.find_last_of("/") + 1);
    VXR_LOG(VXR_DEBUG_LEVEL_ERROR, "[INFO]: Loading mesh: %s.\n", name.c_str());
    string err = tinyobj::LoadObj(m_shapes, m_materials, name.c_str(), path.c_str());

    if (!err.empty())
    {
      VXR_LOG(VXR_DEBUG_LEVEL_ERROR, "[ERROR]: Could not load mesh %s (%s).\n", name.c_str(), err.c_str());
      return nullptr;
    }

    ref_ptr<GameObject> obj;
    obj.alloc();

    for (uint32 i = 0; i < m_shapes.size(); i++)
    {
      ref_ptr<GameObject> child;
      
      if (m_shapes.size() > 1)
      {
        // Create empty parent container if there's multiple parts in the asset.
        child.alloc();
        child->transform()->set_parent(obj->transform());
      }
      else
      {
        child = obj;
      }

      ref_ptr<Mesh> mesh;
      ref_ptr<mat::Standard::Instance> mat;

      mesh.alloc();
      mat.alloc();

      child->addComponent<MeshFilter>()->mesh = mesh;
      child->addComponent<Renderer>()->material = mat.get();

      std::vector<vec3> vertices;
      std::vector<vec3> normals;
      std::vector<vec2> uv;
      std::vector<uint32> indices;

      for (uint32 j = 0; j < m_shapes[i].mesh.indices.size(); ++j)
      {
        indices.push_back(m_shapes[i].mesh.indices[j]);
      }

      for (uint32 j = 0; j < m_shapes[i].mesh.positions.size(); j += 3)
      {
        vertices.push_back(vec3(m_shapes[i].mesh.positions[j], m_shapes[i].mesh.positions[j + 1], m_shapes[i].mesh.positions[j + 2]));
      }

      for (uint32 j = 0; j < m_shapes[i].mesh.normals.size(); j += 3)
      {
        normals.push_back(vec3(m_shapes[i].mesh.normals[j], m_shapes[i].mesh.normals[j + 1], m_shapes[i].mesh.normals[j + 2]));
      }

      for (uint32 j = 0; j < m_shapes[i].mesh.texcoords.size(); j += 2)
      {
        uv.push_back(vec2(m_shapes[i].mesh.texcoords[j], m_shapes[i].mesh.texcoords[j + 1]));
      }

      mesh->set_indices(indices);
      mesh->set_vertices(vertices);
      mesh->set_normals(normals);
      mesh->set_uv(uv);
    }

    m_shapes.clear();
    m_materials.clear();

    return obj;
  }

  ref_ptr<Mesh> Asset::loadMeshOBJ(const char* file, uint32 mesh)
  {
    std::vector<tinyobj::shape_t> m_shapes;
    std::vector<tinyobj::material_t> m_materials;

    string name = file;
    string path = name.substr(0, name.find_last_of("/") + 1);
    VXR_LOG(VXR_DEBUG_LEVEL_ERROR, "[INFO]: Loading mesh: %s.\n", name.c_str());
    string err = tinyobj::LoadObj(m_shapes, m_materials, name.c_str(), path.c_str());

    if (!err.empty())
    {
      VXR_LOG(VXR_DEBUG_LEVEL_ERROR, "[ERROR]: Could not load mesh %s (%s).\n", name.c_str(), err.c_str());
      return nullptr;
    }

    ref_ptr<Mesh> m;

    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<vec2> uv;
    std::vector<uint32> indices;

    for (uint32 i = 0; i < m_shapes[mesh].mesh.indices.size(); ++i)
    {
      indices.push_back(m_shapes[mesh].mesh.indices[i]);
    }

    for (uint32 i = 0; i < m_shapes[mesh].mesh.positions.size(); i += 3)
    {
      vertices.push_back(vec3(m_shapes[mesh].mesh.positions[i], m_shapes[mesh].mesh.positions[i + 1], m_shapes[mesh].mesh.positions[i + 2]));
    }

    for (uint32 i = 0; i < m_shapes[mesh].mesh.normals.size(); i += 3)
    {
      normals.push_back(vec3(m_shapes[mesh].mesh.normals[i], m_shapes[mesh].mesh.normals[i + 1], m_shapes[mesh].mesh.normals[i + 2]));
    }

    for (uint32 i = 0; i < m_shapes[mesh].mesh.texcoords.size(); i += 2)
    {
      uv.push_back(vec2(m_shapes[mesh].mesh.texcoords[i], m_shapes[mesh].mesh.texcoords[i + 1]));
    }

    m->set_indices(indices);
    m->set_vertices(vertices);
    m->set_normals(normals);
    m->set_uv(uv);

    m_shapes.clear();
    m_materials.clear();

    return m;
  }

  AssetManager::AssetManager()
  {

  }

  AssetManager::~AssetManager()
  {

  }

  void AssetManager::init()
  {
    initializeMaterials();
    initializeRenderPasses();
    initializeTextures();
  }

  void AssetManager::initializeMaterials()
  {
    // Adding engine materials
    addMaterial<mat::ScreenMaterial>();
    addMaterial<mat::Standard>();
    addMaterial<mat::Standard::Textured>();
    addMaterial<mat::Standard::TexturedCubemap>();
    addMaterial<mat::Unlit>();
    addMaterial<mat::Wireframe>();
    addMaterial<mat::Skybox>();
  }

  void AssetManager::initializeRenderPasses()
  {
    addRenderPass<mat::Screen>();
    addRenderPass<mat::BuildCubemap>();
    addRenderPass<mat::Negative>();
    addRenderPass<mat::Grayscale>();
  }

  void AssetManager::initializeTextures()
  {
    default_texture_.alloc()->set_name("Default Texture");
    default_texture_->set_type(TextureType::T2D);
    default_texture_->load("../../assets/textures/default.png");

    default_cubemap_.alloc()->set_name("Default Texture");
    default_cubemap_->set_type(TextureType::CubeMap);
    default_cubemap_->load("../../assets/textures/default.png", "../../assets/textures/default.png", "../../assets/textures/default.png", "../../assets/textures/default.png", "../../assets/textures/default.png", "../../assets/textures/default.png");
  }

  void AssetManager::addMaterial(ref_ptr<mat::Material> material)
  {
    materials_.push_back(material);
  }

  ref_ptr<mat::Material> AssetManager::shared_material(const char* shared_material_name) const
  {
    for (uint32 i = 0; i < materials_.size(); ++i)
    {
      if (materials_[i]->name() == shared_material_name)
      {
        return materials_[i];
      }
    }
    VXR_LOG(VXR_DEBUG_LEVEL_WARNING, "[WARNING]: Unknown shared material '%s'.\n", shared_material_name);
    return nullptr;
  }

  std::vector<ref_ptr<mat::Material>> AssetManager::shared_materials() const
  {
    return materials_;
  }

  void AssetManager::addRenderPass(ref_ptr<mat::RenderPass> render_pass)
  {
    render_passes_.push_back(render_pass);
  }

  ref_ptr<mat::RenderPass> AssetManager::shared_render_pass(const char* shared_render_pass_name) const
  {
    for (uint32 i = 0; i < render_passes_.size(); ++i)
    {
      if (render_passes_[i]->name() == shared_render_pass_name)
      {
        return render_passes_[i];
      }
    }
    VXR_LOG(VXR_DEBUG_LEVEL_WARNING, "[WARNING]: Unknown shared render pass '%s'.\n", shared_render_pass_name);
    return nullptr;
  }

  std::vector<ref_ptr<mat::RenderPass>> AssetManager::shared_render_passes() const
  {
    return render_passes_;
  }

  ref_ptr<Texture> AssetManager::default_texture() const
  {
    return default_texture_;
  }

  ref_ptr<Texture> AssetManager::default_cubemap() const
  {
    return default_cubemap_;
  }

}