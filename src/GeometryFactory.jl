module GeometryFactory

using Libdl

# Path to our shared library
const libpath = joinpath(@__DIR__, "..", "build", "libgeometryfactory.so")

# Global state
mutable struct State
    initialized::Bool
    render_task::Union{Task,Nothing}
    should_close::Bool
    State() = new(false, nothing, false)
end

const state = State()

function __init__()
    # Load the shared library
    global libgeometry = dlopen(libpath)
    
    # Get function pointers
    global init_geometry_factory = dlsym(libgeometry, :init_geometry_factory)
    global update_triangle_fan = dlsym(libgeometry, :update_triangle_fan)
    global render_frame = dlsym(libgeometry, :render_frame)
    global cleanup_geometry_factory = dlsym(libgeometry, :cleanup_geometry_factory)
end

"""
    initialize(width::Integer=800, height::Integer=600)

Initialize the geometry factory window with the specified dimensions.
"""
function initialize(width::Integer=800, height::Integer=600)
    if state.initialized
        @warn "GeometryFactory already initialized"
        return nothing
    end

    result = ccall(init_geometry_factory, Bool, (Int32, Int32), width, height)
    if !result
        error("Failed to initialize geometry factory")
    end

    state.initialized = true
    state.should_close = false
    
    # Start render loop in background
    state.render_task = @async begin
        try
            while !state.should_close && render()
                sleep(0.016)  # Cap at ~60 FPS
            end
        catch e
            @error "Render loop failed" exception=e
        finally
            ccall(cleanup_geometry_factory, Cvoid, ())
            state.initialized = false
            state.render_task = nothing
        end
    end

    return nothing
end

"""    update_vertices!(vertices::Vector{Float32})

Update the triangle fan vertices. Each vertex should be a 3D point (x,y,z).
The vertices array should be flattened, e.g. [x1,y1,z1, x2,y2,z2, ...].
"""
function update_vertices!(vertices::Vector{Float32})
    if !state.initialized
        error("GeometryFactory not initialized. Call initialize() first.")
    end

    num_vertices = length(vertices) ÷ 3
    if length(vertices) % 3 != 0
        error("Vertices array must contain 3D points (x,y,z)")
    end
    
    result = ccall(update_triangle_fan, Bool, (Ptr{Float32}, Csize_t), vertices, num_vertices)
    if !result
        error("Failed to update vertices")
    end
    return nothing
end

"""
    render()

Render one frame. Returns false if the window should close.
"""
function render()
    if !state.initialized
        return false
    end
    return ccall(render_frame, Bool, ())
end

"""
    cleanup()

Clean up all resources and close the window.
"""
function cleanup()
    if !state.initialized
        return nothing
    end

    # Signal that we want to close
    state.should_close = true

    # Wait for render task to finish
    if state.render_task !== nothing
        try
            # Give it a moment to finish naturally
            for _ in 1:10
                if istaskdone(state.render_task)
                    break
                end
                sleep(0.1)
            end
            # If still not done, force it to stop
            if !istaskdone(state.render_task)
                schedule(state.render_task, InterruptException(), error=true)
            end
        catch
        end
    end

    return nothing
end

"""
    create_triangle_fan(center::Tuple{Float32,Float32}, radius::Float32, num_points::Integer)

Create vertices for a triangle fan with the given center, radius and number of points.
"""
function create_triangle_fan(center::Tuple{Float32,Float32}, radius::Float32, num_points::Integer)
    vertices = Vector{Float32}()
    
    # Center point
    push!(vertices, center[1], center[2], 0.0f0)
    
    # Points around the circle
    for i in 0:num_points
        angle = 2π * i / num_points
        x = center[1] + radius * cos(angle)
        y = center[2] + radius * sin(angle)
        push!(vertices, x, y, 0.0f0)
    end
    
    return vertices
end

# Ensure cleanup on module unload
atexit(cleanup)

export initialize, update_vertices!, render, cleanup, create_triangle_fan

end # module
