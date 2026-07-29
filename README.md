# Raytracer

A C++23 raytracer that renders `.obj`/`.mtl` scenes to PNG.

## Dependencies

```
brew install libpng
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Usage example

```
./build/raytracer --input scene.obj --output result.png --width 800 --height 800
```

| Flag | Description |
|------|-------------|
| `-i, --input` | Input `.obj` file |
| `-o, --output` | Output `.png` file |
| `-w, --width` | Image width in pixels |
| `-h, --height` | Image height in pixels |
| `-m, --mode` | Render mode: `depth`, `normal`, `full` (default) |
| `-d, --depth` | Ray recursion depth (default: 5) |
| `--look-from` | Camera position as `x,y,z` |
| `--look-to` | Camera target as `x,y,z` |
