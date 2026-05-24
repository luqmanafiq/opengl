# LuqRenderer

A real-time 3D rendering engine built in **C++20 and OpenGL 3.3 Core Profile**.

![C++](https://img.shields.io/badge/C%2B%2B-20-blue)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3_Core-green)
![CMake](https://img.shields.io/badge/CMake-3.20+-red)

---

## Features

- **Blinn-Phong lighting model** — ambient, diffuse, and specular components
- **Multiple light types** — directional light (sun) and up to 8 dynamic point lights
- **Physically-correct attenuation** — constant/linear/quadratic falloff for point lights
- **Gamma correction** — gamma 2.2 applied in the fragment shader
- **4× MSAA** — multisampled anti-aliasing
- **First-person camera** — WASD movement, free-look, scroll-to-zoom
- **Correct normal transformation** — uses the normal matrix (transpose of inverse model)
- **Procedural geometry** — cube, UV sphere, and subdivided plane built at runtime
- **Animated lights** — point lights orbit the scene each frame
- **Wireframe mode** — toggle with `F`
- **Clean abstractions** — `Shader`, `Mesh`, `Camera`, and `Renderer` classes

---

## Architecture

```
LuqRenderer/
├── src/
│   ├── main.cpp        — Window, input, scene setup, render loop
│   ├── Shader.h/cpp    — GLSL shader program wrapper
│   ├── Camera.h/cpp    — FPS camera (Euler angles, view/projection matrices)
│   ├── Mesh.h/cpp      — VAO/VBO/EBO abstraction + geometry factories
│   └── Renderer.h/cpp  — Scene graph, light management, draw dispatch
├── shaders/
│   ├── phong.vert/frag      — Blinn-Phong with directional + point lights
│   └── lightbulb.vert/frag  — Unlit emissive shader for light visualisers
├── third_party/
│   └── glad/               — OpenGL function loader (see setup below)
└── CMakeLists.txt
```

---

## Build Instructions

### Prerequisites

| Tool | Version |
|------|---------|
| CMake | ≥ 3.20 |
| C++ compiler | Clang 14+ / GCC 12+ / MSVC 2022 |
| GLFW | ≥ 3.3 |
| GLM | ≥ 0.9.9 |
| GLAD | OpenGL 3.3 Core |

### macOS (Homebrew)

```bash
brew install cmake glfw glm
```

### Ubuntu / Debian

```bash
sudo apt install cmake libglfw3-dev libglm-dev
```

### GLAD setup

1. Visit https://glad.dav1d.de/
2. Select **Language: C**, **Specification: OpenGL**, **API gl: 3.3**, **Profile: Core**
3. Click *Generate* and download the zip
4. Place the contents into `third_party/glad/` so you have:
   - `third_party/glad/include/glad/glad.h`
   - `third_party/glad/include/KHR/khrplatform.h`
   - `third_party/glad/src/glad.c`

### Compile

```bash
git clone <repo-url> && cd rendering-engine
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
./LuqRenderer
```

---

## Controls

| Key / Input | Action |
|-------------|--------|
| `W A S D` | Move forward / left / backward / right |
| `Space` / `Left Shift` | Move up / down |
| Mouse | Look around |
| Scroll wheel | Zoom (FOV) |
| `ESC` | Toggle cursor lock |
| `F` | Toggle wireframe mode |

---

## Rendering Pipeline Overview

```
CPU                              GPU
───                              ───
Scene objects                    Vertex shader
  └─ Mesh (VAO/VBO/EBO)    ──►    └─ MVP transform
  └─ Material                      └─ Normal matrix transform
                                   └─ Pass FragPos, Normal to frag
Light data                       Fragment shader
  └─ DirectionalLight       ──►    └─ Blinn-Phong per light type
  └─ PointLight[]                  └─ Attenuation for point lights
                                   └─ Gamma correction (2.2)
Camera                             └─ Final colour output
  └─ View matrix            ──►
  └─ Projection matrix
```

---

## Potential Extensions

- Shadow mapping (shadow depth pass + PCF filtering)
- Normal/bump mapping
- PBR (Cook-Torrance BRDF, metallic/roughness workflow)
- Deferred rendering (G-buffer)
- SSAO, bloom (post-processing framebuffers)
- glTF model loading (via tinygltf)
- Texture support (stb_image)
