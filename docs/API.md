# The Geometry Factory API Reference

## Julia API

### Functions

```julia
# Initialize graphics window
initialize(width::Integer=800, height::Integer=600)
```
Creates an OpenGL window for visualization. Must be called before any other functions.

```julia
# Update displayed geometry
update_vertices!(vertices::Vector{Float32})
```
Updates the displayed geometry with new vertex data. Each vertex should be a 3D point (x,y,z).

```julia
# Create triangle fan vertices
create_triangle_fan(
    center::Tuple{Float32,Float32},  # Center point (x,y)
    radius::Float32,                 # Radius
    num_points::Integer              # Number of points
)
```
Helper function to create triangle fan vertices. Returns a Vector{Float32} ready for `update_vertices!`.

```julia
# Clean up resources
cleanup()
```
Cleans up all resources. Call this when you're done.

### Implementation Details

#### State Management
The Julia interface uses a mutable struct `State` to track:
- Initialization status
- Render task handle
- Window close requests

#### Resource Management
1. OpenGL resources managed in C
2. Automatic cleanup on Julia exit
3. Background render loop

## C API

### Functions

```c
// Initialize OpenGL context and window
bool init_geometry_factory(int width, int height);
```
Initializes OpenGL context and creates a window. Returns true on success.

```c
// Update vertex data for triangle fan
bool update_triangle_fan(float* vertices, size_t count);
```
Updates the vertex data for rendering. Each vertex should be a 3D point (x,y,z).

```c
// Render one frame
bool render_frame(void);
```
Renders one frame. Returns false if window should close.

```c
// Clean up resources
void cleanup_geometry_factory(void);
```
Cleans up all resources.
