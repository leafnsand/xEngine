#pragma once

#include "Renderer/Base/Mesh.h"

namespace Module
{
    class MeshBuilder
    {
    public:
        static void Cube(Mesh& mesh);

        static void Sphere(Mesh& mesh, size_t divisions);

        static void Capsule(Mesh& mesh, size_t divisions);

        static void Cylinder(Mesh& mesh, size_t divisions);

        static void Plane(Mesh& mesh);
    };
}
