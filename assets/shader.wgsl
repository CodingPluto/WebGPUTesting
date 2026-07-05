
struct Uniforms {
  time: f32,
  deltaTime: f32
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;

struct VertexOutput{
  @builtin(position) position: vec4f,
  @location(0) color: vec3f
};

@vertex
fn vs_main(@location(0) position: vec2f, @location(1) color: vec3f) -> VertexOutput{
  var out : VertexOutput;

  var offset = vec2f(cos(uniforms.time * 2), sin(uniforms.time * 2)) * 0.2;
  out.position = vec4f(position.x - 0.6 + offset.x, position.y - 0.3 + offset.y, 0.0, 1);
  out.position = vec4f(out.position.x, out.position.y, 0.0, 1);
  out.color = vec3f(color.x, color.y + cos(uniforms.time) * 0.2, color.z + sin(uniforms.time) * 0.2);

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