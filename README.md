# Tesseract
Tesseract (temporary name) is an experimental node-based image processing tool written in C++.

The project started as a simple utility for bulk processing images using SLK_img2pixel, but gradually evolved into a more flexible visual graph system inspired by node-based compositors and shader editors.

The goal is to allow users to visually build image-processing pipelines by connecting nodes together — enabling workflows like pixel-art conversion, color grading, dithering, and batch processing.

---

## Current Status

Early stage. Most systems are still experimental and subject to major changes.

Current focus:
- Solidifying the node graph architecture
- Expanding the preset and processing pipeline
- Improving usability and workflow

---

## Using This Repository

This repository is currently meant as a code showcase rather than a ready-to-use tool.

Even with all the project files, you would still need to download SLK_img2pixel separately
and do some manual configuration to get things running. In the future I plan to improve
this, making setup easier and the project more self-contained. Even removing completely SLK_img2pixel from the project is planned

---

## Features

### Node Graph
- [x] Node graph editor (Dear ImGui + ImnNodes)
- [x] Typed pin system (String, Preset, Int, Float, Bool)
- [x] Data propagation between nodes via runtime link registry
- [x] Polymorphic node architecture (extend `NodeBase` to add nodes)
- [x] Graph serialization and deserialization (JSON)
- [ ] Upd
- [ ] Real-time preview node
- [ ] Undo/redo

### Nodes
- [x] Preset node (define and verify a preset file)
- [x] Preset editor node (edit all processing parameters visually)
- [x] Conversor node (batch image conversion with async execution and progress tracking)
- [x] Path formatter node
- [x] String creator node
- [x] Data viewer node
- [x] Palette viewer node
- [ ] Image preview node
- [ ] Mask node
- [ ] Layer compositor node

### Processing
- [x] SLK_img2pixel integration
- [x] Color adjustments (brightness, contrast, saturation, hue, gamma, tint)
- [x] Transform controls (scale, offset, sampling mode)
- [x] Dithering (mode, amount, target colors, alpha threshold, KMeans++)
- [x] Blur and sharpening
- [x] Preset palette preservation across edits
- [ ] Shader-based processing
- [ ] Custom user-written shaders
- [ ] Procedural image operations

### Batch Workflow
- [x] Folder input with automatic image discovery (.png, .jpg, .bmp, .tga)
- [x] Async batch conversion with per-image progress counter
- [x] Configurable output folder
- [x] Preset-driven pipeline execution
- [ ] Export automation
- [ ] Re-run / redo converted images

---

## Tech Stack

- C++
- Dear ImGui
- ImnNodes
- SDL2
- OpenGL 3.3
- nlohmann/json

---

## Third-Party Tools

Uses [SLK_img2pixel](https://captain4lk.itch.io/slk-img2pixel) by Captain4LK as part of its image processing pipeline.

---

## Inspiration

- Pixel Composer
- Shader Graph (Unity)
- Substance Designer
- Material Maker
- Compositor-style workflows

---

## Goals

Explore node-based tool development in C++ while building something actually useful for pixel-art and shader workflows.