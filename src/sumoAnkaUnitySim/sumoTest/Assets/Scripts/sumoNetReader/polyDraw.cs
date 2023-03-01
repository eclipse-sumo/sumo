using System.Linq;
using UnityEngine;
using Random = UnityEngine.Random;
using UnityEditor;
using System.Collections.Generic;

public class polyDraw
    {
    Vector2[] vertices2D;
    Vector3[] vertices3D_B;
    Vector3[] vertices3D_T;
    float height = 10;
    public polyDraw(Vector2[] vertices2D, Vector3[] vertices3D, float height)
    {
        this.height = height;
        this.vertices2D = vertices2D;
        vertices3D_B = vertices3D;
        vertices3D_T = new Vector3[vertices3D.Length];
        for (int i = 0; i < vertices3D.Length; i++)
        {
            vertices3D_T[i] = vertices3D[i] + new Vector3(0, height, 0);
        }
       

    }


         
    public Mesh[] drawPoly()
    {
        //var vertices3D = System.Array.ConvertAll<Vector2, Vector3>(vertices2D, v =>v);

        // Use the triangulator to get indices for creating triangles
        var triangulator = new Triangulator(vertices2D);
        var indices = triangulator.Triangulate();

        // Generate a color for each vertex
        var colors = Enumerable.Range(0, vertices3D_B.Length)
            .Select(i => Random.ColorHSV())
            .ToArray();

        

        // Create the mesh
        var meshTop = new Mesh
        {
            vertices = vertices3D_T,
            triangles = indices,
            colors = colors
        };


        List<int> indicesS = new List<int>();
        List<Vector3> verticesS = new List<Vector3>();
        List<Vector2> verticesS2d = new List<Vector2>();
        foreach (Vector3 tv in vertices3D_T)
        {
            verticesS.Add(new Vector3(tv.x, tv.y, tv.z));
            verticesS2d.Add(new Vector3(tv.x, tv.y ));
        }
        foreach (Vector3 bv in vertices3D_B)
        {
            verticesS.Add(new Vector3(bv.x, bv.y, bv.z));
            verticesS2d.Add(new Vector3(bv.x, bv.y ));
        }
        

        // Generate a color for each vertex
        var colorsNear = Enumerable.Range(0, verticesS.Count())
            .Select(i => Random.ColorHSV())
            .ToArray();

        int indexB = 0;
        int indexT = vertices3D_B.Count();
        int sumQuads = vertices3D_T.Count();
        for (int i = 0; i < sumQuads; i++)
        {
            // handle closing triangles
            if (i == (sumQuads - 1))
            {
                // second to last triangle (1st in quad)
                indicesS.Add(indexB);
                indicesS.Add(0);                // flipped with 3. index
                indicesS.Add(indexT);           // flipped with 2. index

                // last triangle (2nd in quad)
                indicesS.Add(0);
                indicesS.Add(vertices3D_B.Count());    // flipped with 6. index
                indicesS.Add(indexT);           // flipped with 5. index
            }
            // handle normal case
            else
            {
                // triangle 1
                indicesS.Add(indexB);
                indicesS.Add(indexB + 1);
                indicesS.Add(indexT);

                // triangle 2
                indicesS.Add(indexB + 1);
                indicesS.Add(indexT + 1);
                indicesS.Add(indexT);

                // increment bottom and top index
                indexB++;
                indexT++;
            }
        }


        var meshNear = new Mesh
        {
            vertices = verticesS.ToArray(),
            triangles = indicesS.ToArray(),
            colors = colorsNear
        };


        System.Array.Reverse(vertices3D_T);
        var meshButom = new Mesh
        {
            vertices = vertices3D_B,
            triangles = indices,
            colors = colors
        };


        
        meshTop.uv = vertices2D;
        meshNear.uv = verticesS2d.ToArray();


        meshTop.RecalculateNormals();
        meshTop.RecalculateBounds();
        meshButom.RecalculateNormals();
        meshButom.RecalculateBounds();
        meshNear.RecalculateNormals();
        meshNear.RecalculateBounds();

        Mesh[] mesh = { doubleSidedMesh(meshTop), doubleSidedMesh(meshNear) };

        return mesh;

        //// Set up game object with mesh;
        //var meshRenderer = gameObject.AddComponent<MeshRenderer>();
        //meshRenderer.material = new Material(Shader.Find("Sprites/Default"));

        //var filter = gameObject.AddComponent<MeshFilter>();
        //filter.mesh = mesh;
    }

    
    public Mesh doubleSidedMesh(Mesh mesh)
    {

        
        var vertices = mesh.vertices;
        
        var normals = mesh.normals;
        var szV = vertices.Length;
        var newVerts = new Vector3[szV * 2];
        
        var newNorms = new Vector3[szV * 2];
        for (var j = 0; j < szV; j++)
        {
            // duplicate vertices and uvs:
            newVerts[j] = newVerts[j + szV] = vertices[j];
            
            // copy the original normals...
            newNorms[j] = normals[j];
            // and revert the new ones
            newNorms[j + szV] = -normals[j];
        }
        var triangles = mesh.triangles;
        var szT = triangles.Length;
        var newTris = new int[szT * 2]; // double the triangles
        for (var i = 0; i < szT; i += 3)
        {
            // copy the original triangle
            newTris[i] = triangles[i];
            newTris[i + 1] = triangles[i + 1];
            newTris[i + 2] = triangles[i + 2];
            // save the new reversed triangle
            var j = i + szT;
            newTris[j] = triangles[i] + szV;
            newTris[j + 2] = triangles[i + 1] + szV;
            newTris[j + 1] = triangles[i + 2] + szV;
        }
        mesh.vertices = newVerts;
        
        mesh.normals = newNorms;
        mesh.triangles = newTris; // assign triangles last!

        return mesh;

    }
    
}

