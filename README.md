# Island Demo

This is an ocean scene rendering project I have been working on to learn OpenGL and create a framework I can use for 
other graphics programming projects.

## Features

* OpenGL interface
  * Abstracted Model and Mesh system
  * Custom loading of .obj and .mtl
  * Textures
  * CubeMaps
  * Shaders
  * FrameBuffers
  * etc.
* PBR
  * Blinn-Phong shading
  * Environment reflections
* Fluid simulation
  * Sum of sines method
  * Central Difference normals
* Effects
  * HDR Bloom
  * Depth-based fog
 
## To-do

* Normal mapping
* Specular mapping
* Schlick-Fresnel lighting/reflections

## 1/13/24

Began implementing shadows through depth based approach in post-processing rather than typical fragment shader implementations.

![ezgif-5-c1bffe9675](https://github.com/dylan-berndt/Island/assets/33700799/d3e0acff-94d5-4f17-a9ff-3cb30e9d4350)

First iteration of shadows

![ezgif-7-681bad78c9](https://github.com/dylan-berndt/Island/assets/33700799/e0329522-fbc9-46a6-9f4f-becde684334d)
 
## 1/12/24

Made some changes to effects and a few QoL improvements. Changed model loading to support more vertices and objects. 

![ezgif-7-96244a467b](https://github.com/dylan-berndt/Island/assets/33700799/94cbf2cc-81c9-475c-a356-a3a2434eecb0)

## 1/11/24

Added CubeMaps and subsequently added a skybox. Improved fog and added reflections to the water. Improved model loading to deal
with multiple materials per object. Changed project structure and added palm plant.

![ezgif-1-7165957ce5](https://github.com/dylan-berndt/Water-Demo/assets/33700799/bd7d235c-25df-4066-a198-e97f454201c6)

## 1/6/24

![ezgif-4-c6e04d1e42](https://github.com/dylan-berndt/Water-Demo/assets/33700799/20af5d23-70f3-42de-a6bd-92144ed7da6d)

## 1/5/24

Model loading is more fleshed out now, can load base, specular, and emissive colors along with a base texture.
Next to implement is the rest of Materials, including normal maps and others.

![ezgif-3-6a1c98eb79](https://github.com/dylan-berndt/Water-Demo/assets/33700799/bb6b71af-9942-4de0-b9dd-8a5b5faf05aa)

## 1/4/24

Began implementing Model loading. I couldn't get Assimp to build correctly at any point over the past entire day, 
so I'm making my own system. It currently only works with simple triangular meshes in .obj files, but I expect
to expand it very soon. 

A sample island exported from Blender rendered with the first iteration of the default shader:

![Screenshot (263)](https://github.com/dylan-berndt/Water-Demo/assets/33700799/98ffe87b-de58-43fc-8d6a-488e0c9012c1)
![Screenshot (264)](https://github.com/dylan-berndt/Water-Demo/assets/33700799/ddafdd8d-45bb-4b13-81c2-430122414928)

More in-depth model loading and camera controls:

![ezgif com-video-to-gif-converter](https://github.com/dylan-berndt/Water-Demo/assets/33700799/9c159a61-d235-4023-bf32-49d8aed80a6c)

## 1/3/24

No major visual changes, but transition from direct use of Vertex Array Objects to a Mesh approach. Also standardized 
shader uniforms such as projection and view. About to begin development implementing Models and model loading.

## 12/29/23

Framebuffers implemented, Vertex Array Objects made more flexible, some adjustments to the effects. 
Textures also implemented but not heavily utilized yet. Indexed Arrays will need similar flexibility upgrades.

![ezgif-1-7a5e82feab](https://github.com/dylan-berndt/Water-Demo/assets/33700799/73b841b0-264e-42bf-8ca4-5618e1171037)

Implemented bloom for fun.

![ezgif-5-405160b07d](https://github.com/dylan-berndt/Water-Demo/assets/33700799/a092767f-325d-48a3-97fc-96a6bf60e052)

## 12/28/23

So far, I have implemented Vertex Array Objects, Shaders, and basic lighting.
I'm currently working on Textures, Framebuffers, and reflections in hopes of creating a skybox and a better simulation.

![Screenshot (262)](https://github.com/dylan-berndt/Water-Demo/assets/33700799/95bdc8b7-82b6-4b62-ba57-25ed8732f6d2)
