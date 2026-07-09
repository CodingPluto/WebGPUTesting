
struct Uniforms {
  time: f32,
  delta_time: f32,
  projection_matrix: mat4x4<f32>
}

struct ObjectData {
  //color : vec3f,
  model_matrix: mat4x4f
}


@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<storage, read> objects: array<ObjectData>;

struct VertexOutput{
  @builtin(position) position: vec4f,
  @location(0) color: vec3f
};

@vertex
fn vs_main(@location(0) position: vec2f, @location(1) color: vec3f, @builtin(instance_index) instance_index: u32) -> VertexOutput{
  var out : VertexOutput;
  let model_matrix = objects[instance_index].model_matrix;

  out.position = uniforms.projection_matrix * model_matrix * vec4f(position.x, position.y, 0.0, 1);
  out.color = color;

  return out;
}
@fragment
fn fs_main(@builtin(position) position: vec4f, @location(0) color: vec3f) -> @location(0) vec4f {
  return vec4f(color, 1.0); // if linear mapping of color and it looks wrong, adjust by pow(color, vec3f(2.2))
}

@group(0) @binding(1) var<storage, read> input_buffer: array<f32, 64>;
@group(0) @binding(2) var<storage, read_write> output_buffer: array<f32, 64>;

@compute @workgroup_size(32)
fn cs_main(){
  // Compute Shader
}