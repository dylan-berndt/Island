# Water Demo

This is an ocean scene rendering project I have been working on to learn OpenGL and create a framework I can use for 
other graphics programming projects.

## 1/11/23

Added CubeMaps and subsequently added a skybox. Improved fog and added reflections to the water. Improved model loading to deal
with multiple materials per object. Changed project structure and added palm plant.

![ezgif-1-7165957ce5](https://github.com/dylan-berndt/Water-Demo/assets/33700799/bd7d235c-25df-4066-a198-e97f454201c6)

## 1/6/23

![ezgif-4-c6e04d1e42](https://github.com/dylan-berndt/Water-Demo/assets/33700799/20af5d23-70f3-42de-a6bd-92144ed7da6d)

## 1/5/23

Model loading is more fleshed out now, can load base, specular, and emissive colors along with a base texture.
Next to implement is the rest of Materials, including normal maps and others.

![ezgif-3-6a1c98eb79](https://github.com/dylan-berndt/Water-Demo/assets/33700799/bb6b71af-9942-4de0-b9dd-8a5b5faf05aa)

## 1/4/23 

Began implementing Model loading. I couldn't get Assimp to build correctly at any point over the past entire day, 
so I'm making my own system. It currently only works with simple triangular meshes in .obj files, but I expect
to expand it very soon. 

A sample island exported from Blender rendered with the first iteration of the default shader:

![Screenshot (263)](https://github.com/dylan-berndt/Water-Demo/assets/33700799/98ffe87b-de58-43fc-8d6a-488e0c9012c1)
![Screenshot (264)](https://github.com/dylan-berndt/Water-Demo/assets/33700799/ddafdd8d-45bb-4b13-81c2-430122414928)

More in-depth model loading and camera controls:

![ezgif com-video-to-gif-converter](https://github.com/dylan-berndt/Water-Demo/assets/33700799/9c159a61-d235-4023-bf32-49d8aed80a6c)

## 1/3/23

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
