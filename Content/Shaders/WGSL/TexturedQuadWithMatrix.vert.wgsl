// Input vertex attributes
struct VertexInput {
    @location(0) Position: vec3<f32>,
    @location(1) TexCoord: vec2<f32>,
};

// Output to the fragment shader
struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) outTexCoord: vec2<f32>,
};

// Uniform block
@group(2) @binding(0) var<uniform> UniformBlock: mat4x4<f32>;

@vertex
fn main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    output.outTexCoord = input.TexCoord;
    output.gl_Position = UniformBlock * vec4<f32>(input.Position, 1.0);
    return output;
}
