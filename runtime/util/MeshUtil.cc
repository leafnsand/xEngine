#include <graphics/config/MeshConfig.h>
#include "MeshUtil.h"

namespace xEngine {

static struct {
  // left-top point of face
  glm::vec3 p[6] = {
      glm::vec3(-1.0f,  1.0f, -1.0f), // top
      glm::vec3(-1.0f, -1.0f,  1.0f), // bottom
      glm::vec3(-1.0f,  1.0f,  1.0f), // front
      glm::vec3( 1.0f,  1.0f,  1.0f), // right
      glm::vec3( 1.0f,  1.0f, -1.0f), // back
      glm::vec3(-1.0f,  1.0f, -1.0f), // left
  };
  // right-direction vector
  glm::vec3 u[6] = {
      glm::vec3( 2.0f,  0.0f,  0.0f), // top
      glm::vec3( 2.0f,  0.0f,  0.0f), // bottom
      glm::vec3( 2.0f,  0.0f,  0.0f), // front
      glm::vec3( 0.0f,  0.0f, -2.0f), // right
      glm::vec3(-2.0f,  0.0f,  0.0f), // back
      glm::vec3( 0.0f,  0.0f,  2.0f), // left
  };
  // bottom-direction vector
  glm::vec3 v[6] = {
      glm::vec3( 0.0f,  0.0f,  2.0f), // top
      glm::vec3( 0.0f,  0.0f, -2.0f), // bottom
      glm::vec3( 0.0f, -2.0f,  0.0f), // front
      glm::vec3( 0.0f, -2.0f,  0.0f), // right
      glm::vec3( 0.0f, -2.0f,  0.0f), // back
      glm::vec3( 0.0f, -2.0f,  0.0f), // left
  };
} g_cube_prototype;

MeshUtil MeshUtil::Cube() {
  MeshUtil mesh;
  mesh.config_.layout.AddElement(VertexElementSemantic::kPosition, VertexElementFormat::kFloat3);
  mesh.config_.layout.AddElement(VertexElementSemantic::kTexcoord0, VertexElementFormat::kFloat2);
  mesh.config_.layout.AddElement(VertexElementSemantic::kNormal, VertexElementFormat::kFloat3);
  mesh.config_.vertex_count = 24;
  mesh.config_.vertex_usage = BufferUsage::kImmutable;
  mesh.config_.index_count = 36;
  mesh.config_.index_type = IndexFormat::kUint16;
  mesh.config_.index_usage = BufferUsage::kImmutable;
  mesh.BeginVertex();
  mesh.BeginIndex();
  for (size_t face = 0; face < 6; ++face) {
    auto normal = glm::normalize(glm::cross(g_cube_prototype.v[face], g_cube_prototype.u[face]));
    mesh.Vertex(VertexElementSemantic::kPosition, face * 4,
                g_cube_prototype.p[face].x,
                g_cube_prototype.p[face].y,
                g_cube_prototype.p[face].z)
        .Vertex(VertexElementSemantic::kPosition, face * 4 + 1,
                g_cube_prototype.p[face].x + g_cube_prototype.u[face].x,
                g_cube_prototype.p[face].y + g_cube_prototype.u[face].y,
                g_cube_prototype.p[face].z + g_cube_prototype.u[face].z)
        .Vertex(VertexElementSemantic::kPosition, face * 4 + 2,
                g_cube_prototype.p[face].x + g_cube_prototype.u[face].x + g_cube_prototype.v[face].x,
                g_cube_prototype.p[face].y + g_cube_prototype.u[face].y + g_cube_prototype.v[face].y,
                g_cube_prototype.p[face].z + g_cube_prototype.u[face].z + g_cube_prototype.v[face].z)
        .Vertex(VertexElementSemantic::kPosition, face * 4 + 3,
                g_cube_prototype.p[face].x + g_cube_prototype.v[face].x,
                g_cube_prototype.p[face].y + g_cube_prototype.v[face].y,
                g_cube_prototype.p[face].z + g_cube_prototype.v[face].z)
        .Quad16(static_cast<uint16>(face * 4),
                static_cast<uint16>(face * 4 + 1),
                static_cast<uint16>(face * 4 + 2),
                static_cast<uint16>(face * 4 + 3))
        .Vertex(VertexElementSemantic::kTexcoord0, face * 4, 0.0f, 0.0f)
        .Vertex(VertexElementSemantic::kTexcoord0, face * 4 + 1, 1.0f, 0.0f)
        .Vertex(VertexElementSemantic::kTexcoord0, face * 4 + 2, 1.0f, 1.0f)
        .Vertex(VertexElementSemantic::kTexcoord0, face * 4 + 3, 0.0f, 1.0f)
        .Vertex(VertexElementSemantic::kNormal, face * 4, normal.x, normal.y, normal.z)
        .Vertex(VertexElementSemantic::kNormal, face * 4 + 1, normal.x, normal.y, normal.z)
        .Vertex(VertexElementSemantic::kNormal, face * 4 + 2, normal.x, normal.y, normal.z)
        .Vertex(VertexElementSemantic::kNormal, face * 4 + 3, normal.x, normal.y, normal.z);
  }
  mesh.EndVertex();
  mesh.EndIndex();
  return mesh;
}

MeshUtil MeshUtil::Sphere(size_t divisions) {
  x_assert(divisions > 0);
  const auto count = divisions + 1;
  const auto step = 1.0f / static_cast<float32>(divisions);
  MeshUtil mesh;
  mesh.config_.layout.AddElement(VertexElementSemantic::kPosition, VertexElementFormat::kFloat3);
  mesh.config_.layout.AddElement(VertexElementSemantic::kTexcoord0, VertexElementFormat::kFloat2);
  mesh.config_.layout.AddElement(VertexElementSemantic::kNormal, VertexElementFormat::kFloat3);
  mesh.config_.vertex_count = 6 * count * count;
  mesh.config_.vertex_usage = BufferUsage::kImmutable;
  mesh.config_.index_count = 6 * 6 * divisions * divisions;
  mesh.config_.index_type = IndexFormat::kUint16;
  mesh.config_.index_usage = BufferUsage::kImmutable;
  mesh.BeginVertex();
  mesh.BeginIndex();
  for (size_t face = 0; face < 6; ++face) {
    for (size_t v = 0; v < count; ++v) {
      for (size_t u = 0; u < count; ++u) {
        glm::vec3 point(g_cube_prototype.p[face].x + (g_cube_prototype.u[face].x * u + g_cube_prototype.v[face].x * v) * step,
                        g_cube_prototype.p[face].y + (g_cube_prototype.u[face].y * u + g_cube_prototype.v[face].y * v) * step,
                        g_cube_prototype.p[face].z + (g_cube_prototype.u[face].z * u + g_cube_prototype.v[face].z * v) * step);
        auto point_square = point * point;
        glm::vec3 sphere_point(
            point.x * sqrtf(1.0f - (point_square.y + point_square.z) / 2.0f + point_square.y * point_square.z / 3.0f),
            point.y * sqrtf(1.0f - (point_square.x + point_square.z) / 2.0f + point_square.x * point_square.z / 3.0f),
            point.z * sqrtf(1.0f - (point_square.x + point_square.y) / 2.0f + point_square.x * point_square.y / 3.0f));
        mesh.Vertex(VertexElementSemantic::kPosition, (face * count + v) * count + u, sphere_point.x, sphere_point.y, sphere_point.z);
        auto normalized = glm::normalize(sphere_point);
        mesh.Vertex(VertexElementSemantic::kTexcoord0, (face * count + v) * count + u,
                    0.5f + atan2f(normalized.x, normalized.z) / (2.0f * glm::pi<float32>()),
                    0.5f - asinf(normalized.y) / glm::pi<float32>());
        mesh.Vertex(VertexElementSemantic::kNormal, (face * count + v) * count + u, normalized.x, normalized.y, normalized.z);
        if (u < divisions && v < divisions) {
          mesh.Quad16(static_cast<uint16>((face * count + v) * count + u),
                      static_cast<uint16>((face * count + v) * count + u + 1),
                      static_cast<uint16>((face * count + v + 1) * count + u + 1),
                      static_cast<uint16>((face * count + v + 1) * count + u));
        }
      }
    }
  }
  mesh.EndVertex();
  mesh.EndIndex();
  return mesh;
}

MeshUtil MeshUtil::Capsule(size_t divisions) {
  x_assert(divisions > 0);
  x_assert(divisions % 2 == 0);
  const auto count = divisions + 1;
  const auto middle = divisions / 2;
  const auto begin_beside_index = 2 * count * count;
  const auto begin_beside_quad_index = begin_beside_index + 4 * count * (count + 1);
  const auto step = 1.0f / static_cast<float32>(divisions);
  MeshUtil mesh;
  mesh.config_.layout.AddElement(VertexElementSemantic::kPosition, VertexElementFormat::kFloat3);
  mesh.config_.layout.AddElement(VertexElementSemantic::kTexcoord0, VertexElementFormat::kFloat2);
  mesh.config_.layout.AddElement(VertexElementSemantic::kNormal, VertexElementFormat::kFloat3);
  mesh.config_.vertex_count = 2 * count * count + 4 * count * (count + 1) + 2 * 4 * count;
  mesh.config_.vertex_usage = BufferUsage::kImmutable;
  mesh.config_.index_count = 6 * 6 * divisions * divisions + 4 * 6 * divisions;
  mesh.config_.index_type = IndexFormat::kUint16;
  mesh.config_.index_usage = BufferUsage::kImmutable;
  mesh.BeginVertex();
  mesh.BeginIndex();
  // top and bottom
  for (size_t face = 0; face < 2; ++face) {
    for (size_t v = 0; v < count; ++v) {
      for (size_t u = 0; u < count; ++u) {
        glm::vec3 point(g_cube_prototype.p[face].x + (g_cube_prototype.u[face].x * u + g_cube_prototype.v[face].x * v) * step,
                        g_cube_prototype.p[face].y + (g_cube_prototype.u[face].y * u + g_cube_prototype.v[face].y * v) * step,
                        g_cube_prototype.p[face].z + (g_cube_prototype.u[face].z * u + g_cube_prototype.v[face].z * v) * step);
        auto point_square = point * point;
        glm::vec3 sphere_point(
            point.x * sqrtf(1.0f - (point_square.y + point_square.z) / 2.0f + point_square.y * point_square.z / 3.0f),
            point.y * sqrtf(1.0f - (point_square.x + point_square.z) / 2.0f + point_square.x * point_square.z / 3.0f),
            point.z * sqrtf(1.0f - (point_square.x + point_square.y) / 2.0f + point_square.x * point_square.y / 3.0f));
        if (face == 0) {
          mesh.Vertex(VertexElementSemantic::kPosition, (face * count + v) * count + u, sphere_point.x, sphere_point.y + 1.0f, sphere_point.z);
        } else {
          mesh.Vertex(VertexElementSemantic::kPosition, (face * count + v) * count + u, sphere_point.x, sphere_point.y - 1.0f, sphere_point.z);
        }
        auto normalized = glm::normalize(sphere_point);
        mesh.Vertex(VertexElementSemantic::kTexcoord0, (face * count + v) * count + u,
                    0.5f + atan2f(normalized.x, normalized.z) / (2.0f * glm::pi<float32>()),
                    0.5f - asinf(normalized.y) / glm::pi<float32>());
        mesh.Vertex(VertexElementSemantic::kNormal, (face * count + v) * count + u, normalized.x, normalized.y, normalized.z);
        if (u < divisions && v < divisions) {
          mesh.Quad16(static_cast<uint16>((face * count + v) * count + u),
                      static_cast<uint16>((face * count + v) * count + u + 1),
                      static_cast<uint16>((face * count + v + 1) * count + u + 1),
                      static_cast<uint16>((face * count + v + 1) * count + u));
        }
      }
    }
  }
  // front bottom left right
  for (size_t face = 0; face < 4; ++face) {
    for (size_t v = 0; v < count; ++v) {
      for (size_t u = 0; u < count; ++u) {
        glm::vec3 point(g_cube_prototype.p[face + 2].x + (g_cube_prototype.u[face + 2].x * u + g_cube_prototype.v[face + 2].x * v) * step,
                        g_cube_prototype.p[face + 2].y + (g_cube_prototype.u[face + 2].y * u + g_cube_prototype.v[face + 2].y * v) * step,
                        g_cube_prototype.p[face + 2].z + (g_cube_prototype.u[face + 2].z * u + g_cube_prototype.v[face + 2].z * v) * step);
        auto point_square = point * point;
        glm::vec3 sphere_point(
            point.x * sqrtf(1.0f - (point_square.y + point_square.z) / 2.0f + point_square.y * point_square.z / 3.0f),
            point.y * sqrtf(1.0f - (point_square.x + point_square.z) / 2.0f + point_square.x * point_square.z / 3.0f),
            point.z * sqrtf(1.0f - (point_square.x + point_square.y) / 2.0f + point_square.x * point_square.y / 3.0f));
        auto normalized = glm::normalize(sphere_point);
        auto uv = glm::vec2(0.5f + atan2f(normalized.x, normalized.z) / (2.0f * glm::pi<float32>()),
                            0.5f - asinf(normalized.y) / glm::pi<float32>());
        if (v < middle) {
          mesh.Vertex(VertexElementSemantic::kPosition,
                      begin_beside_index + face * count * (count + 1) + v * count + u,
                      sphere_point.x, sphere_point.y + 1.0f, sphere_point.z)
              .Vertex(VertexElementSemantic::kTexcoord0,
                      begin_beside_index + face * count * (count + 1) + v * count + u,
                      uv.x, uv.y)
              .Vertex(VertexElementSemantic::kNormal,
                      begin_beside_index + face * count * (count + 1) + v * count + u,
                      normalized.x, normalized.y, normalized.z);
          if (u < divisions && v < divisions) {
            mesh.Quad16(static_cast<uint16>(begin_beside_index + face * count * (count + 1) + v * count + u),
                        static_cast<uint16>(begin_beside_index + face * count * (count + 1) + v * count + u + 1),
                        static_cast<uint16>(begin_beside_index + face * count * (count + 1) + (v + 1) * count + u + 1),
                        static_cast<uint16>(begin_beside_index + face * count * (count + 1) + (v + 1) * count + u));
          }
        } else if (v > middle) {
          mesh.Vertex(VertexElementSemantic::kPosition,
                      begin_beside_index + face * count * (count + 1) + (v + 1) * count + u,
                      sphere_point.x, sphere_point.y - 1.0f, sphere_point.z)
              .Vertex(VertexElementSemantic::kTexcoord0,
                      begin_beside_index + face * count * (count + 1) + (v + 1) * count + u,
                      uv.x, uv.y)
              .Vertex(VertexElementSemantic::kNormal,
                      begin_beside_index + face * count * (count + 1) + (v + 1) * count + u,
                      normalized.x, normalized.y, normalized.z);
          if (u < divisions && v < divisions) {
            mesh.Quad16(static_cast<uint16>(begin_beside_index + face * count * (count + 1) + (v + 1) * count + u),
                        static_cast<uint16>(begin_beside_index + face * count * (count + 1) + (v + 1) * count + u + 1),
                        static_cast<uint16>(begin_beside_index + face * count * (count + 1) + (v + 2) * count + u + 1),
                        static_cast<uint16>(begin_beside_index + face * count * (count + 1) + (v + 2) * count + u));
          }
        } else {
          mesh.Vertex(VertexElementSemantic::kPosition,
                      begin_beside_index + face * count * (count + 1) + v * count + u,
                      sphere_point.x, sphere_point.y + 1.0f, sphere_point.z)
              .Vertex(VertexElementSemantic::kTexcoord0,
                      begin_beside_index + face * count * (count + 1) + v * count + u,
                      uv.x, uv.y)
              .Vertex(VertexElementSemantic::kNormal,
                      begin_beside_index + face * count * (count + 1) + v * count + u,
                      normalized.x, normalized.y, normalized.z)
              .Vertex(VertexElementSemantic::kPosition,
                      begin_beside_index + face * count * (count + 1) + (v + 1) * count + u,
                      sphere_point.x, sphere_point.y - 1.0f, sphere_point.z)
              .Vertex(VertexElementSemantic::kTexcoord0,
                      begin_beside_index + face * count * (count + 1) + (v + 1) * count + u,
                      uv.x, uv.y)
              .Vertex(VertexElementSemantic::kNormal,
                      begin_beside_index + face * count * (count + 1) + (v + 1) * count + u,
                      normalized.x, normalized.y, normalized.z)
              .Vertex(VertexElementSemantic::kPosition,
                      begin_beside_quad_index + face * count * 2 + u,
                      sphere_point.x, sphere_point.y + 1.0f, sphere_point.z)
              .Vertex(VertexElementSemantic::kTexcoord0,
                      begin_beside_quad_index + face * count * 2 + u,
                      static_cast<float32>(face * divisions + u) / static_cast<float32>(4 * divisions), 0.0f)
              .Vertex(VertexElementSemantic::kNormal,
                      begin_beside_quad_index + face * count * 2 + u,
                      normalized.x, normalized.y, normalized.z)
              .Vertex(VertexElementSemantic::kPosition,
                      begin_beside_quad_index + face * count * 2 + u + count,
                      sphere_point.x, sphere_point.y - 1.0f, sphere_point.z)
              .Vertex(VertexElementSemantic::kTexcoord0,
                      begin_beside_quad_index + face * count * 2 + u + count,
                      static_cast<float32>(face * divisions + u) / static_cast<float32>(4 * divisions), 1.0f)
              .Vertex(VertexElementSemantic::kNormal,
                      begin_beside_quad_index + face * count * 2 + u + count,
                      normalized.x, normalized.y, normalized.z);
          if (u < divisions && v < divisions) {
            mesh.Quad16(static_cast<uint16>(begin_beside_quad_index + face * count * 2 + u),
                        static_cast<uint16>(begin_beside_quad_index + face * count * 2 + u + 1),
                        static_cast<uint16>(begin_beside_quad_index + face * count * 2 + u + count + 1),
                        static_cast<uint16>(begin_beside_quad_index + face * count * 2 + u + count))
                .Quad16(static_cast<uint16>(begin_beside_index + face * count * (count + 1) + (v + 1) * count + u),
                        static_cast<uint16>(begin_beside_index + face * count * (count + 1) + (v + 1) * count + u + 1),
                        static_cast<uint16>(begin_beside_index + face * count * (count + 1) + (v + 2) * count + u + 1),
                        static_cast<uint16>(begin_beside_index + face * count * (count + 1) + (v + 2) * count + u));
          }
        }
      }
    }
  }
  mesh.EndVertex();
  mesh.EndIndex();
  return mesh;
}

MeshUtil MeshUtil::Cylinder(size_t divisions) {
  x_assert(divisions > 2);
  static const auto square_root_of_two = sqrtf(2.0f);
  const auto count = divisions + 1;
  const auto step = 2.0f * glm::pi<float32>() / static_cast<float32>(divisions);
  const auto top_center_index = 0;
  const auto bottom_center_index = count + 1;
  const auto begin_top_index = top_center_index + 1;
  const auto begin_bottom_index = bottom_center_index + 1;
  const auto begin_beside_index = 2 * (count + 1);
  MeshUtil mesh;
  mesh.config_.layout.AddElement(VertexElementSemantic::kPosition, VertexElementFormat::kFloat3);
  mesh.config_.layout.AddElement(VertexElementSemantic::kTexcoord0, VertexElementFormat::kFloat2);
  mesh.config_.layout.AddElement(VertexElementSemantic::kNormal, VertexElementFormat::kFloat3);
  mesh.config_.vertex_count = 4 * divisions + 8;
  mesh.config_.vertex_usage = BufferUsage::kImmutable;
  mesh.config_.index_count = 12 * count;
  mesh.config_.index_type = IndexFormat::kUint16;
  mesh.config_.index_usage = BufferUsage::kImmutable;
  mesh.BeginVertex();
  mesh.BeginIndex();
  mesh.Vertex(VertexElementSemantic::kPosition, top_center_index, 0.0f, 1.0f, 0.0f)
      .Vertex(VertexElementSemantic::kTexcoord0, top_center_index, 0.5f, 0.5f)
      .Vertex(VertexElementSemantic::kNormal, top_center_index, 0.0f, 1.0f, 0.0f)
      .Vertex(VertexElementSemantic::kPosition, bottom_center_index, 0.0f, -1.0f, 0.0f)
      .Vertex(VertexElementSemantic::kTexcoord0, bottom_center_index, 0.5f, 0.5f)
      .Vertex(VertexElementSemantic::kNormal, bottom_center_index, 0.0f, -1.0f, 0.0f);
  for (size_t i = 0; i < count; ++i) {
    auto x = cosf(step * i);
    auto z = sinf(step * i);
    auto square_x = x * x;
    auto square_z = z * z;
    auto u = 0.5f * (1.0f + (sqrtf(2 + 2 * square_root_of_two * x + square_x - square_z) - sqrtf(2 - 2 * square_root_of_two * x + square_x - square_z)) * 0.5f);
    auto v = 0.5f * (1.0f + (sqrtf(2 + 2 * square_root_of_two * z - square_x + square_z) - sqrtf(2 - 2 * square_root_of_two * z - square_x + square_z)) * 0.5f);
    mesh.Vertex(VertexElementSemantic::kPosition, begin_top_index + i, x, 1.0f, z)
        .Vertex(VertexElementSemantic::kTexcoord0, begin_top_index + i, u, v)
        .Vertex(VertexElementSemantic::kNormal, begin_top_index + i, 0.0f, 1.0f, 0.0f)
        .Triangle16(static_cast<uint16>(begin_top_index + i),
                    static_cast<uint16>(begin_top_index + i + 1),
                    static_cast<uint16>(top_center_index))
        .Vertex(VertexElementSemantic::kPosition, begin_bottom_index + i, x, -1.0f, z)
        .Vertex(VertexElementSemantic::kTexcoord0, begin_bottom_index + i, u, v)
        .Vertex(VertexElementSemantic::kNormal, begin_bottom_index + i, 0.0f, -1.0f, 0.0f)
        .Triangle16(static_cast<uint16>(begin_bottom_index + i + 1),
                    static_cast<uint16>(begin_bottom_index + i),
                    static_cast<uint16>(bottom_center_index))
        .Vertex(VertexElementSemantic::kPosition, begin_beside_index + i * 2, x, 1.0f, z)
        .Vertex(VertexElementSemantic::kPosition, begin_beside_index + i * 2 + 1, x, -1.0f, z)
        .Vertex(VertexElementSemantic::kTexcoord0, begin_beside_index + i * 2, static_cast<float32>(i) / static_cast<float32>(divisions), 0.0f)
        .Vertex(VertexElementSemantic::kTexcoord0, begin_beside_index + i * 2 + 1, static_cast<float32>(i) / static_cast<float32>(divisions), 1.0f)
        .Vertex(VertexElementSemantic::kNormal, begin_beside_index + i * 2, x, 0.0f, z)
        .Vertex(VertexElementSemantic::kNormal, begin_beside_index + i * 2 + 1, x, 0.0f, z)
        .Quad16(static_cast<uint16>(begin_beside_index + (i + 1) * 2),
                static_cast<uint16>(begin_beside_index + i * 2),
                static_cast<uint16>(begin_beside_index + i * 2 + 1),
                static_cast<uint16>(begin_beside_index + (i + 1) * 2 + 1));
  }
  mesh.EndVertex();
  mesh.EndIndex();
  return mesh;
}

MeshUtil MeshUtil::Plane() {
  MeshUtil mesh;
  mesh.config_.layout.AddElement(VertexElementSemantic::kPosition, VertexElementFormat::kFloat3);
  mesh.config_.layout.AddElement(VertexElementSemantic::kTexcoord0, VertexElementFormat::kFloat2);
  mesh.config_.layout.AddElement(VertexElementSemantic::kNormal, VertexElementFormat::kFloat3);
  mesh.config_.vertex_count = 4;
  mesh.config_.vertex_usage = BufferUsage::kImmutable;
  mesh.config_.index_count = 6;
  mesh.config_.index_type = IndexFormat::kUint16;
  mesh.config_.index_usage = BufferUsage::kImmutable;
  mesh.BeginVertex()
      .Vertex(VertexElementSemantic::kPosition, 0, -0.5f, 0.0f, -0.5f)
      .Vertex(VertexElementSemantic::kPosition, 1, 0.5f, 0.0f, -0.5f)
      .Vertex(VertexElementSemantic::kPosition, 2, 0.5f, 0.0f, 0.5f)
      .Vertex(VertexElementSemantic::kPosition, 3, -0.5f, 0.0f, 0.5f)
      .Vertex(VertexElementSemantic::kTexcoord0, 0, 0.0f, 0.0f)
      .Vertex(VertexElementSemantic::kTexcoord0, 1, 1.0f, 0.0f)
      .Vertex(VertexElementSemantic::kTexcoord0, 2, 1.0f, 1.0f)
      .Vertex(VertexElementSemantic::kTexcoord0, 3, 0.0f, 1.0f)
      .Vertex(VertexElementSemantic::kNormal, 0, 0.0f, 1.0f, 0.0f)
      .Vertex(VertexElementSemantic::kNormal, 1, 0.0f, 1.0f, 0.0f)
      .Vertex(VertexElementSemantic::kNormal, 2, 0.0f, 1.0f, 0.0f)
      .Vertex(VertexElementSemantic::kNormal, 3, 0.0f, 1.0f, 0.0f)
      .EndVertex()
      .BeginIndex()
      .Quad16(0, 1, 2, 3)
      .EndIndex();
  return mesh;
}

MeshUtil &MeshUtil::BeginVertex() {
  x_assert(vertex_data_ == nullptr && config_.vertex_count > 0 && config_.layout.size > 0);
  vertex_data_ = Data::Create(config_.vertex_count * config_.layout.size);
  return *this;
}

MeshUtil &MeshUtil::Vertex(VertexElementSemantic semantic, size_t index, float32 x) {
  x_assert(vertex_data_ != nullptr);
  x_assert(index < config_.vertex_count);
  for (auto i = 0; i < config_.layout.element_count; ++i) {
    if (config_.layout.elements[i].semantic == semantic) {
      auto raw = static_cast<uint8 *>(vertex_data_->buffer()) + index * config_.layout.size + config_.layout.elements[i].offset;
      if (config_.layout.elements[i].format == VertexElementFormat::kFloat1) {
        auto pointer = reinterpret_cast<float32 *>(raw);
        *pointer = x;
        return *this;
      } else {
        x_error("unsupported semantic format\n");
      }
    }
  }
  x_error("no semantic in layout\n");
  return *this;
}

MeshUtil &MeshUtil::Vertex(VertexElementSemantic semantic, size_t index, float32 x, float32 y) {
  x_assert(vertex_data_ != nullptr);
  x_assert(index < config_.vertex_count);
  for (auto i = 0; i < config_.layout.element_count; ++i) {
    if (config_.layout.elements[i].semantic == semantic) {
      auto raw = static_cast<uint8 *>(vertex_data_->buffer()) + index * config_.layout.size + config_.layout.elements[i].offset;
      if (config_.layout.elements[i].format == VertexElementFormat::kFloat2) {
        auto pointer = reinterpret_cast<float32 *>(raw);
        *pointer++ = x;
        *pointer = y;
        return *this;
      } else if (config_.layout.elements[i].format == VertexElementFormat::kShort2) {
        auto pointer = reinterpret_cast<int16 *>(raw);
        *pointer++ = Convert<int16>(x);
        *pointer = Convert<int16>(y);
        return *this;
      } else if (config_.layout.elements[i].format == VertexElementFormat::kShort2Normalized) {
        auto pointer = reinterpret_cast<int16 *>(raw);
        *pointer++ = ConvertNormalized<int16>(x);
        *pointer = ConvertNormalized<int16>(y);
        return *this;
      } else {
        x_error("unsupported semantic format\n");
      }
    }
  }
  x_error("no semantic in layout\n");
  return *this;
}

MeshUtil &MeshUtil::Vertex(VertexElementSemantic semantic, size_t index, float32 x, float32 y, float32 z) {
  x_assert(vertex_data_ != nullptr);
  x_assert(index < config_.vertex_count);
  for (auto i = 0; i < config_.layout.element_count; ++i) {
    if (config_.layout.elements[i].semantic == semantic) {
      auto raw = static_cast<uint8 *>(vertex_data_->buffer()) + index * config_.layout.size + config_.layout.elements[i].offset;
      if (config_.layout.elements[i].format == VertexElementFormat::kFloat3) {
        auto pointer = reinterpret_cast<float32 *>(raw);
        *pointer++ = x;
        *pointer++ = y;
        *pointer = z;
        return *this;
      } else {
        x_error("unsupported semantic format\n");
      }
    }
  }
  x_error("no semantic in layout\n");
  return *this;
}

MeshUtil &MeshUtil::Vertex(VertexElementSemantic semantic, size_t index, float32 x, float32 y, float32 z, float32 w) {
  x_assert(vertex_data_ != nullptr);
  x_assert(index < config_.vertex_count);
  for (auto i = 0; i < config_.layout.element_count; ++i) {
    if (config_.layout.elements[i].semantic == semantic) {
      auto raw = static_cast<uint8 *>(vertex_data_->buffer()) + index * config_.layout.size + config_.layout.elements[i].offset;
      if (config_.layout.elements[i].format == VertexElementFormat::kFloat4) {
        auto pointer = reinterpret_cast<float32 *>(raw);
        *pointer++ = x;
        *pointer++ = y;
        *pointer++ = z;
        *pointer = w;
        return *this;
      } else if (config_.layout.elements[i].format == VertexElementFormat::kByte4) {
        auto pointer = reinterpret_cast<int8 *>(raw);
        *pointer++ = Convert<int8>(x);
        *pointer++ = Convert<int8>(y);
        *pointer++ = Convert<int8>(z);
        *pointer = Convert<int8>(w);
        return *this;
      } else if (config_.layout.elements[i].format == VertexElementFormat::kByte4Normalized) {
        auto pointer = reinterpret_cast<int8 *>(raw);
        *pointer++ = ConvertNormalized<int8>(x);
        *pointer++ = ConvertNormalized<int8>(y);
        *pointer++ = ConvertNormalized<int8>(z);
        *pointer = ConvertNormalized<int8>(w);
        return *this;
      } else if (config_.layout.elements[i].format == VertexElementFormat::kUnsignedByte4) {
        auto pointer = raw;
        *pointer++ = Convert<uint8>(x);
        *pointer++ = Convert<uint8>(y);
        *pointer++ = Convert<uint8>(z);
        *pointer = Convert<uint8>(w);
        return *this;
      } else if (config_.layout.elements[i].format == VertexElementFormat::kUnsignedByte4Normalized) {
        auto pointer = raw;
        *pointer++ = ConvertNormalized<uint8>(x);
        *pointer++ = ConvertNormalized<uint8>(y);
        *pointer++ = ConvertNormalized<uint8>(z);
        *pointer = ConvertNormalized<uint8>(w);
        return *this;
      } else if (config_.layout.elements[i].format == VertexElementFormat::kShort4) {
        auto pointer = reinterpret_cast<int16 *>(raw);
        *pointer++ = Convert<int16>(x);
        *pointer++ = Convert<int16>(y);
        *pointer++ = Convert<int16>(z);
        *pointer = Convert<int16>(w);
        return *this;
      } else if (config_.layout.elements[i].format == VertexElementFormat::kShort4Normalized) {
        auto pointer = reinterpret_cast<int16 *>(raw);
        *pointer++ = ConvertNormalized<int16>(x);
        *pointer++ = ConvertNormalized<int16>(y);
        *pointer++ = ConvertNormalized<int16>(z);
        *pointer = ConvertNormalized<int16>(w);
        return *this;
      } else {
        x_error("unsupported semantic format\n");
      }
    }
  }
  x_error("no semantic in layout\n");
  return *this;
}

MeshUtil &MeshUtil::EndVertex() {
  x_assert(vertex_data_ != nullptr);
  config_.vertex_data = vertex_data_;
  vertex_data_.reset();
  return *this;
}

MeshUtil &MeshUtil::BeginIndex() {
  x_assert(index_data_ == nullptr && config_.index_count > 0 && config_.index_type != IndexFormat::kNone);
  index_data_ = Data::Create(config_.index_count * SizeOfIndexFormat(config_.index_type));
  index_index_ = 0;
  return *this;
}

MeshUtil &MeshUtil::Index16(uint16 value) {
  x_assert(index_data_ != nullptr);
  x_assert(config_.index_type == IndexFormat::kUint16);
  x_assert(index_index_ < config_.index_count);
  static_cast<uint16 *>(index_data_->buffer())[index_index_] = value;
  ++index_index_;
  return *this;
}

MeshUtil &MeshUtil::Index32(uint32 value) {
  x_assert(index_data_ != nullptr);
  x_assert(config_.index_type == IndexFormat::kUint32);
  x_assert(index_index_ < config_.index_count);
  static_cast<uint32 *>(index_data_->buffer())[index_index_] = value;
  ++index_index_;
  return *this;
}

MeshUtil &MeshUtil::Triangle16(uint16 a, uint16 b, uint16 c) {
  x_assert(index_data_ != nullptr);
  x_assert(config_.index_type == IndexFormat::kUint16);
  x_assert(index_index_ + 3 <= config_.index_count);
  static_cast<uint16 *>(index_data_->buffer())[index_index_] = a;
  static_cast<uint16 *>(index_data_->buffer())[index_index_ + 1] = b;
  static_cast<uint16 *>(index_data_->buffer())[index_index_ + 2] = c;
  index_index_ += 3;
  return *this;
}

MeshUtil &MeshUtil::Triangle32(uint32 a, uint32 b, uint32 c) {
  x_assert(index_data_ != nullptr);
  x_assert(config_.index_type == IndexFormat::kUint32);
  x_assert(index_index_ + 3 <= config_.index_count);
  static_cast<uint32 *>(index_data_->buffer())[index_index_] = a;
  static_cast<uint32 *>(index_data_->buffer())[index_index_ + 1] = b;
  static_cast<uint32 *>(index_data_->buffer())[index_index_ + 2] = c;
  index_index_ += 3;
  return *this;
}

MeshUtil &MeshUtil::Quad16(uint16 a, uint16 b, uint16 c, uint16 d) {
  x_assert(index_data_ != nullptr);
  x_assert(config_.index_type == IndexFormat::kUint16);
  x_assert(index_index_ + 6 <= config_.index_count);
  static_cast<uint16 *>(index_data_->buffer())[index_index_] = a;
  static_cast<uint16 *>(index_data_->buffer())[index_index_ + 1] = b;
  static_cast<uint16 *>(index_data_->buffer())[index_index_ + 2] = c;
  static_cast<uint16 *>(index_data_->buffer())[index_index_ + 3] = a;
  static_cast<uint16 *>(index_data_->buffer())[index_index_ + 4] = c;
  static_cast<uint16 *>(index_data_->buffer())[index_index_ + 5] = d;
  index_index_ += 6;
  return *this;
}

MeshUtil &MeshUtil::Quad32(uint32 a, uint32 b, uint32 c, uint32 d) {
  x_assert(index_data_ != nullptr);
  x_assert(config_.index_type == IndexFormat::kUint32);
  x_assert(index_index_ + 6 <= config_.index_count);
  static_cast<uint32 *>(index_data_->buffer())[index_index_] = a;
  static_cast<uint32 *>(index_data_->buffer())[index_index_ + 1] = b;
  static_cast<uint32 *>(index_data_->buffer())[index_index_ + 2] = c;
  static_cast<uint32 *>(index_data_->buffer())[index_index_ + 3] = a;
  static_cast<uint32 *>(index_data_->buffer())[index_index_ + 4] = c;
  static_cast<uint32 *>(index_data_->buffer())[index_index_ + 5] = d;
  index_index_ += 6;
  return *this;
}

MeshUtil &MeshUtil::EndIndex() {
  x_assert(index_data_ != nullptr);
  config_.index_data = index_data_;
  index_data_.reset();
  index_index_ = 0;
  return *this;
}

} // namespace xEngine