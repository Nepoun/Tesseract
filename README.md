# Tesseract

Tesseract (Temporary name) is an experimental node-based image processing tool written in C++.

The project started as a simple utility for applying shader pipelines and bulk processing to images using tools such as SLK_img2pixel, but gradually evolved into a more flexible visual graph system inspired by node-based compositors and shader editors.

The goal is to allow users to visually create image-processing pipelines by connecting nodes together, enabling workflows such as shader passes, pixel-art conversion, procedural operations, and batch processing.

---

## Current Status

This project is currently in a very early stage of development.

Most systems are still experimental and subject to major architectural and workflow changes.

The current focus is on:
- Building the node graph system
- Defining the rendering pipeline
- Integrating shader-based image processing
- Experimenting with workflow and usability

---

## Planned Features

### Core
- [ ] Node graph editor
- [ ] Image input/output nodes
- [ ] Graph serialization (save/load)
- [ ] Real-time preview
- [ ] Undo/redo support

### Processing
- [ ] Shader-based image processing
- [ ] Custom user-written shader support
- [ ] SLK_img2pixel integration
- [ ] Procedural image operations
- [ ] Layer compositing
- [ ] Mask support

### Batch Workflow
- [ ] Bulk image processing
- [ ] Folder input/output
- [ ] Preset pipelines
- [ ] Export automation

### Future Ideas
- [ ] Shape generation nodes
- [ ] Procedural texture generation
- [ ] Animation support
- [ ] Plugin system
- [ ] GPU compute workflows

---

## Tech Stack

Currently using:

- C++
- Dear ImGui
- SDL2 backend
- OpenGL 3 renderer
- GLSL shaders

---

## Third-Party Tools

This project uses SLK_img2pixel as part of its image-processing workflow and node system.

SLK_img2pixel was created by Captain4LK and can be found here:

https://captain4lk.itch.io/slk-img2pixel

---

## Inspiration

Inspired by tools such as:

- Pixel Composer
- Shader Graph
- Substance Designer
- Material Maker
- Compositor-style workflows

---

## Goals

The main goal of this project is to explore:
- Node-based workflows
- GPU image processing
- Procedural graphics pipelines
- Tool development in C++
- Real-time rendering architectures

while also serving as a flexible image-processing utility for pixel-art and shader experimentation.