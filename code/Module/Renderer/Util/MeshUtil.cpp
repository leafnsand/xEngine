#include "Renderer/Util/MeshUtil.h"

#include <AzCore/Math/Vector3.h>

namespace Module
{
    static struct {
        // left-top point of face
        AZ::Vector3 p[6] = {
            AZ::Vector3(-1.0f,  1.0f, -1.0f), // top
            AZ::Vector3(-1.0f, -1.0f,  1.0f), // bottom
            AZ::Vector3(-1.0f,  1.0f,  1.0f), // front
            AZ::Vector3(1.0f,  1.0f,  1.0f), // right
            AZ::Vector3(1.0f,  1.0f, -1.0f), // back
            AZ::Vector3(-1.0f,  1.0f, -1.0f), // left
        };
        // right-direction vector
        AZ::Vector3 u[6] = {
            AZ::Vector3(2.0f,  0.0f,  0.0f), // top
            AZ::Vector3(2.0f,  0.0f,  0.0f), // bottom
            AZ::Vector3(2.0f,  0.0f,  0.0f), // front
            AZ::Vector3(0.0f,  0.0f, -2.0f), // right
            AZ::Vector3(-2.0f,  0.0f,  0.0f), // back
            AZ::Vector3(0.0f,  0.0f,  2.0f), // left
        };
        // bottom-direction vector
        AZ::Vector3 v[6] = {
            AZ::Vector3(0.0f,  0.0f,  2.0f), // top
            AZ::Vector3(0.0f,  0.0f, -2.0f), // bottom
            AZ::Vector3(0.0f, -2.0f,  0.0f), // front
            AZ::Vector3(0.0f, -2.0f,  0.0f), // right
            AZ::Vector3(0.0f, -2.0f,  0.0f), // back
            AZ::Vector3(0.0f, -2.0f,  0.0f), // left
        };
    } g_cube_prototype;

    /*

    void MeshBuilder::Cube(Mesh& mesh)
    {
        mesh.Clear();

        mesh.Begin(CubeStr);
        for (size_t face = 0; face < 6; ++face)
        {
            auto normal = g_cube_prototype.v[face].Cross(g_cube_prototype.u[face]).GetNormalized();
            mesh.VertexPos(g_cube_prototype.p[face].GetX(),
                g_cube_prototype.p[face].GetY(),
                g_cube_prototype.p[face].GetZ())
                .VertexPos(g_cube_prototype.p[face].GetX() + g_cube_prototype.u[face].GetX(),
                    g_cube_prototype.p[face].GetY() + g_cube_prototype.u[face].GetY(),
                    g_cube_prototype.p[face].GetZ() + g_cube_prototype.u[face].GetZ())
                .VertexPos(g_cube_prototype.p[face].GetX() + g_cube_prototype.u[face].GetX() + g_cube_prototype.v[face].GetX(),
                    g_cube_prototype.p[face].GetY() + g_cube_prototype.u[face].GetY() + g_cube_prototype.v[face].GetY(),
                    g_cube_prototype.p[face].GetZ() + g_cube_prototype.u[face].GetZ() + g_cube_prototype.v[face].GetZ())
                .VertexPos(g_cube_prototype.p[face].GetX() + g_cube_prototype.v[face].GetX(),
                    g_cube_prototype.p[face].GetY() + g_cube_prototype.v[face].GetY(),
                    g_cube_prototype.p[face].GetZ() + g_cube_prototype.v[face].GetZ())
                .Quad16(static_cast<AZ::u16>(face * 4),
                    static_cast<AZ::u16>(face * 4 + 1),
                    static_cast<AZ::u16>(face * 4 + 2),
                    static_cast<AZ::u16>(face * 4 + 3))
                .VertexUV(0.0f, 0.0f)
                .VertexUV(1.0f, 0.0f)
                .VertexUV(1.0f, 1.0f)
                .VertexUV(0.0f, 1.0f)
                .VertexNormal(normal.GetX(), normal.GetY(), normal.GetZ())
                .VertexNormal(normal.GetX(), normal.GetY(), normal.GetZ())
                .VertexNormal(normal.GetX(), normal.GetY(), normal.GetZ())
                .VertexNormal(normal.GetX(), normal.GetY(), normal.GetZ());
        }
        mesh.End();
        mesh.MarkVertexDirty();
        mesh.MarkIndexDirty();
    }

    void MeshBuilder::Sphere(Mesh& mesh, size_t divisions)
    {
        AZ_Assert(divisions > 0, "divisions should > 0");

        mesh.Clear();

        const auto count = divisions + 1;
        const auto step = 1.0f / static_cast<float>(divisions);

        mesh.Begin(SphereStr);
        for (size_t face = 0; face < 6; ++face) {
            for (size_t v = 0; v < count; ++v) {
                for (size_t u = 0; u < count; ++u) {
                    AZ::Vector3 point(g_cube_prototype.p[face].GetX() + (g_cube_prototype.u[face].GetX() * static_cast<float>(u) + g_cube_prototype.v[face].GetX() * static_cast<float>(v)) * step,
                        g_cube_prototype.p[face].GetY() + (g_cube_prototype.u[face].GetY() * static_cast<float>(u) + g_cube_prototype.v[face].GetY() * static_cast<float>(v)) * step,
                        g_cube_prototype.p[face].GetZ() + (g_cube_prototype.u[face].GetZ() * static_cast<float>(u) + g_cube_prototype.v[face].GetZ() * static_cast<float>(v)) * step);
                    auto point_square = point * point;
                    AZ::Vector3 sphere_point(
                        point.GetX() * sqrtf(1.0f - (point_square.GetY() + point_square.GetZ()) / 2.0f + point_square.GetY() * point_square.GetZ() / 3.0f),
                        point.GetY() * sqrtf(1.0f - (point_square.GetX() + point_square.GetZ()) / 2.0f + point_square.GetX() * point_square.GetZ() / 3.0f),
                        point.GetZ() * sqrtf(1.0f - (point_square.GetX() + point_square.GetY()) / 2.0f + point_square.GetX() * point_square.GetY() / 3.0f));
                    mesh.VertexPos(sphere_point.GetX(), sphere_point.GetY(), sphere_point.GetZ());
                    auto normalized = sphere_point.GetNormalized(); 
                    mesh.VertexUV(
                        0.5f + atan2f(normalized.GetX(), normalized.GetZ()) / AZ::Constants::TwoPi,
                        0.5f - asinf(normalized.GetY()) / AZ::Constants::Pi);
                    mesh.VertexNormal(normalized.GetX(), normalized.GetY(), normalized.GetZ());
                    if (u < divisions && v < divisions) {
                        mesh.Quad16(static_cast<AZ::u16>((face * count + v) * count + u),
                            static_cast<AZ::u16>((face * count + v) * count + u + 1),
                            static_cast<AZ::u16>((face * count + v + 1) * count + u + 1),
                            static_cast<AZ::u16>((face * count + v + 1) * count + u));
                    }
                }
            }
        }
        mesh.End();
        mesh.MarkVertexDirty();
        mesh.MarkIndexDirty();
    }

    void MeshBuilder::Capsule(Mesh& mesh, size_t divisions)
    {
        AZ_Assert(divisions > 0, "divisions should > 0");
        AZ_Assert(divisions % 2 == 0, "divisions should % 2 == 0");

        mesh.Clear();

        const auto count = divisions + 1;
        const auto middle = divisions / 2;
        const auto begin_beside_index = 2 * count * count;
        const auto begin_beside_quad_index = begin_beside_index + 4 * count * (count + 1);
        const auto step = 1.0f / static_cast<float>(divisions);
        // top and bottom
        mesh.Begin(CapsuleStr);
        for (size_t face = 0; face < 2; ++face) {
            for (size_t v = 0; v < count; ++v) {
                for (size_t u = 0; u < count; ++u) {
                    AZ::Vector3 point(g_cube_prototype.p[face].GetX() + (g_cube_prototype.u[face].GetX() * static_cast<float>(u) + g_cube_prototype.v[face].GetX() * static_cast<float>(v)) * step,
                        g_cube_prototype.p[face].GetY() + (g_cube_prototype.u[face].GetY() * static_cast<float>(u) + g_cube_prototype.v[face].GetY() * static_cast<float>(v)) * step,
                        g_cube_prototype.p[face].GetZ() + (g_cube_prototype.u[face].GetZ() * static_cast<float>(u) + g_cube_prototype.v[face].GetZ() * static_cast<float>(v)) * step);
                    auto point_square = point * point;
                    AZ::Vector3 sphere_point(
                        point.GetX() * sqrtf(1.0f - (point_square.GetY() + point_square.GetZ()) / 2.0f + point_square.GetY() * point_square.GetZ() / 3.0f),
                        point.GetY() * sqrtf(1.0f - (point_square.GetX() + point_square.GetZ()) / 2.0f + point_square.GetX() * point_square.GetZ() / 3.0f),
                        point.GetZ() * sqrtf(1.0f - (point_square.GetX() + point_square.GetY()) / 2.0f + point_square.GetX() * point_square.GetY() / 3.0f));
                    if (face == 0) {
                        mesh.VertexPos(sphere_point.GetX(), sphere_point.GetY() + 1.0f, sphere_point.GetZ());
                    }
                    else {
                        mesh.VertexPos(sphere_point.GetX(), sphere_point.GetY() - 1.0f, sphere_point.GetZ());
                    }
                    auto normalized = sphere_point.GetNormalized();
                    mesh.VertexUV(0.5f + atan2f(normalized.GetX(), normalized.GetZ()) / AZ::Constants::TwoPi,
                        0.5f - asinf(normalized.GetY()) / AZ::Constants::Pi);
                    mesh.VertexNormal(normalized.GetX(), normalized.GetY(), normalized.GetZ());
                    if (u < divisions && v < divisions) {
                        mesh.Quad16(static_cast<AZ::u16>((face * count + v) * count + u),
                            static_cast<AZ::u16>((face * count + v) * count + u + 1),
                            static_cast<AZ::u16>((face * count + v + 1) * count + u + 1),
                            static_cast<AZ::u16>((face * count + v + 1) * count + u));
                    }
                }
            }
        }
        // front bottom left right
        for (size_t face = 0; face < 4; ++face) {
            for (size_t v = 0; v < count; ++v) {
                for (size_t u = 0; u < count; ++u) {
                    AZ::Vector3 point(g_cube_prototype.p[face + 2].GetX() + (g_cube_prototype.u[face + 2].GetX() * static_cast<float>(u) + g_cube_prototype.v[face + 2].GetX() * static_cast<float>(v)) * step,
                        g_cube_prototype.p[face + 2].GetY() + (g_cube_prototype.u[face + 2].GetY() * static_cast<float>(u) + g_cube_prototype.v[face + 2].GetY() * static_cast<float>(v)) * step,
                        g_cube_prototype.p[face + 2].GetZ() + (g_cube_prototype.u[face + 2].GetZ() * static_cast<float>(u) + g_cube_prototype.v[face + 2].GetZ() * static_cast<float>(v)) * step);
                    auto point_square = point * point;
                    AZ::Vector3 sphere_point(
                        point.GetX() * sqrtf(1.0f - (point_square.GetY() + point_square.GetZ()) / 2.0f + point_square.GetY() * point_square.GetZ() / 3.0f),
                        point.GetY() * sqrtf(1.0f - (point_square.GetX() + point_square.GetZ()) / 2.0f + point_square.GetX() * point_square.GetZ() / 3.0f),
                        point.GetZ() * sqrtf(1.0f - (point_square.GetX() + point_square.GetY()) / 2.0f + point_square.GetX() * point_square.GetY() / 3.0f));
                    auto normalized = sphere_point.GetNormalized();
                    auto uv = AZ::Vector2(0.5f + atan2f(normalized.GetX(), normalized.GetZ()) / AZ::Constants::TwoPi,
                        0.5f - asinf(normalized.GetY()) / AZ::Constants::Pi);
                    if (v < middle) {
                        mesh.VertexPos(sphere_point.GetX(), sphere_point.GetY() + 1.0f, sphere_point.GetZ())
                            .VertexUV(uv.GetX(), uv.GetY())
                            .VertexNormal(normalized.GetX(), normalized.GetY(), normalized.GetZ());
                        if (u < divisions && v < divisions) {
                            mesh.Quad16(static_cast<AZ::u16>(begin_beside_index + face * count * (count + 1) + v * count + u),
                                static_cast<AZ::u16>(begin_beside_index + face * count * (count + 1) + v * count + u + 1),
                                static_cast<AZ::u16>(begin_beside_index + face * count * (count + 1) + (v + 1) * count + u + 1),
                                static_cast<AZ::u16>(begin_beside_index + face * count * (count + 1) + (v + 1) * count + u));
                        }
                    }
                    else if (v > middle) {
                        mesh.VertexPos(sphere_point.GetX(), sphere_point.GetY() - 1.0f, sphere_point.GetZ())
                            .VertexUV(uv.GetX(), uv.GetY())
                            .VertexNormal(normalized.GetX(), normalized.GetY(), normalized.GetZ());
                        if (u < divisions && v < divisions) {
                            mesh.Quad16(static_cast<AZ::u16>(begin_beside_index + face * count * (count + 1) + (v + 1) * count + u),
                                static_cast<AZ::u16>(begin_beside_index + face * count * (count + 1) + (v + 1) * count + u + 1),
                                static_cast<AZ::u16>(begin_beside_index + face * count * (count + 1) + (v + 2) * count + u + 1),
                                static_cast<AZ::u16>(begin_beside_index + face * count * (count + 1) + (v + 2) * count + u));
                        }
                    }
                    else {
                        mesh.VertexPos(sphere_point.GetX(), sphere_point.GetY() + 1.0f, sphere_point.GetZ())
                            .VertexUV(uv.GetX(), uv.GetY())
                            .VertexNormal(normalized.GetX(), normalized.GetY(), normalized.GetZ())
                            .VertexPos(sphere_point.GetX(), sphere_point.GetY() - 1.0f, sphere_point.GetZ())
                            .VertexUV(uv.GetX(), uv.GetY())
                            .VertexNormal(normalized.GetX(), normalized.GetY(), normalized.GetZ())
                            .VertexPos(sphere_point.GetX(), sphere_point.GetY() + 1.0f, sphere_point.GetZ())
                            .VertexUV(static_cast<float>(face * divisions + u) / static_cast<float>(4 * divisions), 0.0f)
                            .VertexNormal(normalized.GetX(), normalized.GetY(), normalized.GetZ())
                            .VertexPos(sphere_point.GetX(), sphere_point.GetY() - 1.0f, sphere_point.GetZ())
                            .VertexUV(static_cast<float>(face * divisions + u) / static_cast<float>(4 * divisions), 1.0f)
                            .VertexNormal(normalized.GetX(), normalized.GetY(), normalized.GetZ());
                        if (u < divisions && v < divisions) {
                            mesh.Quad16(static_cast<AZ::u16>(begin_beside_quad_index + face * count * 2 + u),
                                static_cast<AZ::u16>(begin_beside_quad_index + face * count * 2 + u + 1),
                                static_cast<AZ::u16>(begin_beside_quad_index + face * count * 2 + u + count + 1),
                                static_cast<AZ::u16>(begin_beside_quad_index + face * count * 2 + u + count))
                                .Quad16(static_cast<AZ::u16>(begin_beside_index + face * count * (count + 1) + (v + 1) * count + u),
                                    static_cast<AZ::u16>(begin_beside_index + face * count * (count + 1) + (v + 1) * count + u + 1),
                                    static_cast<AZ::u16>(begin_beside_index + face * count * (count + 1) + (v + 2) * count + u + 1),
                                    static_cast<AZ::u16>(begin_beside_index + face * count * (count + 1) + (v + 2) * count + u));
                        }
                    }
                }
            }
        }
        mesh.End();
        mesh.MarkVertexDirty();
        mesh.MarkIndexDirty();
    }

    void MeshBuilder::Cylinder(Mesh& mesh, size_t divisions)
    {
        AZ_Assert(divisions > 2, "divisions should > 2");

        mesh.Clear();

        static const auto square_root_of_two = sqrtf(2.0f);
        const auto count = divisions + 1;
        const auto step = 2.0f * AZ::Constants::Pi / static_cast<float>(divisions);
        const auto top_center_index = 0;
        const auto bottom_center_index = count + 1;
        const auto begin_top_index = top_center_index + 1;
        const auto begin_bottom_index = bottom_center_index + 1;
        const auto begin_beside_index = 2 * (count + 1);

        mesh.Begin(CylinderStr);
        mesh.VertexPos(0.0f, 1.0f, 0.0f)
            .VertexUV(0.5f, 0.5f)
            .VertexNormal(0.0f, 1.0f, 0.0f)
            .VertexPos(0.0f, -1.0f, 0.0f)
            .VertexUV(0.5f, 0.5f)
            .VertexNormal(0.0f, -1.0f, 0.0f);

        for (size_t i = 0; i < count; ++i) {
            auto x = cosf(step * i);
            auto z = sinf(step * i);
            auto square_x = x * x;
            auto square_z = z * z;
            auto u = 0.5f * (1.0f + (sqrtf(2 + 2 * square_root_of_two * x + square_x - square_z) - sqrtf(2 - 2 * square_root_of_two * x + square_x - square_z)) * 0.5f);
            auto v = 0.5f * (1.0f + (sqrtf(2 + 2 * square_root_of_two * z - square_x + square_z) - sqrtf(2 - 2 * square_root_of_two * z - square_x + square_z)) * 0.5f);
            mesh.VertexPos(x, 1.0f, z)
                .VertexUV(u, v)
                .VertexNormal(0.0f, 1.0f, 0.0f)
                .Triangle16(static_cast<AZ::u16>(begin_top_index + i),
                    static_cast<AZ::u16>(begin_top_index + i + 1),
                    static_cast<AZ::u16>(top_center_index))
                .VertexPos(x, -1.0f, z)
                .VertexUV(u, v)
                .VertexNormal(0.0f, -1.0f, 0.0f)
                .Triangle16(static_cast<AZ::u16>(begin_bottom_index + i + 1),
                    static_cast<AZ::u16>(begin_bottom_index + i),
                    static_cast<AZ::u16>(bottom_center_index))
                .VertexPos(x, 1.0f, z)
                .VertexPos(x, -1.0f, z)
                .VertexUV(static_cast<float>(i) / static_cast<float>(divisions), 0.0f)
                .VertexUV(static_cast<float>(i) / static_cast<float>(divisions), 1.0f)
                .VertexNormal(x, 0.0f, z)
                .VertexNormal(x, 0.0f, z)
                .Quad16(static_cast<AZ::u16>(begin_beside_index + (i + 1) * 2),
                    static_cast<AZ::u16>(begin_beside_index + i * 2),
                    static_cast<AZ::u16>(begin_beside_index + i * 2 + 1),
                    static_cast<AZ::u16>(begin_beside_index + (i + 1) * 2 + 1));
        }
        mesh.End();
        mesh.MarkVertexDirty();
        mesh.MarkIndexDirty();
    }

    void MeshBuilder::Plane(Mesh& mesh)
    {
        mesh.Clear();

        mesh.Begin(PlaneStr);
        mesh.VertexPos(-0.5f, 0.0f, -0.5f)
          .VertexPos(0.5f, 0.0f, -0.5f)
          .VertexPos(0.5f, 0.0f, 0.5f)
          .VertexPos(-0.5f, 0.0f, 0.5f)
          .VertexUV(0.0f, 0.0f)
          .VertexUV(1.0f, 0.0f)
          .VertexUV(1.0f, 1.0f)
          .VertexUV(0.0f, 1.0f)
          .VertexNormal(0.0f, 1.0f, 0.0f)
          .VertexNormal(0.0f, 1.0f, 0.0f)
          .VertexNormal(0.0f, 1.0f, 0.0f)
          .VertexNormal(0.0f, 1.0f, 0.0f)
          .Quad16(0, 1, 2, 3);
        mesh.End();
        mesh.MarkVertexDirty();
        mesh.MarkIndexDirty();
    }

    */
}
